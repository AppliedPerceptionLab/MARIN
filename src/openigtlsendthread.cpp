#include <QDebug>
#include "openigtlsendthread.h"

OpenIGTLsendThread::OpenIGTLsendThread(QObject *parent) : QThread(parent){
    setTerminationEnabled();
    //TODO
}


void OpenIGTLsendThread::setSender(Sender * sender){
    s = sender;
}

void OpenIGTLsendThread::run(){
    bool sent = false;
    while ( !this->isInterruptionRequested() ) {
        sent = this->s->send();
        //sent is false when the send function didn't succeed.
        //This usually means that the init hasn't been done or that
        //we were disconnected from the server for some reason
        if ( !sent ){
            //if it didn't work, we wait a little bit and we'll try again later
            qInfo() << "[OpenIGTLSendThread] Will wait a second and try receiving again.";
            usleep(1000000);
        }
    }
}

void OpenIGTLsendThread::end(){
    qInfo() << "[OpenIGTLSendThread] Stopping send thread.";
    s->closeSocket();
    this->requestInterruption();
    //TODO: Does this exit cleanly?
}
