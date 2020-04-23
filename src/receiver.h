#ifndef RECEIVER_H
#define RECEIVER_H

#pragma once
//TODO: clean includes
#include <iostream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <algorithm>
#include <vector>
#include <cstdlib>

#include <QObject>
#include <QVideoProbe>
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
#include <QAbstractVideoSurface>
#include <QVideoSurfaceFormat>
#include <QAbstractVideoBuffer>
#include <QImage>
#include <QHostAddress>
#include <QNetworkInterface>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlStatusMessage.h"
#include "igtlCommandMessage.h"
#include "igtlClientSocket.h"
#include "igtlServerSocket.h"
#include "igtl_header.h"
#include "igtlVideoMessage.h"
#include "igtl_video.h"
#include "igtlH264Decoder.h"
#include "igtlUDPClientSocket.h"
#include "igtlMessageRTPWrapper.h"
#include "igtlVideoStreamIGTLinkReceiver.h"

#include <configs/constants.h>

using namespace igtl;

class Receiver : public QObject{
    Q_OBJECT

public:
    Receiver(QObject *parent = nullptr, QString add = "" );
    ~Receiver();

    QObject * parent;
    bool receive();
    bool receiveCommand();
    bool connectVideo();
    bool connectCommands();
    QImage * get_current_image() { return act_img; }
    int getPortVideo() { return port_video; }
    int getPortCommands() { return port_commands; }
    void closeSocket();
    igtl::MessageRTPWrapper::Pointer getWrapper() { return rtpWrapper; }
    igtl::UDPClientSocket::Pointer getUDPSocket() { return UDPSocket; }
    bool keepReceiving() { return stillReceiving; }
    void WriteTimeInfo(unsigned char * UDPPacket);
    QMutex * getRTPMutex() { return glock; }
    QString getClientAddress() { return thisaddress; }

public slots:
    void change_port_video( int p );
    void change_host( std::string str );

signals:
    void new_image_received();
    void TrackingStatusChanged( bool );
    void serverFound();

private:

    void getParamsFromServer();

    std::string hostname = SERVER_ADDRESS;
    QString thisaddress = "127.0.0.1";
    int port_video = VIDEO_RECEIVER_PORT;
    int port_commands = COMMANDS_RECEIVER_PORT;
    int version = IGTL_HEADER_VERSION_2;
    bool connected_video = false;
    bool connected_commands = false;
    bool initialized = false;

    int width = -99;
    int height = -99;
    int frame_size = 0;
    QImage * act_img;

    bool stillReceiving = true;
    
    H264Decoder * h264StreamDecoder = new H264Decoder();
    GenericDecoder * decoder = h264StreamDecoder;
    SourcePicture * decodedPic = new SourcePicture();
    igtl::VideoMessage::Pointer videoMsg = igtl::VideoMessage::New();
    igtl::StatusMessage::Pointer statusMsg = igtl::StatusMessage::New();
    igtl::MessageHeader::Pointer headerMsg = igtl::MessageHeader::New();
    
    igtl::ClientSocket::Pointer commandsSocket = igtl::ClientSocket::New();
    
    igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
    igtl::UDPClientSocket::Pointer UDPSocket;
    igtl::MessageRTPWrapper::Pointer rtpWrapper;
    QMutex * glock;
    
    int readThreadID = -99;
    int unwrapThreadID = -99;
};

#endif
