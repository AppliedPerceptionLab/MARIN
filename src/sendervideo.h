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

#include "configs/constants.h"
#include "sender.h"

//TODO: Use this instead of current buffers
typedef struct frame{
    int frame_number;
    uchar * data;
    size_t size;
}frame;

using namespace igtl;

class SenderVideo : public Sender{
    Q_OBJECT
public:
    SenderVideo( QObject * parent = nullptr, QString add = "", int port = 99999, int w = 0 , int h = 0, int frw = 0, int frh = 0 );
    ~SenderVideo();
    
    bool send() override;
    bool connect() override;

    void setCamera( QCamera * camera ){
        this->camera = camera;
    }

public slots:
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
    bool m_send_full_res_picture = false;

    bool reading_1 = false;
    bool slot1_being_written = false;
    bool slot2_being_written = false;
    bool slot1_being_read = false;
    bool slot2_being_read = false;
    bool newest_is_1 = false;
    frame slot1;
    frame slot2;

    H264Encoder::Pointer h264StreamEncoder;
    I420Encoder::Pointer I420StreamEncoder;
    GenericEncoder::Pointer encoder;
    SourcePicture* srcPic = new SourcePicture();
    
    igtl::VideoMessage::Pointer videoMessage;
    igtl::ImageMessage::Pointer imageMessage;
    
    igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
    igtl::MutexLock::Pointer glock = igtl::MutexLock::New();
    
    igtl::TimeStamp::Pointer ServerTimer  = igtl::TimeStamp::New();
};
