#include "threadshandler.h"
#include <QtNetwork>
#include <QHostAddress>

ThreadsHandler::ThreadsHandler( QObject *parent ) : QObject( parent ){
    //create send thread:
    videoSendThread = new OpenIGTLsendThread();
    commandsSendThread = new OpenIGTLsendThread();
    vrt = new VideoReceiveThread;
    crt = new CommandsReceiveThread;

    udpSocket = new QUdpSocket(this);
    udpSocket->bind( QHostAddress::Any, 45454 );
    QObject::connect( udpSocket, SIGNAL(readyRead()), this, SLOT(receivedBroadcast()) );
}

ThreadsHandler::~ThreadsHandler(){
    delete vrt;
    delete crt;
    delete videoSendThread;
    delete commandsSendThread;
    delete vs;
    delete cs;
    delete r;
    delete udpSocket;
}

void ThreadsHandler::setReceiver( Receiver * receiver ){
    vrt->setReceiver( receiver );
    crt->setReceiver( receiver );
    r = receiver;
}

void ThreadsHandler::setSenderVideo( Sender * ss ){
    videoSendThread->setSender( ss );
    vs = ss;
}

void ThreadsHandler::setSenderCommand( Sender * ss ){
    commandsSendThread->setSender( ss );
    cs = ss;
}


void ThreadsHandler::startSenders(){
    videoSendThread->start();
    commandsSendThread->start();
}

void ThreadsHandler::startReceiver(){
    vrt->start();
    crt->start();
}

//TODO: should have some sort of pause variable instead. This should all be reworked.
void ThreadsHandler::toggleSender( bool b ){
    vs->setSending( b );
}

void ThreadsHandler::toggleReceiver( bool b ){
    if( b ){
        vrt = new VideoReceiveThread();
        vrt->setReceiver( r );
        vrt->start();
    }else{
        vrt->end();
    }
}

void ThreadsHandler::quit(){
    qInfo() << "[ThreadsHandler] Requesting both threads to stop.";
    videoSendThread->end();
    commandsSendThread->end();
    vrt->end();
    crt->end();
    //wait for threads to finish:
    qInfo() << "[ThreadsHandler] Waiting for all threads to stop.";
    bool gracefulSVideo = videoSendThread->wait( 5000 );
    bool gracefulSCommands = commandsSendThread->wait( 5000 );
    bool gracefulRVideo = vrt->wait( 5000 );
    bool gracefulRCommands = crt->wait( 5000 );
    //if the threads aren't done after 5 secs, we just kill them:
    if( !gracefulSVideo ){
        qInfo() << "[ThreadsHandler] Video Send thread wouldn't finish. Killing it.";
        videoSendThread->terminate();
    }
    if( !gracefulSCommands ){
        qInfo() << "[ThreadsHandler] Video Send thread wouldn't finish. Killing it.";
        commandsSendThread->terminate();
    }
    if( !gracefulRVideo ){
        qInfo() << "[ThreadsHandler] Receive video thread wouldn't finish. Killing it.";
        vrt->terminate();
    }
    if( !gracefulRCommands ){
        qInfo() << "[ThreadsHandler] Receive commands thread wouldn't finish. Killing it.";
        crt->terminate();
    }
    //giving the exit operation a go:
    emit readyToExit();
}

//TODO: Should be improved.
////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////   RECEIVED_BROADCAST   /////////////////////////////////////////
void ThreadsHandler::receivedBroadcast(){
    QByteArray datagram;
    QHostAddress * address = nullptr;
    quint16 * port = nullptr;
    while( udpSocket->hasPendingDatagrams() ) {
        datagram.resize( int( udpSocket->pendingDatagramSize() ) );
        //TODO: looks like address and port are both 0, so the address needs to be included in the data
        udpSocket->readDatagram( datagram.data(), datagram.size(), address, port );
        QString str = QString( datagram.data() );
        if( str == lastestBroadcast ){
            continue;
        }
        QStringList toks = str.split( QString( ":" ) );
        QRegularExpression qreAddress = QRegularExpression( "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}" );
        QRegularExpressionMatch matchAddress = qreAddress.match( toks.at(1) );
        QRegularExpression qrePort = QRegularExpression( "[0-9]{5}" );
        QRegularExpressionMatch matchPort = qrePort.match( toks.at(2) );
        if ( matchAddress.hasMatch() ){
            qInfo() << "[Threadhandler] Server address is : " << toks.at(1);
            vs->change_host( toks.at(1).toStdString() );
            r->change_host( toks.at(1).toStdString() );
            lastestBroadcast = str;
            emit serverFound();
        }else{
            std::cerr << "[Threadhandler] Server address received from broadcast wasn't formated properly." << std::endl;
        }
        if ( matchPort.hasMatch() ){
            qInfo() << "[Threadhandler] Server port is : " << toks.at(2);
            r->change_port_video( toks.at(2).toInt() );
            lastestBroadcast = str;
        }else{
            std::cerr << "[Threadhandler] Server address received from broadcast wasn't formated properly." << std::endl;
        }
    }
    //TODO: Eventually change ports as well
}
