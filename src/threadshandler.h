#ifndef THREADSHANDLER_H
#define THREADSHANDLER_H

#pragma once
#include <QObject>
#include <QString>

#include "openigtlsendthread.h"
#include "videoreceivethread.h"
#include "commandsreceivethread.h"

class QUdpSocket;

class ThreadsHandler : public QObject
{
    Q_OBJECT
public:
    explicit ThreadsHandler( QObject *parent = nullptr );
    ~ThreadsHandler();
    void setSenderVideo( Sender * s );
    void setSenderCommand( Sender * s );
    void setReceiver( Receiver * r );
    void startSenders();
    void startReceiver();
    Q_INVOKABLE int getReceivingPortVideo() const { return r->getPortVideo(); }
    Q_INVOKABLE int getReceivingPortCommands() const { return r->getPortCommands(); }
    Q_INVOKABLE int getSendingPortVideo() const { return vs->getPort(); }
    Q_INVOKABLE int getSendingPortCommands() const { return cs->getPort(); }
    Q_INVOKABLE QString getClientAddress() const { return r->getClientAddress(); }
    Q_INVOKABLE QString getServerAddress() const { return vs->getServerAddress(); }
    Q_INVOKABLE bool getSendingVideo() const { return vs->sending(); }

signals:
    void serverFound();
    void readyToExit();

public slots:
    void toggleSender( bool b );
    void toggleReceiver( bool b );

    void receivedBroadcast();
    void quit();
    
private:
    Sender * vs;
    Sender * cs;
    Receiver * r;
    VideoReceiveThread * vrt;
    CommandsReceiveThread * crt;
    OpenIGTLsendThread * videoSendThread;
    OpenIGTLsendThread * commandsSendThread;
    QUdpSocket * udpSocket = nullptr;
    QString lastestBroadcast = QString("");
};

#endif
