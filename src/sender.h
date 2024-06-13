#pragma once

//TODO: clean includes
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <queue>
#include <sstream>
#include <map>
#include <string.h>

#include <QObject>
#include <QVideoProbe>
#include <QCamera>
#include <QThread>
#include <QMutex>
#include <QInputDialog>
#include <QReadWriteLock>
#include <QtCore/QStandardPaths>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickView>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlCommandMessage.h"
#include "igtlClientSocket.h"
#include "igtlServerSocket.h"
#include "igtlVideoMessage.h"
#include "igtlUDPServerSocket.h"
#include "igtlMessageRTPWrapper.h"
#include "igtlMultiThreader.h"
#include "igtl_video.h"
#include "igtl_types.h"
#include "igtl_header.h"
#include "igtl_util.h"
#include "igtlH264Encoder.h"
#include "igtlH264Decoder.h"
#include "igtlI420Encoder.h"
#include "igtlI420Decoder.h"

#include <configs/constants.h>

#define EPSILON 0.00001

using namespace igtl;

class Sender : public QObject{
    Q_OBJECT
public:
    Sender( QObject * parent = nullptr, QString add = "", int port = 99999 );
    ~Sender();
    
    QObject * parent;
    virtual bool send() = 0;
    virtual bool connect() = 0;
    
    void closeSocket();
    int getPort() { return port; }
    QString getServerAddress() { return hostname; }
    void setServerAddress( const QString address ) {
        if( address == hostname ){
            return;
        }
        hostname = address;
        emit serverAddressChanged();
    }

    bool sending(){ return m_sending; }
    void setSending( const bool b ){
        if( b == m_sending ){
            return;
        }
        m_sending = b;
        emit sendingStatusChanged();
    }

signals:
    void sendingStatusChanged();
    void serverAddressChanged();

public slots:
    void change_host( std::string str );
    void change_port( int p );

protected:
    
    //socket to send on:
    igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
    igtl::ServerSocket::Pointer tcpServerSocket = igtl::ServerSocket::New();
    igtl::UDPServerSocket::Pointer udpServerSocket = igtl::UDPServerSocket::New();
    igtl::MessageRTPWrapper::Pointer rtpWrapper = igtl::MessageRTPWrapper::New();
    
    igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
    igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
    
    igtl::TimeStamp::Pointer ServerTimer  = igtl::TimeStamp::New();
        
    bool init_done = false;
    bool connected = false;
    int msgID = 0;
    
private:
    
    bool m_sending = false;
    ushort port = 99999;

    int version = 2;
    QString thisaddress = QString("127.0.0.1");
    QString hostname = "";

    QMutex mutex;
    QReadWriteLock rwlock;

};
