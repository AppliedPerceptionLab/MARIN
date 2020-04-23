#include "sender.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  CONSTRUCTOR  //////////////////////////////////////////////
Sender::Sender( QObject *parent, int w, int h, QString thisaddress, int frw, int frh ) : QObject( parent ) {
    this->parent = parent;
    setSendingVideo( INITIALLY_SENDING_VIDEO );
    setServerAddress( SERVER_ADDRESS );
    qInfo() << "[Sender] Will send video to " << hostname;
    width = w;
    height = h;
    full_res_width = frw;
    full_res_height = frh;
    full_res_frame = new uchar[ full_res_width * full_res_height * 3/2 ];
    converted = new uchar[ CAMERA_WIDTH * CAMERA_HEIGHT * 3/2 ];
    srcPic->colorFormat = FormatI420;
    srcPic->picWidth = width;
    srcPic->picHeight = height;
    this->thisaddress = thisaddress;
    udpVideoServerSocket->SetIPAddress( thisaddress.toStdString().c_str() );
    udpVideoServerSocket->SetPortNumber( port_video );
    udpCommandsServerSocket->SetIPAddress( thisaddress.toStdString().c_str() );
    udpCommandsServerSocket->SetPortNumber( port_commands );
//    h264StreamEncoder = new H264Encoder( OH264_CONFIG_FILE_PATH );
    h264StreamEncoder = new H264Encoder( "" );
    
    h264StreamEncoder->SetSpeed( HIGH_COMPLEXITY );
    
    encoder = h264StreamEncoder;

    ServerTimer = igtl::TimeStamp::New();
    
    //TODO: Should eventually give user control over this value.
    int kbps = TARGET_BIT_RATE;
    int netWorkBandWidthInBPS = kbps * 1000; //networkBandwidth is in kbps
    int time = floor( 8 * RTP_PAYLOAD_LENGTH * 1e9 / netWorkBandWidthInBPS + 1.0 ); // the needed time in nanosecond to send a RTP payload.
    rtpWrapper->packetIntervalTime = time;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  DESTRUCTOR   ////////////////////////////////////////////
Sender::~Sender(){
    delete[] send_buffer;
    delete[] converted;
    delete[] full_res_frame;
    delete srcPic;
    delete camera;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////   COPY(ON_PREVIEW)_FRAME   //////////////////////////////////
void Sender::copy_frame(QVideoFrame frame){
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
            qCritical() << "[Sender] This format is not yet supported.";
            expected_size = curr_width*curr_height*3/2;
        }else if ( curr_format == QVideoFrame::Format_NV21 ){
            qCritical() << "[Sender] This format is not yet supported.";
            expected_size = curr_width*curr_height*3/2;
        }else if ( curr_format == QVideoFrame::Format_ARGB32 ){
            qCritical() << "[Sender] QVideoFrame input format set to QVideoFrame::Format_ARGB32.";
            expected_size = curr_width*curr_height*4;
        }else{
            qCritical() << "[Sender] The format " << curr_format << " is not yet supported.";
        }
        if( size != expected_size ){ qWarning() << "[Sender] The size of the buffer in memory is" << size << "but it should have been " << expected_size; }
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
bool Sender::send(){
    //try to connect if not already:
    if ( !connected_video ){
        connectVideo();
    }
    if ( !connected_commands ){
        connectCommands();
    }
    if( (!connected_commands && !connected_video) || !init_done ){
        return false;
    }
    //if there is a command to send we do this first:
    if( connected_commands && commands.size() > 0 ){
        command_number++;
        commandMsg = igtl::CommandMessage::New();
        commandMsg->SetDeviceName( THIS_DEVICE_NAME );
        commandMsg->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
        Command c = commands.front();
        commands.pop();
        CommandName actual_command = c.c;
        switch( actual_command ){
            case ToggleAnatomy:
                commandMsg->SetCommandName("ToggleAnatomy");
                break;
            case ToggleQuadView:
                commandMsg->SetCommandName("ToggleQuadView");
                break;
            case NavigateSlice:
                commandMsg->SetCommandName("NavigateSlice");
                break;
            case ReregisterAR:
                commandMsg->SetCommandName("ReregisterAR");
                break;
            case RotateView:
                commandMsg->SetCommandName("RotateView");
                break;
            case FreezeFrame:
                commandMsg->SetCommandName("FreezeFrame");
                break;
            case ResetReregistration:
                commandMsg->SetCommandName("ResetReregistration");
                break;
            case ArbitraryCommand:
                commandMsg->SetCommandName("ArbitraryCommand");
                break;
            default:
                qInfo() << "[Sender[BUG?]] Well... it looks like there is nothing to do here. The command provided is either empty or UNDEFINED or not recognized.";
        }
        commandMsg->SetCommandId( command_number );
        //generate the string:
        std::string param_string =  "<Params><par1>" +
                                           std::to_string( c.param1 ) +
                                   "</par1><par2>" +
                                           std::to_string( c.param2 ) +
                                   "</par2><par3>" +
                                           std::to_string( c.param3 ) +
                                   "</par3><par4>" +
                                           std::to_string( c.param4 ) +
                                   "</par4><par5>" +
                                           std::to_string( c.param5 ) +
                                   "</par5><par6>" +
                                           c.param6 +
                                   "</par6></Params>";
        commandMsg->SetCommandContent( param_string );
        commandMsg->Pack();
        
        uchar * frame = new uchar[ commandMsg->GetPackSize() ];
        memcpy( frame, commandMsg->GetPackPointer(), commandMsg->GetPackSize() );
        rtpWrapper->WrapMessageAndSend( udpCommandsServerSocket, frame, commandMsg->GetPackSize() );
    }
    //then we send the video frame:
    if( m_sendingVideo ){
        if ( newest_is_1 ){
            if ( !slot1_being_written ){
                slot1_being_read = true;
                memcpy( send_buffer, slot1.data, (uint)(argb_buffer_size) );
                slot1_being_read = false;
            }else{
                qDebug() << "[Sender] Buffer.slot1 is in use.";
            }
        }else if ( !newest_is_1 ){
            if ( !slot2_being_written ){
                slot2_being_read = true;
                memcpy( send_buffer, slot2.data, (uint)(argb_buffer_size) );
                slot2_being_read = false;
            }else{
                qDebug() << "[Sender] Buffer.slot2 is in use.";
            }
        }
        videoMessage = igtl::VideoMessage::New();
        videoMessage->SetDeviceName( THIS_DEVICE_NAME );
        videoMessage->SetCodecType( IGTL_VIDEO_CODEC_NAME_H264 );
        videoMessage->SetHeaderVersion( IGTL_HEADER_VERSION_2 );
        int convertError = libyuv::ARGBToI420( send_buffer, width*4, converted, width, converted + width * height, width / 2, converted + width * height * 5/4  , width / 2, width, height);
        if( convertError == -1 ){
            qCritical() << "[Sender] Frame convert error.";
            return false;
        }
        srcPic->data[0] = converted;
        srcPic->data[1] = converted + width * height;
        srcPic->data[2] = converted + width * height * 5/4;

        //set timestamp:
        ServerTimer->GetTime();
        videoMessage->SetTimeStamp(ServerTimer);
        int encodeError = encoder->EncodeSingleFrameIntoVideoMSG( srcPic, videoMessage, false );
        if( encodeError == -1 ){
            qCritical() << "[Sender] Frame encode error.";
            return false;
        }

        uchar * frame = new uchar[ videoMessage->GetPackSize() ];
        memcpy( frame, videoMessage->GetPackPointer(), videoMessage->GetPackSize() );
        rtpWrapper->WrapMessageAndSend( udpVideoServerSocket, frame, videoMessage->GetPackSize() );
        std::cout << "SEND!" << std::endl;
        delete [] frame;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////   CONNECT   ///////////////////////////////////////////
bool Sender::connectVideo(){
    if( !connected_video ){
        //create UDP socket for video:
        int s = udpVideoServerSocket->CreateUDPServer();
        if ( s < 0 ){
            std::cerr << "[Sender] Could not create a server socket for video (UDP)." << std::endl;
            connected_video = false;
        }else{
            int clientID = udpVideoServerSocket->AddClient( hostname.toStdString().c_str(), port_video, 0 );
            std::cout << "[Sender] added client: " << clientID << std::endl;
            connected_video = clientID >= 0;
            std::cout << "[Sender] Created a server socket. (UDP for video)" << std::endl;
    //        socket->SetTimeout(3000);
        }
    }else{
        qWarning() << "[Sender] Already connected to send video. Nothing to do.";
    }
    return connected_video;
}
bool Sender::connectCommands(){
    if( !connected_commands ){
        //create UDP socket for commands:
        int s = udpCommandsServerSocket->CreateUDPServer();
        if ( s < 0 ){
            std::cerr << "[Sender] Could not create a server socket for video (UDP)." << std::endl;
            connected_commands = false;
        }else{
            int clientID = udpCommandsServerSocket->AddClient( hostname.toStdString().c_str(), port_commands, 0 );
            std::cout << "[Sender] added client: " << clientID << std::endl;
            connected_commands = clientID >= 0;
            std::cout << "[Sender] Created a server socket.  (UDP for commands)" << std::endl;
            //        socket->SetTimeout(3000);
        }
    }else{
        qWarning() << "[Sender] Already connected to send commands. Nothing to do.";
    }
    return connected_commands;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////   CLOSE_SOCKET   /////////////////////////////////////////
void Sender::closeSocket(){
    //TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  SET_DIMENSIONS   ///////////////////////////////////////////
//this is called when the frame dimension change
bool Sender::setEncoder( int w, int h ){
    qDebug() << "[Sender] Dimensions of the H264 encoder set to: " << w << "x" << h;
    qDebug() << "[Sender] RCMode set to " << IMAGE_SEND_MODE << ", with " << TARGET_BIT_RATE << " bitrate.";
    h264StreamEncoder->SetPicWidthAndHeight( w, h );
    h264StreamEncoder->SetRCMode( IMAGE_SEND_MODE );
    h264StreamEncoder->SetRCTaregetBitRate( TARGET_BIT_RATE );
    h264StreamEncoder->InitializeEncoder();
    encoder = h264StreamEncoder;
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////   CHANGE_HOST   ///////////////////////////////////////////
void Sender::change_host( std::string str ){
    //TODO
    qWarning() << "This function is not yet defined.";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////   CHANGE_PORT   ///////////////////////////////////////////
void Sender::change_port( int p ){
    qWarning() << "[Sender] New port: " << p;
    //TODO
    //This shouldn't be used
    port_video = p;
    closeSocket();
    connected_video = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  ANATOMY_TOGGLED   ///////////////////////////////////////////
void Sender::toggleAnatomy( int i, bool b ){
    std::cout << "anatomy toggled: " << i << ", " << b << std::endl;
    Command c = { ToggleAnatomy, -99., -99., -99., i, b, "" };
    commands.push( c );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////  QUADVIEW_TOGGLED   ///////////////////////////////////////////
void Sender::toggleQuadView( bool b ){
    std::cout << "quadview toggled: " << b << std::endl;
    Command c = { ToggleQuadView, -99., -99., -99., b, -99, "" };
    commands.push( c );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////  NAVIGATE_SLICES   ////////////////////////////////////////////
void Sender::navigateSlice( SliceView slice, double x, double y ){
    std::cout << "NavigateSlice " << slice << " : (" << x << ", " << y << ")" << std::endl;
    Command c = { NavigateSlice, x, y, -99., slice, -99, "" };
    commands.push( c );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////  ROTATE_VIEW   ////////////////////////////////////////////
void Sender::rotateView( double dx, double dy, double z ){
    std::cout << "RotateView: ( " << dx << ", " << dy << " ), Zoom: " << z << std::endl;
    Command c = { RotateView, dx, dy, z, -99, -99, "" };
    commands.push( c );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  REREGISTER_AR   ////////////////////////////////////////////
void Sender::reregisterAR( double dx, double dy, double a ){
    std::cout << "ReregisterAR: T->( " << dx << ", " << dy << " ) ; R->" << a << std::endl;
    Command c = { ReregisterAR, dx, dy, a, -99, -99, "" };
    commands.push( c );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  FREEZE_FRAME   /////////////////////////////////////////////
void Sender::freezeFrame( bool value ){
    std::cout << "FreezeFrame: " << value << std::endl;
    Command c = { FreezeFrame, -99., -99., -99., value, -99, "" };
    commands.push( c );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  RESET_REREGISTRATION   //////////////////////////////////////////
void Sender::resetReregistration(){
    std::cout << "Reset reregistration!" << std::endl;
    Command c = { ResetReregistration, -99., -99., -99., -99, -99, "" };
    commands.push(c);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  SEND_POINT_SET   ////////////////////////////////////////////
void Sender::sendPointSet( std::vector<std::pair<int,int>> points ){
    std::string str = "";
    for(std::pair<int,int> pair : points){
        str += std::string( std::to_string(pair.first) + "," + std::to_string(pair.second) + ";" );
    }
    std::cout << "Send point set:" << str << std::endl;
    Command c = { ArbitraryCommand, -99., -99., -99., -99, -99, str };
    commands.push(c);
}
