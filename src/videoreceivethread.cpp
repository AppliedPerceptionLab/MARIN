#include <QDebug>
#include "videoreceivethread.h"

VideoReceiveThread::VideoReceiveThread(QObject *parent) : QThread(parent){
    setTerminationEnabled();
    //TODO
}

void VideoReceiveThread::setReceiver(Receiver * ss){
    s = ss;
}

void VideoReceiveThread::run(){
    bool received = false;
    while ( !this->isInterruptionRequested() ) {
        received = this->s->receive();
        //wait 33ms:
//        usleep(33000);
        usleep(11000);
        //received is false when the send function didn't succeed.
        //This usually means that the init hasn't been done or that
        //we were disconnected from the server for some reason
        if ( !received ){
            //if it didn't work, we wait 1 second and we'll try again later
            qInfo() << "[VideoReceiveThread] Will wait a second and try receiving again.";
//            usleep(1000000);
        }
    }
}

void VideoReceiveThread::end(){
    qInfo() << "[VideoReceiveThread] Stopping receive thread.";
    s->closeSocket();
    this->requestInterruption();
    //TODO: Does this exit cleanly?
}
