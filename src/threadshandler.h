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
    void setSender( Sender * s );
    void setReceiver( Receiver * r );
    void startSender();
    void startReceiver();
    Q_INVOKABLE int getReceivingPortVideo() const { return r->getPortVideo(); }
    Q_INVOKABLE int getReceivingPortCommands() const { return r->getPortCommands(); }
    Q_INVOKABLE int getSendingPortVideo() const { return s->getPortVideo(); }
    Q_INVOKABLE int getSendingPortCommands() const { return s->getPortCommands(); }
    Q_INVOKABLE QString getClientAddress() const { return r->getClientAddress(); }
    Q_INVOKABLE QString getServerAddress() const { return s->getServerAddress(); }
    Q_INVOKABLE bool getSendingVideo() const { return s->sendingVideo(); }

signals:
    void serverFound();
    void readyToExit();

public slots:
    void toggleSender( bool b );
    void toggleReceiver( bool b );

    void quit();
    void anatomyToggled( int i , bool b ){ st->anatomyToggled( i, b ); }
    void quadviewToggled( bool b ){ st->quadviewToggled( b ); }
    void navigateSlice( SliceView sv, double x, double y ){ st->navigateSlice( sv, x, y ); }
    void reregisterAR( double dx, double dy, double a ){ st->reregisterAR( dx, dy, a ); }
    void rotateView( double dx, double dy, double z ){ st->rotateView( dx, dy, z ); }
    void freezeFrame( bool b ){ st->freezeFrame( b ); }
    void resetReregistration( ){ st->resetReregistration( ); }
    void receivedBroadcast();
    void sendPointSet( std::vector<std::pair<int,int>> points ){ st->sendPointSet( points ); }
    
private:
    Sender * s;
    Receiver * r;
    VideoReceiveThread * vrt;
    CommandsReceiveThread * crt;
    OpenIGTLsendThread * st;
    QUdpSocket * udpSocket = nullptr;
    QString lastestBroadcast = QString("");
};

#endif
