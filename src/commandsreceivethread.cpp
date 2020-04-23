#include <QDebug>
#include "commandsreceivethread.h"

CommandsReceiveThread::CommandsReceiveThread(QObject *parent) : QThread(parent){
    setTerminationEnabled();
    //TODO
}

void CommandsReceiveThread::setReceiver( Receiver * receiver ){
    this->receiver = receiver;
}

void CommandsReceiveThread::run(){
    bool received = false;
    while ( !this->isInterruptionRequested() ) {
        received = this->receiver->receiveCommand();
        //wait 33ms:
        usleep(33000);
        //received is false when the send function didn't succeed.
        //This usually means that the init hasn't been done or that
        //we were disconnected from the server for some reason
        if ( !received ){
            //if it didn't work, we wait 1 second and try again
            qInfo() << "[CommandsReceiveThread] Will wait a second and try receiving again.";
            usleep( 1000000 );
        }
    }
}

void CommandsReceiveThread::end(){
    qInfo() << "[CommandsReceiveThread] Stopping receive thread.";
    receiver->closeSocket();
    this->requestInterruption();
    //TODO: Does this exit cleanly?
}
