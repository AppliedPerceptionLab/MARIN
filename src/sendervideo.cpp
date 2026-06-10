#include "sendervideo.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  CONSTRUCTOR  //////////////////////////////////////////////
SenderVideo::SenderVideo( QObject *parent, QString thisaddress, int port, int w, int h, int frw, int frh ) : Sender( parent, thisaddress, port ) {
    
    setSending( INITIALLY_SENDING_VIDEO );
    qInfo() << "[SenderVideo] Will send image/video to " << getServerAddress();
    width = w;
    height = h;
    // Always use the same buffer (size of BGRA ( w * h * 4 ) even when it's something smaller
    converted = new uchar[ CAMERA_WIDTH * CAMERA_HEIGHT * 4 ];
    srcPic->colorFormat = FormatI420;
    srcPic->picWidth = width;
    srcPic->picHeight = height;
    
    switch( VIDEO_MODE ){
        case VideoModes::Image_Y:
            // TODO?
            break;
        case VideoModes::Image_RGB:
            // TODO?
            break;
        case VideoModes::Image_BGRA:
            // TODO?
            break;
        case VideoModes::Video_I420:
            I420StreamEncoder = new I420Encoder( (char*)"" );
            encoder = I420StreamEncoder;
            // TODO?
            break;
        case VideoModes::Video_H264:
            h264StreamEncoder = new H264Encoder( (char*)OH264_CONFIG_FILE_PATH );
            h264StreamEncoder->SetSpeed( ENCODER_SPEED );
            encoder = h264StreamEncoder;
            break;
        case VideoModes::VIDEOMODE_UNDEFINED:
            std::cerr << "Video mode must be defined in configuration file." << std::endl;
            exit(1);
            break;
    }
    
    protocol = IMAGE_TRANSMISSION_PROTOCOL;

    switch( protocol ){
        case TransmissionProtocol::TCP:
            break;
        case TransmissionProtocol::UDP:
            udpServerSocket->SetIPAddress( thisaddress.toStdString().c_str() );
            udpServerSocket->SetPortNumber( port );
            break;
        case TransmissionProtocol::PROTOCOL_UNDEFINED:
            std::cerr << "Transmission protocol must be defined in configuration file." << std::endl;
            exit(1);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  DESTRUCTOR   ////////////////////////////////////////////
SenderVideo::~SenderVideo(){
    delete[] send_buffer;
    delete[] converted;
    delete srcPic;
    delete camera;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////   COPY(ON_PREVIEW)_FRAME   //////////////////////////////////
void SenderVideo::copy_frame(QVideoFrame frame){
    copy_number++;
    //map and init:
    frame.map( QVideoFrame::ReadOnly );
    int curr_height = frame.height();
    int curr_width = frame.width();
    
    //TODO, all of this is for NV12, should add support for ARGB eventually
    QVideoFrameFormat::PixelFormat curr_format = frame.pixelFormat();
    int sizeY = frame.mappedBytes( 0 );
    int sizeUV = frame.mappedBytes( 1 );
    int size = sizeY + sizeUV;
    
    //qDebug() << "size:" << size;
    QImage::Format imageFormat = QVideoFrameFormat::imageFormatFromPixelFormat( frame.pixelFormat() );
    
    //on first call, init buffers:
    if( slot1.data == nullptr
            || curr_height != height
            || curr_width != width
            || curr_format != format
            || input_image_size != size ){
        input_image_size = size;
        if( curr_format == QVideoFrameFormat::Format_NV12 ){
            qDebug() << "[SenderVideo] QVideoFrame input format set to QVideoFrameFormat::Format_NV12.";
            buffer_size = curr_width*curr_height*3/2;
        }else if( curr_format == QVideoFrameFormat::Format_NV21 ){
            qDebug() << "[SenderVideo] QVideoFrame input format set to QVideoFrameFormat::Format_NV21.";
            buffer_size = curr_width*curr_height*3/2;
        }else if( curr_format == QVideoFrameFormat::Format_ARGB8888 ){
            qDebug() << "[SenderVideo] QVideoFrame input format set to QVideoFrameFormat::Format_ARGB8888.";
            buffer_size = curr_width*curr_height*4;
        }else{
            qCritical() << "[SenderVideo] The format " << curr_format << " is not yet supported.";
        }
        if( size != buffer_size ){ qWarning() << "[SenderVideo] The size of the buffer in memory is" << size << "but it should have been " << buffer_size; }

        slot1.frame_number = copy_number;
        slot1.size = size;
        slot1.data = new uchar[size];
        slot2.frame_number = copy_number;
        slot2.size = size;
        slot2.data = new uchar[size];
        send_buffer = new uchar[size];
        width = curr_width;
        height = curr_height;
        bytes_per_line = frame.bytesPerLine( 0 );
        format = curr_format;
        init_done = true;
        //set encoder parameters:
        setEncoder( width, height );
    }

    //looks like a two frame buffer is enough.
    //TODO, implement the version for ARGB as well (currently NV12 only)
    if( !newest_is_1 ) {
        if( !slot1_being_read ){
            slot1_being_written = true;
            memcpy( slot1.data, frame.bits( 0 ), frame.mappedBytes( 0 ) );
            memcpy( slot1.data + frame.mappedBytes( 0 ), frame.bits( 1 ), frame.mappedBytes( 1 ) );
            slot1.frame_number = copy_number;
            slot1_being_written = false;
            newest_is_1 = true;
        }else{
            qInfo() << "Nothing to do.";
        }
    }else if( newest_is_1 ){
        if( !slot2_being_read ){
            slot2_being_written = true;
            memcpy( slot2.data, frame.bits( 0 ), frame.mappedBytes( 0 ) );
            memcpy( slot2.data + frame.mappedBytes( 0 ), frame.bits( 1 ), frame.mappedBytes( 1 ) );
            slot2.frame_number = copy_number;
            slot2_being_written = false;
            newest_is_1 = false;
        }else{
            qInfo() << "Nothing to do.";
        }
    }
    frame.unmap();
}

#include <convert.h>
#include <algorithm>
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////   SEND   ////////////////////////////////////////////////
bool SenderVideo::send(){
    
    if( !connected ){
        std::cout << "[SenderVideo] Trying to connect..." << std::endl;
        connect( "video" );
        return false;
    }
    if( !connected || !init_done ){
        return false;
    }
    
    if( sending() ){
        // copy data from buffer:
        if ( newest_is_1 ){
            if ( !slot1_being_written ){
                slot1_being_read = true;
                memcpy( send_buffer, slot1.data, (uint)(buffer_size) );
                slot1_being_read = false;
            }else{
                qDebug() << "[SenderVideo] Buffer.slot1 is in use.";
            }
        }else if ( !newest_is_1 ){
            if ( !slot2_being_written ){
                slot2_being_read = true;
                memcpy( send_buffer, slot2.data, (uint)(buffer_size) );
                slot2_being_read = false;
            }else{
                qDebug() << "[SenderVideo] Buffer.slot2 is in use.";
            }
        }
        //send data:
        /////////////////////////// IMAGE_MESSAGE, Y PLANE ONLY ///////////////////////////
        if( VIDEO_MODE == VideoModes::Image_Y ){
            int   size[]     = { width, height, 1 };                    // image dimension
            float spacing[]  = { 1.0, 1.0, 1.0 };                       // spacing (mm/pixel)
            int   scalarType = igtl::ImageMessage::TYPE_UINT8;          // scalar type
            
            imageMessage = igtl::ImageMessage::New();
            imageMessage->SetDimensions( size );
            imageMessage->SetSpacing( spacing );
            imageMessage->SetScalarType( scalarType );
            imageMessage->SetDeviceName( THIS_DEVICE_NAME );
            imageMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
            imageMessage->SetMessageID( msgID++ );
            imageMessage->AllocateScalars();
            
            if( !convertToI420() ) return false;
            
            //set timestamp:
            ServerTimer->GetTime();
            imageMessage->SetTimeStamp( ServerTimer );
            
            //TODO, do same for other formats

            memcpy( imageMessage->GetScalarPointer(), converted, imageMessage->GetPackSize() );
            sendCurrentImageMessage();
            
        /////////////////////////// IMAGE_MESSAGE, BGRA ///////////////////////////
        }else if( VIDEO_MODE == VideoModes::Image_BGRA ){
            //TODO if for CAMERA_FORMAT (like below)
            int   size[]     = { width, height, 1 };                    // image dimension
            float spacing[]  = { 1., 1., 1. };                          // spacing (mm/pixel)
            int   scalarType = igtl::ImageMessage::TYPE_UINT8;          // scalar type
            
            imageMessage = igtl::ImageMessage::New();
            imageMessage->SetDimensions( size );
            imageMessage->SetSpacing( spacing );
            imageMessage->SetScalarType( scalarType );
            imageMessage->SetNumComponents( 4 );
            imageMessage->SetDeviceName( THIS_DEVICE_NAME );
            imageMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
            imageMessage->SetMessageID( msgID++ );
            imageMessage->AllocateScalars();
            
            //TODO, need to convert
            
            //set timestamp:
            ServerTimer->GetTime();
            imageMessage->SetTimeStamp( ServerTimer );
            
            memcpy( imageMessage->GetScalarPointer(), send_buffer, imageMessage->GetPackSize() );
            sendCurrentImageMessage();
            
        /////////////////////////// IMAGE_MESSAGE, RGB ///////////////////////////
        }else if( VIDEO_MODE == VideoModes::Image_RGB ){
            int   size[]     = { width, height, 1 };                    // image dimension
            float spacing[]  = { 1., 1., 1. };                          // spacing (mm/pixel)
            int   scalarType = igtl::ImageMessage::TYPE_UINT8;          // scalar type
            
            imageMessage = igtl::ImageMessage::New();
            imageMessage->SetDimensions( size );
            imageMessage->SetSpacing( spacing );
            imageMessage->SetScalarType( scalarType );
            imageMessage->SetNumComponents( 3 );
            imageMessage->SetDeviceName( THIS_DEVICE_NAME );
            imageMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
            imageMessage->SetMessageID( msgID++ );
            imageMessage->AllocateScalars();
            
            if( !convertToRGB() ) return false;
            
            //set timestamp:
            ServerTimer->GetTime();
            imageMessage->SetTimeStamp( ServerTimer );
            
            //TODO NEED TO CONVERT, SINCE THE INCOMING IMAGE IS NOT NV12

//            uchar * RGB_buffer = new uchar[ width*height*3 ];
//            for( int i = 0; i < width; i++ ){
//                for( int j = 0; j < height; j++ ){
//                    RGB_buffer[ i*3 + j*width*3 + 0] = send_buffer[ i*4 + j*width*4 + 2 ];
//                    RGB_buffer[ i*3 + j*width*3 + 1] = send_buffer[ i*4 + j*width*4 + 1 ];
//                    RGB_buffer[ i*3 + j*width*3 + 2] = send_buffer[ i*4 + j*width*4 + 0 ];
//                }
//            }
            //TODO THIS SEGFAULTS, SIZE IS INCORRECT PROBABLY
            memcpy( imageMessage->GetScalarPointer(), converted, imageMessage->GetPackSize() );
            sendCurrentImageMessage();

        /////////////////////////// VIDEO_MESSAGE, I420 ///////////////////////////
        }else if( VIDEO_MODE == VideoModes::Video_I420 ){
            videoMessage = igtl::VideoMessage:: New();
            videoMessage->SetDeviceName( THIS_DEVICE_NAME );
            videoMessage->SetCodecType( IGTL_VIDEO_CODEC_NAME_I420 );
            videoMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
            
            if( !convertToI420() ) return false;
            
            srcPic->data[0] = converted;
            srcPic->data[1] = converted + width * height;
            srcPic->data[2] = converted + width * height * 5/4;
            srcPic->stride[0] = width;
            srcPic->stride[1] = width / 2;
            srcPic->stride[2] = width / 2;
            
            //set timestamp:
            ServerTimer->GetTime();
            videoMessage->SetTimeStamp(ServerTimer);
            videoMessage->SetMessageID( msgID++ );
            int encodeError = encoder->EncodeSingleFrameIntoVideoMSG( srcPic, videoMessage, false );
            if( encodeError == -1 ){
                qCritical() << "[SenderVideo] Frame encode error.";
                return false;
            }
            
            sendCurrentVideoMessage();
            
        /////////////////////////// VIDEO_MESSAGE, H264 ///////////////////////////
        }else if( VIDEO_MODE == VideoModes::Video_H264 ){
            videoMessage = igtl::VideoMessage:: New();
            videoMessage->SetDeviceName( THIS_DEVICE_NAME );
            videoMessage->SetCodecType( IGTL_VIDEO_CODEC_NAME_H264 );
            videoMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );

            if( !convertToI420() ) return false;

            srcPic->data[0] = converted;
            srcPic->data[1] = converted + width * height;
            srcPic->data[2] = converted + width * height * 5/4;
            srcPic->stride[0] = width;
            srcPic->stride[1] = width / 2;
            srcPic->stride[2] = width / 2;
            //set timestamp:
            ServerTimer->GetTime();
            videoMessage->SetTimeStamp( ServerTimer );
            videoMessage->SetMessageID( msgID++ );
            int encodeError = encoder->EncodeSingleFrameIntoVideoMSG( srcPic, videoMessage, false );
            if( encodeError == -1 ){
                qCritical() << "[SenderVideo] Frame encode error.";
                return false;
            }
            
            sendCurrentVideoMessage();

        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  SET_DIMENSIONS   ///////////////////////////////////////////
//this is called when the frame dimension change
bool SenderVideo::setEncoder( int w, int h ){
    qDebug() << "[SenderVideo] Dimensions of the H264 encoder set to: " << w << "x" << h;
    qDebug() << "[SenderVideo] RCMode set to " << IMAGE_SEND_MODE << ", with " << TARGET_BIT_RATE << " bitrate.";
    if( VIDEO_MODE == VideoModes::Video_H264 ){
        h264StreamEncoder->SetPicWidthAndHeight( w, h );
        h264StreamEncoder->SetRCMode( IMAGE_SEND_MODE );
        h264StreamEncoder->SetRCTargetBitRate( TARGET_BIT_RATE );
        h264StreamEncoder->SetLosslessLink( LOSSLESS_VIDEO_TRANSMISSION );
        h264StreamEncoder->SetQP( MAX_QP, MIN_QP );
        h264StreamEncoder->InitializeEncoder();
        encoder = h264StreamEncoder;
    }else if( VIDEO_MODE == VideoModes::Video_I420 ){
        I420StreamEncoder->SetPicWidthAndHeight( w, h );
        I420StreamEncoder->InitializeEncoder();
        encoder = I420StreamEncoder;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  Convert_camera_image_to_I420   /////////////////////////////////////
bool SenderVideo::convertToI420(){
    if( CAMERA_FORMAT == QVideoFrameFormat::Format_ARGB8888 ){
        int convertError = libyuv::ARGBToI420( send_buffer,                         width * 4,
                                              converted,                            width,
                                              converted + width * height,           width / 2,
                                              converted + width * height * 5/4  ,   width / 2,
                                              width, height);
        if( convertError == -1 ){
            qCritical() << "[SenderVideo] Frame convert error.";
            return false;
        }
    }else if( CAMERA_FORMAT == QVideoFrameFormat::Format_NV12 ){
        int convertError = libyuv::NV12ToI420( send_buffer,                         width,
                                              send_buffer + width * height,         width,
                                              converted,                            width,
                                              converted + width * height,           width / 2,
                                              converted + width * height * 5/4  ,   width / 2,
                                              width, height);
        if( convertError == -1 ){
            qCritical() << "[SenderVideo] Frame convert error.";
            return false;
        }
    }else{
        qCritical() << "[SenderVideo] Camera format invalid.";
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  Convert_camera_image_to_RGB   /////////////////////////////////////
bool SenderVideo::convertToRGB(){
    if( CAMERA_FORMAT == QVideoFrameFormat::Format_ARGB8888 ){
        //TODO
        qCritical() << "[SenderVideo] TODO THIS CONFIGURATION IS NOT IMPLEMENTED.";
        return false;
    }else if( CAMERA_FORMAT == QVideoFrameFormat::Format_NV12 ){
        int convertError = libyuv::NV12ToRGB24( send_buffer,                         width,
                                              send_buffer + width * height,         width,
                                              converted,                            width * 3,
                                              width, height);
        if( convertError == -1 ){
            qCritical() << "[SenderVideo] Frame convert error.";
            return false;
        }
    }else{
        qCritical() << "[SenderVideo] Camera format invalid.";
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  Convert_camera_image_to_ABGR   /////////////////////////////////////
bool SenderVideo::convertToABGR(){
    if( CAMERA_FORMAT == QVideoFrameFormat::Format_ARGB8888 ){
        //TODO
        qCritical() << "[SenderVideo] TODO THIS CONFIGURATION IS NOT IMPLEMENTED.";
        return false;    }else if( CAMERA_FORMAT == QVideoFrameFormat::Format_NV12 ){
        int convertError = libyuv::NV12ToABGR( send_buffer,                         width,
                                              send_buffer + width * height,         width,
                                              converted,                            width * 4,
                                              width, height);
        if( convertError == -1 ){
            qCritical() << "[SenderVideo] Frame convert error.";
            return false;
        }
    }else{
        qCritical() << "[SenderVideo] Camera format invalid.";
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  SEND_CURRENT_IMAGE_MESSAGE   /////////////////////////////////////
void SenderVideo::sendCurrentImageMessage(){
    imageMessage->Pack();
    if( connected ){
        if( IMAGE_TRANSMISSION_PROTOCOL == TransmissionProtocol::TCP ){
            std::cout << "Sending image (TCP) with ID: " << msgID << std::endl;
            socket->Send( imageMessage->GetPackPointer(), imageMessage->GetPackSize() );
        }else if( protocol == TransmissionProtocol::UDP ){
            std::cout << "Sending image (UDP) with ID: " << msgID << std::endl;
            rtpWrapper->WrapMessageAndSend( udpServerSocket, (uchar * )(imageMessage->GetPackPointer()), imageMessage->GetPackSize() );
        }else{
            std::cerr << "Application configuration incorrect. Image transmission protocol undefined." << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////  SEND_CURRENT_VIDEO_MESSAGE   /////////////////////////////////////
void SenderVideo::sendCurrentVideoMessage(){
    if( connected ){
        if( protocol == TransmissionProtocol::TCP ){
            socket->Send( videoMessage->GetPackPointer(), videoMessage->GetPackSize() );
            std::cout << "Sending video (TCP) with ID: " << msgID << std::endl;
        }else if( protocol == TransmissionProtocol::UDP ){
            std::cout << "Sending video (UDP) with ID: " << msgID << std::endl;
            rtpWrapper->WrapMessageAndSend( udpServerSocket, (uchar * )(videoMessage->GetPackPointer()), videoMessage->GetPackSize() );
        }else{
            std::cerr << "Application configuration incorrect. Image transmission protocol undefined." << std::endl;
        }
    }
}
