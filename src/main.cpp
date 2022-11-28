#include <QApplication>
#include <QVideoFrame>

#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QVideoProbe>
#include <QList>
#include <QQuickWidget>
#include <QDesktopWidget>
#include "sender.h"
#include "receiver.h"
#include "threadshandler.h"
#include "qmlmainwindow.h"
#include "camera.h"
#include <QString>
#include <QDateTime>
#include <QCameraFocus>
#include <QCamera>
#include <QHostAddress>
#include <QScreen>

#include "videoFrameGrabber.h"

int main(int argc, char *argv[]){

//    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app( argc, argv );

    //internal vars to set:
    int camera_width = -99;
    int camera_height = -99;
    int screen_resolution_width = -99;
    int screen_resolution_height = -99;
    int full_res_width = -99;
    int full_res_height = -99;
    QVideoFrame::PixelFormat format = QVideoFrame::Format_Invalid;

    //init:
    //Fetch screen resolution:
    screen_resolution_width = QGuiApplication::screens().at(0)->size().width();
    screen_resolution_height = QGuiApplication::screens().at(0)->size().height();
    //Fetch own address:
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QHostAddress address;
    for( int i = 0; i < addresses.length(); i++ ) {
        if ( addresses.at(i).protocol() == QAbstractSocket::IPv4Protocol && addresses.at(i) != QHostAddress(QHostAddress::LocalHost) ){
            //we take the first address that matches and leave (the second one is bluetooth usually)
            //TODO: this should eventually be set by user in a proper way
            address = addresses.at(i);
//            std::string ipname = address.toString().toStdString().c_str();
//            break;
        }
    }
    QString thisaddress = address.toString();
    qDebug() << "[Main][Debug] This device's IP address is: " << thisaddress.toStdString().c_str();
    qDebug() << "[Main][Debug] Screen resolution is: " << screen_resolution_width << "by" << screen_resolution_height;

//    QVideoProbe * qvp = new QVideoProbe;
    VideoFrameGrabber * grabber = new VideoFrameGrabber();
    Camera * camera = new Camera();
    Receiver * receiver = new Receiver( nullptr, thisaddress );

    //Create OpenGL widget:
    QmlMainWindow window( screen_resolution_width, screen_resolution_height );
    window.setReceiver( receiver );

    //create and set camera:
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach ( const QCameraInfo &cameraInfo, cameras ) {
        //use backfacing camera:
        if ( cameraInfo.position() == QCamera::BackFace ) {
            qDebug() << "[Main][Debug] Cam info"  << cameraInfo;
            
            camera->setCamera(new QCamera( cameraInfo ) );
            //Choose focus modes (available focus modes for iPad 6th Gen are AutoFocus and ContinuousFocus):
            QCameraFocus * qcf = camera->getCamera()->focus();
            QCameraViewfinder * viewfinder = new QCameraViewfinder();
            viewfinder->show();
            camera->getCamera()->setViewfinder( viewfinder );
            camera->getCamera()->setCaptureMode( QCamera::CaptureVideo );
            QCameraViewfinderSettings viewfinderSettings = camera->getCamera()->viewfinderSettings();
//            if (qvp->setSource((QMediaObject *)camera->getCamera())) {
//                qDebug() << " set source succeed";
//            }
            
            camera->getCamera()->start();
            qDebug() << "[Main][Debug] Camera"  << camera->getCamera();
            //output info on supported resolutions and check user-defined params:
            bool user_params_ok = false;
            QList<QCameraViewfinderSettings> ff = camera->getCamera()->supportedViewfinderSettings();
            qDebug() << "[Main][Debug] The" << ff.length() << "supported camera resolutions are:";
            for ( int i = 0 ; i < ff.length() ; i++ ){
                qDebug() << "\t" << i << ":" << ff[i].resolution() << "," << ff[i].pixelFormat();
                if ( ff[i].resolution().height() == CAMERA_HEIGHT &&
                     ff[i].resolution().width() == CAMERA_WIDTH &&
                     ff[i].pixelFormat() == CAMERA_FORMAT ){
                    camera_width = CAMERA_WIDTH;
                    camera_height = CAMERA_HEIGHT;
                    format = CAMERA_FORMAT;
                    qDebug() << "Is the user selected focus mode supported? : " << qcf->isFocusModeSupported( QCameraFocus::FOCUS_MODE );
                    qcf->setFocusMode( QCameraFocus::FOCUS_MODE );
                    grabber->setVideoFinderSettings(&ff[i]);
                    assert(grabber->setSource(camera->getCamera()));
                    user_params_ok = true;
                }
            }
            //set max res:
            full_res_width =ff[ff.length()-1].resolution().width();
            full_res_height =ff[ff.length()-1].resolution().height();
            qDebug() << "[Main][Debug] Maximum available resolution: (" << full_res_width << "," << full_res_height << ")";

            //if user-defined params didn't fit with any of the supported resolutions, take the first one available:
            if( !user_params_ok ){
                qCritical() << "[Main][Error] User-defined camera parameters invalid, taking default parameters instead." ;
                camera_width = ff[0].resolution().width();
                camera_height = ff[0].resolution().height();
                format = ff[0].pixelFormat();
            }
            //Pass values to GLWidget (window):
//            camera_width = 1366;
//            camera_height = 1024;
//            format = QVideoFrame::Format_ARGB32;
            qDebug() << "[Main][Debug] Camera width set to: "  << camera_width;
            qDebug() << "[Main][Debug] Camera height set to: "  << camera_height;
            qDebug() << "[Main][Debug] Camera format set to: "  << format;
            viewfinderSettings.setResolution( camera_width, camera_height );
            viewfinderSettings.setPixelFormat( format );
            window.setCameraImageResolution( camera_width, camera_height );
            window.setPixelFormat( format );
            camera->getCamera()->setViewfinderSettings( viewfinderSettings );
            //show focus modes information:
            QString modes = QString("");
            QFlags<QCameraFocus::FocusMode> focusmodes = qcf->focusMode();
            qDebug() << "[Main][Debug] Focus mode set to:" << focusmodes;
            break;
        }
    }

    Sender * sender = new Sender( nullptr, camera_width, camera_height, thisaddress, full_res_width, full_res_height );
    sender->setCamera( camera->getCamera() );

    
//    assert( qvp->setSource( camera->getCamera() ) );
    //Copy frames to buffer every time the QCamera has one available:
//    QObject::connect( qvp, SIGNAL( videoFrameProbed( const QVideoFrame &) ), sender, SLOT( copy_frame( const QVideoFrame &) ) );
    QObject::connect( grabber, SIGNAL( videoFrameProbed( const QVideoFrame &) ), sender, SLOT( copy_frame( const QVideoFrame &) ) );

    //create threads handler:
    ThreadsHandler * th = new ThreadsHandler();
    th->setReceiver( receiver );
    th->setSender( sender );
    th->startSender();
    th->startReceiver();

    //connect signals and slots:
    QObject::connect( receiver, SIGNAL( new_image_received() ),                             &window,    SLOT( update_widget() ) );
//    QObject::connect( qvp,      SIGNAL( videoFrameProbed( const QVideoFrame &)),                    &window,    SLOT( new_camera_frame( const QVideoFrame &) ) );
    QObject::connect( grabber,  SIGNAL( videoFrameProbed( const QVideoFrame &)),            &window,    SLOT( new_camera_frame( const QVideoFrame & ) ) );
    QObject::connect( camera->getCamera(),      SIGNAL( videoFrameProbed( const QVideoFrame )),                    &window,    SLOT( new_camera_frame( const QVideoFrame ) ) );
    QObject::connect( &window,  SIGNAL( toggleSenderSignal( bool ) ),                       th,         SLOT( toggleSender( bool ) ) );
    QObject::connect( &window,  SIGNAL( toggleReceiverSignal( bool ) ),                     th,         SLOT( toggleReceiver( bool ) ) );
    QObject::connect( th,       SIGNAL( serverFound() ),                                    &window,    SLOT( serverFound() ) );
    QObject::connect( receiver, SIGNAL( serverFound() ),                                    &window,    SLOT( serverFound() ) );
    QObject::connect( &window,  SIGNAL( anatomyToggledSignal( int, bool ) ),                th,         SLOT( anatomyToggled( int, bool ) ) );
    QObject::connect( &window,  SIGNAL( setQuadViewSignal( bool ) ),                        th,         SLOT( quadviewToggled( bool ) ) );
    QObject::connect( &window,  SIGNAL( navigateSliceSignal( SliceView, double, double ) ), th,         SLOT( navigateSlice( SliceView, double, double ) ) );
    QObject::connect( &window,  SIGNAL( reregisterARSignal( double, double, double ) ),     th,         SLOT( reregisterAR( double, double, double ) ) );
    QObject::connect( &window,  SIGNAL( rotateViewSignal( double, double, double ) ),       th,         SLOT( rotateView( double, double, double ) ) );
    QObject::connect( &window,  SIGNAL( freezeFrameSignal( bool ) ),                        th,         SLOT( freezeFrame( bool ) ) );
    QObject::connect( &window,  SIGNAL( resetReregistrationSignal() ),                      th,         SLOT( resetReregistration() ) );
    QObject::connect( &window,  SIGNAL( exitSignal() ),                                     th,         SLOT( quit() ) );
    QObject::connect( th,       SIGNAL( readyToExit() ),                                    &app,       SLOT( quit() ) );
    QObject::connect( &window,  SIGNAL( freezeCamera(bool) ),                               camera,     SLOT( freezeCamera(bool) ) );
    QObject::connect( &window,  SIGNAL( sendPointSetSignal( std::vector<std::pair<int,int>> ) ), th,         SLOT( sendPointSet( std::vector<std::pair<int,int>> ) ) );

    //Give threadhandler context access to QML:
    //TODO: This should be fixed
    window.getEngine()->rootContext()->setContextProperty( "th", th );

    //launch:
    window.show();
    app.exec();

    delete th;
//    delete qvp;
    delete camera;

    qDebug() << "Exiting normally.";

    return 0;
}
