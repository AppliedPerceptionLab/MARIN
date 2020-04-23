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

#include <configs/constants.h>

#define EPSILON 0.00001

//TODO: Use this instead of current buffers
typedef struct frame{
    int frame_number;
    uchar * data;
    size_t size;
}frame;

using namespace igtl;

class Sender : public QObject{
    Q_OBJECT
public:
    Sender( QObject * parent = nullptr, int w = 0 , int h = 0, QString add = "", int frw = 0, int frh = 0 );
    ~Sender();
    
    QObject * parent;
    bool send();
    bool connectVideo();
    bool connectCommands();
    void closeSocket();
    void toggleAnatomy( int i, bool b );
    void toggleQuadView( bool b);
    void navigateSlice( SliceView slice, double x, double y );
    void rotateView( double dx, double dy, double z );
    void reregisterAR( double dx, double dy, double a );
    void freezeFrame( bool value );
    void resetReregistration();
    void sendPointSet( std::vector<std::pair<int,int>> points );
    void arbitraryCommand( std::string str );
    int getPortVideo() { return port_video; }
    int getPortCommands() { return port_commands; }
    QString getServerAddress() { return hostname; }
    void setServerAddress( const QString address ) {
        if( address == hostname ){
            return;
        }
        hostname = address;
        emit serverAddressChanged();
    }
    void setCamera( QCamera * camera ){
        this->camera = camera;
    }

    bool sendingVideo(){ return m_sendingVideo; }
    void setSendingVideo( const bool b ){
        if( b == m_sendingVideo ){
            return;
        }
        m_sendingVideo = b;
        emit sendingVideoStatusChanged();
    }

signals:
    void sendingVideoStatusChanged();
    void serverAddressChanged();

public slots:
    void change_host( std::string str );
    void change_port( int p );
    void copy_frame( QVideoFrame f );

private:
    QCamera * camera = nullptr;
    bool setEncoder( int w, int h );
    
    QVideoFrame::PixelFormat format = QVideoFrame::Format_Invalid;

    int argb_buffer_size = -99;
    uchar * send_buffer;
    uchar * converted;
    uchar * full_res_frame;
    
    int width = -99;
    int height = -99;
    int full_res_width = -99;
    int full_res_height = -99;
    int input_image_size = -99;
    int bytes_per_line = 0;
    int copy_number = 0;
    int command_number = 0;
    bool newCommand = false;
    std::queue<Command> commands;
    bool m_sendingVideo;
    bool m_send_full_res_picture = false;
    
    ushort port_video = VIDEO_SENDER_PORT;
    ushort port_commands = COMMANDS_SENDER_PORT;

    int version = 2;
    QString thisaddress = QString("127.0.0.1");
    QString hostname = "";

    QMutex mutex;
    QReadWriteLock rwlock;
    bool reading_1 = false;
    bool slot1_being_written = false;
    bool slot2_being_written = false;
    bool slot1_being_read = false;
    bool slot2_being_read = false;
    bool newest_is_1 = false;
    frame slot1;
    frame slot2;
    bool init_done = false;
    bool connected_video = false;
    bool connected_commands = false;
    bool connected_images = false;

    H264Encoder::Pointer h264StreamEncoder;
    GenericEncoder::Pointer encoder;
    SourcePicture* srcPic = new SourcePicture();
    
    igtl::CommandMessage::Pointer commandMsg;
    igtl::VideoMessage::Pointer videoMessage;
    igtl::ImageMessage::Pointer imageMessage;
    
    //socket to send commands on:
    igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
    igtl::UDPServerSocket::Pointer udpCommandsServerSocket = igtl::UDPServerSocket::New();
    //socket to send video on:
    igtl::UDPServerSocket::Pointer udpVideoServerSocket = igtl::UDPServerSocket::New();
    igtl::MessageRTPWrapper::Pointer rtpWrapper = igtl::MessageRTPWrapper::New();
    
    igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
    igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
    
    igtl::TimeStamp::Pointer ServerTimer  = igtl::TimeStamp::New();
};
