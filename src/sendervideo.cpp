#include "sendervideo.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  CONSTRUCTOR  //////////////////////////////////////////////
SenderVideo::SenderVideo( QObject *parent, QString thisaddress, int port, int w, int h, int frw, int frh ) : Sender( parent, thisaddress, port ) {
    
    setSending( INITIALLY_SENDING_VIDEO );
    qInfo() << "[SenderVideo] Will send image/video to " << getServerAddress();
    width = w;
    height = h;
    full_res_width = frw;
    full_res_height = frh;
    full_res_frame = new uchar[ full_res_width * full_res_height * 3/2 ];
    converted = new uchar[ CAMERA_WIDTH * CAMERA_HEIGHT * 3/2 ];
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
        case VideoModes::I420:
            I420StreamEncoder = new I420Encoder( (char*)"" );
            encoder = I420StreamEncoder;
            // TODO?
            break;
        case VideoModes::H264:
            //    h264StreamEncoder = new H264Encoder( OH264_CONFIG_FILE_PATH );
            h264StreamEncoder = new H264Encoder( (char*)"" );
            //h264StreamEncoder->SetSpeed( HIGH_COMPLEXITY );
            h264StreamEncoder->SetSpeed( LOW_COMPLEXITY );
            encoder = h264StreamEncoder;
            break;
        case VideoModes::VIDEOMODE_UNDEFINED:
            std::cerr << "Video mode must be defined in configuration file." << std::endl;
            exit(1);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  DESTRUCTOR   ////////////////////////////////////////////
SenderVideo::~SenderVideo(){
    delete[] send_buffer;
    delete[] converted;
    delete[] full_res_frame;
    delete srcPic;
    delete camera;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////   COPY(ON_PREVIEW)_FRAME   //////////////////////////////////
void SenderVideo::copy_frame(QVideoFrame frame){
    copy_number++;
    //map and init:
    frame.map(QAbstractVideoBuffer::ReadOnly);
    int curr_height = frame.height();
    int curr_width = frame.width();
    QVideoFrame::PixelFormat curr_format = frame.pixelFormat();
    int size = frame.mappedBytes();
    //Flip it vertically here for vtk conpatibility.
    QImage::Format imageFormat = QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat());
    QImage img( frame.bits(),
                frame.width(),
                frame.height(),
                frame.bytesPerLine(),
               imageFormat);
    //to flip it:
//    QImage img_mirrored = img.mirrored();
    //or not:
    QImage img_mirrored = img;
    //TODO: not sure why this is necessary
    long real_count_bytes = img_mirrored.sizeInBytes();
    
    //on first call, init buffers:
    if ( slot1.data == nullptr
            || curr_height != height
            || curr_width != width
            || curr_format != format
            || input_image_size != size ){
        input_image_size = size;
        int expected_size = -99;
        //switch case for different formats (eventually more should be supported...):
        if (curr_format == QVideoFrame::Format_NV12){
            qCritical() << "[SenderVideo] This format is not yet supported.";
            expected_size = curr_width*curr_height*3/2;
        }else if ( curr_format == QVideoFrame::Format_NV21 ){
            qCritical() << "[SenderVideo] This format is not yet supported.";
            expected_size = curr_width*curr_height*3/2;
        }else if ( curr_format == QVideoFrame::Format_ARGB32 ){
            qCritical() << "[SenderVideo] QVideoFrame input format set to QVideoFrame::Format_ARGB32.";
            expected_size = curr_width*curr_height*4;
        }else{
            qCritical() << "[SenderVideo] The format " << curr_format << " is not yet supported.";
        }
        if( size != expected_size ){ qWarning() << "[SenderVideo] The size of the buffer in memory is" << size << "but it should have been " << expected_size; }
        size = expected_size;
        argb_buffer_size = curr_width*curr_height*4;
        slot1.frame_number = copy_number;
        slot1.size = size;
        slot1.data = new uchar[size];
        slot2.frame_number = copy_number;
        slot2.size = size;
        slot2.data = new uchar[size];
        send_buffer = new uchar[size];
        width = curr_width;
        height = curr_height;
        bytes_per_line = frame.bytesPerLine();
        format = curr_format;
        init_done = true;
        //set encoder parameters:
        setEncoder( curr_width,curr_height );
    }

    //looks like a two frame buffer is enough.
    if ( !newest_is_1 ) {
        if ( !slot1_being_read ){
            slot1_being_written = true;
            memcpy(slot1.data, img_mirrored.bits(), (uint)real_count_bytes);
            slot1.frame_number = copy_number;
            slot1_being_written = false;
            newest_is_1 = true;
        }else{
            qInfo() << "Nothing to do.";
        }
    }else if ( newest_is_1 ){
        if ( !slot2_being_read ){
            slot2_being_written = true;
            memcpy(slot2.data, img_mirrored.bits(), (uint)(real_count_bytes));
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
    
    if ( !connected ){
        connect();
    }
    if( !connected || !init_done ){
        return false;
    }
    
    if( sending() ){
        // copy data from buffer:
        if ( newest_is_1 ){
            if ( !slot1_being_written ){
                slot1_being_read = true;
                memcpy( send_buffer, slot1.data, (uint)(argb_buffer_size) );
                slot1_being_read = false;
            }else{
                qDebug() << "[SenderVideo] Buffer.slot1 is in use.";
            }
        }else if ( !newest_is_1 ){
            if ( !slot2_being_written ){
                slot2_being_read = true;
                memcpy( send_buffer, slot2.data, (uint)(argb_buffer_size) );
                slot2_being_read = false;
            }else{
                qDebug() << "[SenderVideo] Buffer.slot2 is in use.";
            }
        }
        //send data:
        if( VIDEO_MODE == VideoModes::Image_Y ){
            if ( tcpServerSocket->GetConnected() ) {
                int   size[]     = { width, height, 1 };                   // image dimension
                float spacing[]  = { 1.0, 1.0, 1.0 };                 // spacing (mm/pixel)
                int   scalarType = igtl::ImageMessage::TYPE_UINT8;  // scalar type
                imageMessage = igtl::ImageMessage::New();
                imageMessage->SetDimensions( size );
                imageMessage->SetSpacing( spacing );
                imageMessage->SetScalarType( scalarType );
                imageMessage->SetDeviceName( THIS_DEVICE_NAME );
                imageMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
                imageMessage->SetMessageID( msgID++ );
                imageMessage->AllocateScalars();
                
                int convertError = libyuv::ARGBToI420( send_buffer, width*4, converted, width, converted + width * height, width / 2, converted + width * height * 5/4  , width / 2, width, height);
                if( convertError == -1 ){
                    qCritical() << "[SenderVideo] Frame convert error.";
                    return false;
                }
                srcPic->data[0] = converted;
                srcPic->data[1] = converted + width * height;
                srcPic->data[2] = converted + width * height * 5/4;
                
                //set timestamp:
                ServerTimer->GetTime();
                imageMessage->SetTimeStamp( ServerTimer );
                
                memcpy( imageMessage->GetScalarPointer(), converted, imageMessage->GetPackSize() );
                imageMessage->Pack();
                if( IMAGE_TRANSMISSION_PROTOCOL == TransmissionProtocol::TCP ){
                    socket->Send( imageMessage -> GetPackPointer(), imageMessage -> GetPackSize() );
                    std::cout << "Sending image (TCP) with ID: " << msgID << std::endl;
                }else{
                    //TODO, add support
                    std::cerr << "Configuration not supported." << std::endl;
                }

            }else{
                std::cout << "Not connected (TCP Image)" << std::endl;
            }
        }else if( VIDEO_MODE == VideoModes::Image_BGRA ){
            if ( tcpServerSocket->GetConnected() ) {

                int   size[]     = { width, height, 1 };                   // image dimension
                float spacing[]  = { 1., 1., 1. };                   // spacing (mm/pixel)
                int   scalarType = igtl::ImageMessage::TYPE_UINT8;      // scalar type
                
                imageMessage = igtl::ImageMessage::New();
                imageMessage->SetDimensions( size );
                imageMessage->SetSpacing( spacing );
                imageMessage->SetScalarType( scalarType );
                imageMessage->SetNumComponents( 4 );
                imageMessage->SetDeviceName( THIS_DEVICE_NAME );
                imageMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
                imageMessage->SetMessageID( msgID++ );
                imageMessage->AllocateScalars();
                
                //set timestamp:
                ServerTimer->GetTime();
                imageMessage->SetTimeStamp( ServerTimer );
                
                memcpy( imageMessage->GetScalarPointer(), send_buffer, imageMessage->GetPackSize() );
                imageMessage->Pack();
                if( IMAGE_TRANSMISSION_PROTOCOL == TransmissionProtocol::TCP ){
                    socket->Send( imageMessage->GetPackPointer(), imageMessage->GetPackSize() );
                    std::cout << "Sending image (TCP) with ID: " << msgID << std::endl;
                }else{
                    //TODO, add support
                    std::cerr << "Configuration not supported." << std::endl;
                }

            }else{
                std::cout << "Not connected (TCP Image)" << std::endl;
            }
        }else if( VIDEO_MODE == VideoModes::Image_RGB ){
            if ( tcpServerSocket->GetConnected() ) {

                int   size[]     = { width, height, 1 };                   // image dimension
                float spacing[]  = { 1., 1., 1. };                   // spacing (mm/pixel)
                int   scalarType = igtl::ImageMessage::TYPE_UINT8;      // scalar type
                
                imageMessage = igtl::ImageMessage::New();
                imageMessage->SetDimensions( size );
                imageMessage->SetSpacing( spacing );
                imageMessage->SetScalarType( scalarType );
                imageMessage->SetNumComponents( 3 );
                imageMessage->SetDeviceName( THIS_DEVICE_NAME );
                imageMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
                imageMessage->SetMessageID( msgID++ );
                imageMessage->AllocateScalars();
                
                //set timestamp:
                ServerTimer->GetTime();
                imageMessage->SetTimeStamp( ServerTimer );

                uchar * RGB_buffer = new uchar[ width*height*3 ];
                for( int i = 0; i < width; i++ ){
                    for( int j = 0; j < height; j++ ){
                        RGB_buffer[ i*3 + j*width*3 + 0] = send_buffer[ i*4 + j*width*4 + 2 ];
                        RGB_buffer[ i*3 + j*width*3 + 1] = send_buffer[ i*4 + j*width*4 + 1 ];
                        RGB_buffer[ i*3 + j*width*3 + 2] = send_buffer[ i*4 + j*width*4 + 0 ];
                    }
                }
                memcpy( imageMessage->GetScalarPointer(), RGB_buffer, imageMessage->GetPackSize() );
                imageMessage->Pack();
                if( IMAGE_TRANSMISSION_PROTOCOL == TransmissionProtocol::TCP ){
                    socket->Send( imageMessage->GetPackPointer(), imageMessage->GetPackSize() );
                    std::cout << "Sending image (TCP) with ID: " << msgID << std::endl;
                }else{
                    //TODO, add support
                    std::cerr << "Configuration not supported." << std::endl;
                }
            }else{
                std::cout << "Not connected (TCP Image)" << std::endl;
            }
        }else if( VIDEO_MODE == VideoModes::I420 ){
            videoMessage = igtl::VideoMessage:: New();
            videoMessage->SetDeviceName( THIS_DEVICE_NAME );
            videoMessage->SetCodecType( IGTL_VIDEO_CODEC_NAME_H264 );
            videoMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
            int convertError = libyuv::ARGBToI420( send_buffer, width*4, converted, width, converted + width * height, width / 2, converted + width * height * 5/4  , width / 2, width, height);
            if( convertError == -1 ){
                qCritical() << "[SenderVideo] Frame convert error.";
                return false;
            }
            srcPic->data[0] = converted;
            srcPic->data[1] = converted + width * height;
            srcPic->data[2] = converted + width * height * 5/4;
            
            //set timestamp:
            ServerTimer->GetTime();
            videoMessage->SetTimeStamp(ServerTimer);
            videoMessage->SetMessageID( msgID++ );
            int encodeError = encoder->EncodeSingleFrameIntoVideoMSG( srcPic, videoMessage, false );
            if( encodeError == -1 ){
                qCritical() << "[SenderVideo] Frame encode error.";
                return false;
            }
            
            uchar * frame = new uchar[ videoMessage->GetPackSize() ];
            memcpy( frame, videoMessage->GetPackPointer(), videoMessage->GetPackSize() );
            if( IMAGE_TRANSMISSION_PROTOCOL == TransmissionProtocol::TCP ){
                socket->Send( videoMessage->GetPackPointer(), videoMessage->GetPackSize() );
                std::cout << "Sending video (TCP) with ID: " << msgID << std::endl;
            }else{
                //TODO, add support
                std::cerr << "Configuration not supported." << std::endl;
            }
            delete [] frame;
        }else if( VIDEO_MODE == VideoModes::H264 ){
            videoMessage = igtl::VideoMessage:: New();
            videoMessage->SetDeviceName( THIS_DEVICE_NAME );
            videoMessage->SetCodecType( IGTL_VIDEO_CODEC_NAME_H264 );
            videoMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
            int convertError = libyuv::ARGBToI420( send_buffer, width*4, converted, width, converted + width * height, width / 2, converted + width * height * 5/4  , width / 2, width, height);
            if( convertError == -1 ){
                qCritical() << "[SenderVideo] Frame convert error.";
                return false;
            }
            srcPic->data[0] = converted;
            srcPic->data[1] = converted + width * height;
            srcPic->data[2] = converted + width * height * 5/4;
            
            //set timestamp:
            ServerTimer->GetTime();
            videoMessage->SetTimeStamp(ServerTimer);
            videoMessage->SetMessageID( msgID++ );
            videoMessage->SetMessageID( 0 );
            int encodeError = encoder->EncodeSingleFrameIntoVideoMSG( srcPic, videoMessage, false );
            if( encodeError == -1 ){
                qCritical() << "[SenderVideo] Frame encode error.";
                return false;
            }
            
            uchar * frame = new uchar[ videoMessage->GetPackSize() ];
            memcpy( frame, videoMessage->GetPackPointer(), videoMessage->GetPackSize() );
            if( IMAGE_TRANSMISSION_PROTOCOL == TransmissionProtocol::UDP ){
                rtpWrapper->WrapMessageAndSend( udpServerSocket, frame, videoMessage->GetPackSize() );
                std::cout << "Sending video (UDP) with ID: " << msgID << std::endl;
            }else{
                //TODO, add support
                std::cerr << "Configuration not supported." << std::endl;
            }
            delete [] frame;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////   CONNECT   ///////////////////////////////////////////
bool SenderVideo::connect(){
    if( !connected ){
        if( IMAGE_TRANSMISSION_PROTOCOL == TransmissionProtocol::TCP ){
            //create TCP socket:
            int st = tcpServerSocket->CreateServer( getPort() );
            if( st < 0 ){
                std::cerr << "[SenderVideo] Could not create a server socket for image (TCP)." << std::endl;
                connected = false;
            }else{
                std::cout << "[SenderVideo] Created a server socket. (TCP for image)" << std::endl;
                    socket = tcpServerSocket->WaitForConnection(3000);
                    connected = true;
            }
        }else if( IMAGE_TRANSMISSION_PROTOCOL == TransmissionProtocol::UDP ){
            //create UDP socket:
            int s = udpServerSocket->CreateUDPServer();
            if ( s < 0 ){
                std::cerr << "[SenderVideo] Could not create a server socket for video (UDP)." << std::endl;
                connected = false;
            }else{
                int clientID = udpServerSocket->AddClient( getServerAddress().toStdString().c_str(), getPort(), 0 );
                std::cout << "[SenderVideo] added client: " << clientID << std::endl;
                connected = clientID >= 0;
                std::cout << "[SenderVideo] Created a server socket. (UDP for video)" << std::endl;
            }
        }
    }else{
        qWarning() << "[SenderVideo] Already connected to send video. Nothing to do.";
    }
    return connected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  SET_DIMENSIONS   ///////////////////////////////////////////
//this is called when the frame dimension change
bool SenderVideo::setEncoder( int w, int h ){
    qDebug() << "[SenderVideo] Dimensions of the H264 encoder set to: " << w << "x" << h;
    qDebug() << "[SenderVideo] RCMode set to " << IMAGE_SEND_MODE << ", with " << TARGET_BIT_RATE << " bitrate.";
    if( VIDEO_MODE == VideoModes::H264 ){
        h264StreamEncoder->SetPicWidthAndHeight( w, h );
        h264StreamEncoder->SetRCMode( IMAGE_SEND_MODE );
        h264StreamEncoder->SetRCTaregetBitRate( TARGET_BIT_RATE );
        h264StreamEncoder->InitializeEncoder();
        encoder = h264StreamEncoder;
    }else if( VIDEO_MODE == VideoModes::I420 ){
        I420StreamEncoder->SetPicWidthAndHeight( w, h );
        I420StreamEncoder->InitializeEncoder();
        encoder = I420StreamEncoder;
    }
    return true;
}
