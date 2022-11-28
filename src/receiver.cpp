#include "receiver.h"
#include "igtlMultiThreader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// UNWRAP_THREAD ////////////////////////////////////////////////
static void * ThreadFunctionUnWrap( void * ptr ){
    // Get thread information
    igtl::MultiThreader::ThreadInfo * info = static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
    const char *deviceType = "VIDEO";
    Receiver * parentObj = static_cast<Receiver*>(info->UserData);
    //TODO: Eeventually the device name should be fetched from the server. Ideally MARIN should broadcast its address, then the server would respond with its name or something similar.
    const char * videoDeviceName = SERVER_NAME;
    while( parentObj->keepReceiving() ){
        parentObj->getWrapper()->UnWrapPacketWithTypeAndName(deviceType, videoDeviceName);
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// READ_SOCKET_THREAD /////////////////////////////////////////////
static void * ThreadFunctionReadSocket( void * ptr ){
    // Get thread information
    igtl::MultiThreader::ThreadInfo * info = static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
    Receiver * parentObj = static_cast<Receiver*>(info->UserData);
    unsigned char UDPPacket[RTP_PAYLOAD_LENGTH+RTP_HEADER_LENGTH];
    while( parentObj->keepReceiving() ){
        int totMsgLen = parentObj->getUDPSocket()->ReadSocket( UDPPacket, RTP_PAYLOAD_LENGTH+RTP_HEADER_LENGTH );
        parentObj->WriteTimeInfo( UDPPacket );
        if ( totMsgLen > 0 ){
            parentObj->getWrapper()->PushDataIntoPacketBuffer( UDPPacket, totMsgLen );
            //TODO: This next line is very suboptimal and should be fixed. For now I send server found every time I have a frame.
            emit parentObj->serverFound();
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// WRITE_TIME_INFO /////////////////////////////////////////////
void Receiver::WriteTimeInfo( unsigned char * UDPPacket ){
    igtl_uint16 fragmentField;
    igtl_uint32 messageID;
    int extendedHeaderLength = IGTL_EXTENDED_HEADER_SIZE;
    memcpy(&fragmentField, (void*)(UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+extendedHeaderLength-2),2);
    memcpy(&messageID, (void*)(UDPPacket + RTP_HEADER_LENGTH+IGTL_HEADER_SIZE+extendedHeaderLength-6),4);
    if(igtl_is_little_endian()){
        fragmentField = BYTE_SWAP_INT16(fragmentField);
        messageID = BYTE_SWAP_INT32(messageID);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// CONSTRUCTOR ////////////////////////////////////////////////
Receiver::Receiver(QObject *parent, QString ta ) : QObject(parent){
    this->parent = parent;

    //init:
    glock = new QMutex();
    rtpWrapper = igtl::MessageRTPWrapper::New();
    UDPSocket = igtl::UDPClientSocket::New();
    
    decoder = h264StreamDecoder;
    decodedPic->data[0] = new igtl_uint8[ AUGMENTATION_WIDTH * AUGMENTATION_HEIGHT * 3/2 ];
    decodedPic->colorFormat = FormatI420;
    decodedPic->picWidth = AUGMENTATION_WIDTH;
    decodedPic->picHeight = AUGMENTATION_HEIGHT;
    thisaddress = ta;
    act_img = new QImage(AUGMENTATION_WIDTH, AUGMENTATION_HEIGHT, QImage::Format_ARGB32);

    connectVideo();

    Receiver * receiver = this;
    readThreadID = threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionReadSocket, receiver);
    unwrapThreadID = threader->SpawnThread((igtl::ThreadFunctionType)&ThreadFunctionUnWrap, receiver);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// RECEIVE ////////////////////////////////////////////////
bool Receiver::receiveCommand(){
    if( !connected_commands || !(commandsSocket->GetConnected()) ){
        connectCommands();
        return false;
    }
    
    //init the header message:
    igtl::MessageHeader::Pointer headerMsg;
    headerMsg = igtl::MessageHeader::New();
    headerMsg->InitPack();
    //TODO: this should all be fixed
    bool timeout(false);
    int r = commandsSocket->Receive( headerMsg->GetPackPointer(), headerMsg->GetPackSize(), timeout);
    //this whould happen when the timeout was reached (there were no messages to receive.)
    if( r == -1 ){
        return true;
    }
    
    // Deserialize the header:
    headerMsg->Unpack();
    if( headerMsg->GetHeaderVersion() != version ){
        connected_commands = false;
        qWarning() << "[Receiver] Version of the client and server don't match. ( I received" << headerMsg->GetHeaderVersion() << "but expected" << version << ")";
        return false;
    }
    if( strcmp(headerMsg->GetDeviceType(), "STATUS") == 0 ){
        statusMsg->SetMessageHeader(headerMsg);
        statusMsg->AllocatePack();
        bool timeout(false);
        int sizei = commandsSocket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize(), timeout);
        statusMsg->Unpack();
        if( statusMsg->GetCode() == 1 ){            //STATUS_OK
            //everything is back to normal!
            qInfo() << "[Receiver] DEBUG: Tracking is back.";
            emit TrackingStatusChanged(true);
        }else if( statusMsg->GetCode() == 15 ){     //STATUS_DISABLED
            qInfo() << "[Receiver] DEBUG: Tracking is gone.";
            emit TrackingStatusChanged(false);
        }else{
            qWarning() << "[Receiver] Status code not recognized.";
        }
    }
    return true;
}

#include <libyuv.h>
#include <qdatetime.h>
#include <qstring.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// RECEIVE ////////////////////////////////////////////////
bool Receiver::receive(){
    //if not connected, connect:
    if ( !connected_video ){ connectVideo(); return false; }
    //check if any new video messages:
    getRTPMutex()->lock();
        unsigned long nbMsgs = rtpWrapper->unWrappedMessages.size();
    getRTPMutex()->unlock();
    if( nbMsgs ){
        videoMsg = igtl::VideoMessage::New();
        getRTPMutex()->lock();
            std::map<igtl_uint32, igtl::UnWrappedMessage*>::iterator it = rtpWrapper->unWrappedMessages.begin();
            igtlUint8 * message = new igtlUint8[it->second->messageDataLength];
            int MSGLength = it->second->messageDataLength;
            memcpy( message, it->second->messagePackPointer, it->second->messageDataLength );
            delete it->second;
            it->second = nullptr;
            rtpWrapper->unWrappedMessages.erase(it);
        getRTPMutex()->unlock();
        headerMsg = igtl::MessageHeader::New();
        headerMsg->InitPack();
        memcpy( headerMsg->GetPackPointer(), message, IGTL_HEADER_SIZE );
        headerMsg->Unpack();
        if( headerMsg->GetHeaderVersion() != version ){
            connected_video = false;
            qCritical() << "[Receiver] Version of the client and server don't match. ( I received" << headerMsg->GetHeaderVersion() << "but I expected" << version << ")";
            return false;
        }
        
        if( strcmp(headerMsg->GetDeviceType(), "VIDEO") == 0 ){
            videoMsg->SetMessageHeader( headerMsg );
            videoMsg->AllocateBuffer();
            //Not necessary
            if( MSGLength == videoMsg->GetPackSize() ){
                memcpy(videoMsg->GetPackPointer(), message, MSGLength);
                videoMsg->Unpack(0);
                decoder->DecodeVideoMSGIntoSingleFrame( videoMsg, decodedPic );
                int conversion_success = libyuv::I420ToARGB( decodedPic->data[0], AUGMENTATION_WIDTH, decodedPic->data[0] + AUGMENTATION_WIDTH * AUGMENTATION_HEIGHT, AUGMENTATION_WIDTH / 2, decodedPic->data[0] + AUGMENTATION_WIDTH * AUGMENTATION_HEIGHT * 5/4, AUGMENTATION_WIDTH / 2, act_img->bits(), AUGMENTATION_WIDTH * 4, AUGMENTATION_WIDTH, AUGMENTATION_HEIGHT );
                emit new_image_received();
            }else{
                qCritical() << "[Receiver] The message length is " << MSGLength << " but it should be " << videoMsg->GetPackSize();
                delete [] message;
                return false;
            }
        }else{
            qCritical() << "[Receiver] Received a " << headerMsg->GetDeviceType() << " message. Don't know what to do with it.";
        }
        delete [] message;
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  CLEANUP   //////////////////////////////////////////////
Receiver::~Receiver(){
    stillReceiving = false;
    usleep( 50000 );
    threader->TerminateThread( readThreadID );
    threader->TerminateThread( unwrapThreadID );
    delete act_img;
    delete h264StreamDecoder;
    delete decodedPic;
    delete glock;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////   CONNECT   ///////////////////////////////////////////
bool Receiver::connectVideo(){
    int joined = UDPSocket->JoinNetwork( thisaddress.toLatin1(), port_video );
    if ( joined <= 0 ){
        qInfo() << "[Receiver] Cannot connect to the server for video.";
        connected_video = false;
    }else{
        qInfo() << "[Receiver] Connected for video.";
        connected_video = true;
        UDPSocket->SetTimeout(3000);
    }
    return connected_video;
}
bool Receiver::connectCommands(){
    if( !connected_commands ){
        //create TCP socket for commands:
        int r = commandsSocket->ConnectToServer( hostname.c_str(), port_commands, false );
        if (r != 0){
            qInfo() << "[Receiver] Cannot connect to the server for status updates (TCP).";
            connected_commands = false;
        }else{
            qInfo() << "[Receiver] Connected for commands.";
            connected_commands = true;
            commandsSocket->SetReceiveBlocking( 0 );
        }
    }else{
        qWarning() << "[Receiver] Already connected to receive commands. Nothing to do.";
    }
    return connected_commands;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////   CLOSE_SOCKET   /////////////////////////////////////////
void Receiver::closeSocket(){
    UDPSocket->CloseSocket();
}

#include <QString>
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////   CHANGE_HOST   ///////////////////////////////////////////
void Receiver::change_host( std::string str ){
    qInfo() << "[Receiver] new server address: " << QString(str.c_str());
    closeSocket();
    hostname = str;
    connected_video = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////   CHANGE_PORT   ///////////////////////////////////////////
void Receiver::change_port_video( int p ){
    qInfo() << "[Receiver] new port :" << p;
    port_video = p;
    closeSocket();
    connected_video = false;
}
