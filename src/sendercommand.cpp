#include "sendercommand.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  CONSTRUCTOR  //////////////////////////////////////////////
SenderCommand::SenderCommand( QObject *parent, QString thisaddress, int port ) : Sender( parent, thisaddress, port ) {

    setSending( true );

    switch( COMMANDS_TRANSMISSION_PROTOCOL ){
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
    init_done = true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  DESTRUCTOR   ////////////////////////////////////////////
SenderCommand::~SenderCommand(){
    // TODO
}

#include <convert.h>
#include <algorithm>
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////   SEND   ////////////////////////////////////////////////

bool SenderCommand::send(){
    
    //try to connect if not already:
    if ( !connected ){
        return connect();
    }
    if( !connected || !init_done ){
        return false;
    }
    
    if( commands.size() > 0 ){
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
                qInfo() << "[SenderCommand[BUG?]] Well... it looks like there is nothing to do here. The command provided is either empty or UNDEFINED or not recognized.";
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
        
        if( COMMANDS_TRANSMISSION_PROTOCOL == TransmissionProtocol::TCP ){
            int success = socket->Send( commandMsg->GetPackPointer(), commandMsg->GetPackSize() );
            if( !success ){
                std::cout << "[SenderCommand] Couldn't send command (TCP)." << std::endl;
                connected = false;
            }else{
                std::cout << "[SenderCommand] Sent command (TCP)" << std::endl;
            }
        }else if( COMMANDS_TRANSMISSION_PROTOCOL == TransmissionProtocol::UDP ){
            rtpWrapper->WrapMessageAndSend( udpServerSocket, frame, commandMsg->GetPackSize() );
        }else{
            std::cerr << "[SenderCommand] Configuration not supported." << std::endl;
        }

    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////   CONNECT   ///////////////////////////////////////////
bool SenderCommand::connect(){
    std::cout << "[SenderCommand] Will try to connect to send commands." << std::endl;
    if( !connected ){
        if( COMMANDS_TRANSMISSION_PROTOCOL == TransmissionProtocol::UDP ){
            int s = udpServerSocket->CreateUDPServer();
            if ( s < 0 ){
                std::cerr << "[SenderCommand] Could not create a server socket for video (UDP)." << std::endl;
                connected = false;
            }else{
                int clientID = udpServerSocket->AddClient( getServerAddress().toStdString().c_str(), getPort(), 0 );
                std::cout << "[SenderCommand] Added client: " << clientID << std::endl;
                connected = clientID >= 0;
                std::cout << "[SenderCommand] Created a server socket. (UDP for commands)" << std::endl;
            }
        }else if( COMMANDS_TRANSMISSION_PROTOCOL == TransmissionProtocol::TCP ){
            // Create TCP socket if not already:
            if( !tcpServerSocket->GetConnected() ){
                int st = tcpServerSocket->CreateServer( getPort() );
                if( st < 0 ){
                    std::cerr << "[SenderCommand] Could not create a server socket for commands (TCP)." << std::endl;
                    connected = false;
                    return false;
                }else{
                    std::cout << "[SenderCommand] Created a server socket. (TCP for commands)" << std::endl;
                }
            }
            // Connect to the server:
            socket = tcpServerSocket->WaitForConnection( 3000 );
            if( socket == nullptr ){
                connected = false;
                std::cout << "[SenderCommand] Timed out trying to connect to TCP for commands." << std::endl;
            }else{
                connected = true;
            }
        }else{
            std::cerr << "[SenderCommand] INVALID PROTOCOL FOR SENDING COMMANDS" << std::endl;
        }
    }else{
        qWarning() << "[SenderCommand] Already connected to send commands. Nothing to do.";
    }
    return connected;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  COMMANDS  //////////////////////////////////////
void SenderCommand::toggleAnatomy( int i, bool b ){
    std::cout << "Anatomy toggled: " << i << ", " << b << std::endl;
    Command c = { ToggleAnatomy, -99., -99., -99., i, b, "" };
    commands.push( c );
}
void SenderCommand::toggleQuadView( bool b ){
    std::cout << "Quadview toggled: " << b << std::endl;
    Command c = { ToggleQuadView, -99., -99., -99., b, -99, "" };
    commands.push( c );
}
void SenderCommand::navigateSlice( SliceView slice, double x, double y ){
    std::cout << "NavigateSlice " << slice << " : (" << x << ", " << y << ")" << std::endl;
    Command c = { NavigateSlice, x, y, -99., slice, -99, "" };
    commands.push( c );
}
void SenderCommand::rotateView( double dx, double dy, double z ){
    std::cout << "RotateView: ( " << dx << ", " << dy << " ), Zoom: " << z << std::endl;
    Command c = { RotateView, dx, dy, z, -99, -99, "" };
    commands.push( c );
}
void SenderCommand::reregisterAR( double dx, double dy, double a ){
    std::cout << "ReregisterAR: T->( " << dx << ", " << dy << " ) ; R->" << a << std::endl;
    Command c = { ReregisterAR, dx, dy, a, -99, -99, "" };
    commands.push( c );
}
void SenderCommand::freezeFrame( bool value ){
    std::cout << "FreezeFrame: " << value << std::endl;
    Command c = { FreezeFrame, -99., -99., -99., value, -99, "" };
    commands.push( c );
}
void SenderCommand::resetReregistration(){
    std::cout << "Reset reregistration!" << std::endl;
    Command c = { ResetReregistration, -99., -99., -99., -99, -99, "" };
    commands.push(c);
}
void SenderCommand::sendPointSet( std::vector<std::pair<int,int>> points ){
    std::string str = "";
    for(std::pair<int,int> pair : points){
        str += std::string( std::to_string(pair.first) + "," + std::to_string(pair.second) + ";" );
    }
    std::cout << "Send point set:" << str << std::endl;
    Command c = { ArbitraryCommand, -99., -99., -99., -99, -99, str };
    commands.push(c);
}
void SenderCommand::arbitraryCommand( std::string str ){
    std::cout << "Send arbitrary command:" << str << std::endl;
    Command c = { ArbitraryCommand, -99., -99., -99., -99, -99, str };
    commands.push(c);
}
