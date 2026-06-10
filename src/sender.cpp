#include "sender.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  CONSTRUCTOR  //////////////////////////////////////////////
Sender::Sender( QObject *parent, QString thisaddress, int port ) : QObject( parent ) {
    
    this->parent = parent;
    setServerAddress( SERVER_ADDRESS );
    this->thisaddress = thisaddress;
    this->port = port;
    
    udpServerSocket->SetIPAddress( thisaddress.toStdString().c_str() );
    udpServerSocket->SetPortNumber( port );
    
    ServerTimer = igtl::TimeStamp::New();
    
    int netWorkBandWidthInBPS = TARGET_BIT_RATE; //networkBandwidth is in kbps
    int time = floor( 8 * RTP_PAYLOAD_LENGTH * 1e9 / netWorkBandWidthInBPS + 1.0 ); // the needed time in nanosecond to send a RTP payload.
    rtpWrapper->packetIntervalTime = time / 10; //Use a factor 10 for now, but could tweak this eventually, or add it to config for user to choose
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  DESTRUCTOR   ////////////////////////////////////////////
Sender::~Sender(){
    //TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////   CLOSE_SOCKET   /////////////////////////////////////////
void Sender::closeSocket(){
    //TODO
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
    port = p;
    closeSocket();
    connected = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////   CONNECT   ///////////////////////////////////////////
bool Sender::connect( std::string connection_description ){
    if( !connected ){
        if( protocol == TransmissionProtocol::UDP ){
            int s = udpServerSocket->CreateUDPServer();
            if ( s < 0 ){
                std::cerr << "[Sender] Could not create a server socket (UDP) for " << connection_description << std::endl;
                connected = false;
            }else{
                int clientID = udpServerSocket->AddClient( getServerAddress().toStdString().c_str(), getPort(), 0 );
                std::cout << "[Sender] Added client: " << clientID << std::endl;
                connected = clientID >= 0;
                std::cout << "[Sender] Created a server socket (UDP) for " << connection_description << std::endl;
            }
        }else if( protocol == TransmissionProtocol::TCP ){
            // Create TCP socket if not already:
            if( !tcpServerSocket->GetConnected() ){
                int st = tcpServerSocket->CreateServer( getPort() );
                if( st < 0 ){
                    std::cerr << "[Sender] Could not create a server socket (TCP) for " << connection_description << std::endl;
                    connected = false;
                    return false;
                }else{
                    std::cout << "[Sender] Created a server socket (TCP) for " << connection_description << std::endl;
                }
            }
            // Connect to the server:
            socket = tcpServerSocket->WaitForConnection( 3000 );
            if( socket == nullptr ){
                connected = false;
                std::cout << "[Sender] Timed out trying to connect to TCP for " << connection_description << std::endl;
            }else{
                std::cout << "[Sender] TCP connection established for " << connection_description << std::endl;
                connected = true;
            }
        }else{
            std::cerr << "[Sender] INVALID PROTOCOL" << std::endl;
        }
    }else{
        qWarning() << "[Sender] Already connected for " << connection_description << ". Nothing to do.";
    }
    return connected;
}
