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

using namespace igtl;

class SenderCommand : public Sender{
    Q_OBJECT
public:
    SenderCommand( QObject * parent = nullptr, QString add = "", int port = 99999 );
    ~SenderCommand();
    
    bool send() override;
    bool connect() override;
    
public slots:
    void toggleAnatomy( int i, bool b );
    void toggleQuadView( bool b);
    void navigateSlice( SliceView slice, double x, double y );
    void rotateView( double dx, double dy, double z );
    void reregisterAR( double dx, double dy, double a );
    void freezeFrame( bool value );
    void resetReregistration();
    void sendPointSet( std::vector<std::pair<int,int>> points );
    void arbitraryCommand( std::string str );

private:

    int command_number = 0;
    bool newCommand = false;
    std::queue<Command> commands;
    
    igtl::CommandMessage::Pointer commandMsg;

};
