#include <QApplication>
#include <QGuiApplication>
#include <QVideoFrame>
#include <QPermissions>

#include <QMediaDevices>
#include <QCameraDevice>
#include <QList>
#include <QQuickWidget>
#include <QQuickWindow>
#include "sendervideo.h"
#include "sendercommand.h"
#include "receiver.h"
#include "threadshandler.h"
#include "qmlmainwindow.h"
#include "camera.h"
#include <QString>
#include <QDateTime>
#include <QCamera>
#include <QHostAddress>
#include <QScreen>

#include "videoFrameGrabber.h"

int main(int argc, char *argv[]){

    qputenv("QSG_RHI_BACKEND", "opengl");
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    QApplication app( argc, argv );

    //internal vars to set:
    int camera_width = -99;
    int camera_height = -99;
    int screen_resolution_width = -99;
    int screen_resolution_height = -99;
    int full_res_width = -99;
    int full_res_height = -99;
    QVideoFrameFormat::PixelFormat format = QVideoFrameFormat::Format_Invalid;

    //init:
    //Fetch screen resolution:
    screen_resolution_width = QGuiApplication::screens().at(0)->size().width();
    screen_resolution_height = QGuiApplication::screens().at(0)->size().height();
    //Fetch own address:
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    QHostAddress address;
    for( int i = 0; i < addresses.length(); i++ ) {
        // 169.254.0.0/16 is usually going to be Bluetooth
        if ( addresses.at(i).protocol() == QAbstractSocket::IPv4Protocol && addresses.at(i) != QHostAddress(QHostAddress::LocalHost) && !(addresses.at(i).isInSubnet( QHostAddress("169.254.0.0"), 16)) ){
            //TODO: deal with case where more than one network adapter (isn't an issue on current generation iPads)
            address = addresses.at(i);
            break;
        }
    }
    QString thisaddress = address.toString();
    qDebug() << "[Main][Debug] This device's IP address is: " << thisaddress.toStdString().c_str();
    qDebug() << "[Main][Debug] Screen resolution is: " << screen_resolution_width << "by" << screen_resolution_height;

    VideoFrameGrabber * grabber = new VideoFrameGrabber();
    Camera * camera = new Camera();
    Receiver * receiver = new Receiver( nullptr, thisaddress );

    //Create OpenGL widget:
    QmlMainWindow window( screen_resolution_width, screen_resolution_height );
    window.setReceiver( receiver );

    ThreadsHandler * th = nullptr;

    auto setupAndStart = [&]() {
        //create and set camera:
        const QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
        for ( const QCameraDevice &cameraInfo : cameras ) {
            //use backfacing camera:
            if ( cameraInfo.position() == QCameraDevice::BackFace ) {
                qDebug() << "[Main][Debug] Cam info"  << cameraInfo;
                
                camera->setCamera(new QCamera( cameraInfo ) );
                camera->getCamera()->start();
                qDebug() << "[Main][Debug] Camera"  << camera->getCamera();
                //output info on supported resolutions and check user-defined params:
                bool user_params_ok = false;
                QList<QCameraFormat> ff = camera->getCamera()->cameraDevice().videoFormats();
                qDebug() << "[Main][Debug] The" << ff.length() << "supported camera resolutions are:";
                QCameraFormat selectedFormat;
                for ( int i = 0 ; i < ff.length() ; i++ ){
                    qDebug() << "\t" << i << ":" << ff[i].resolution() << "," << ff[i].pixelFormat();
                    if ( ff[i].resolution().height() == CAMERA_HEIGHT &&
                         ff[i].resolution().width() == CAMERA_WIDTH &&
                         ff[i].pixelFormat() == CAMERA_FORMAT ){
                        camera_width = CAMERA_WIDTH;
                        camera_height = CAMERA_HEIGHT;
                        format = ff[i].pixelFormat();
                        qDebug() << "Is the user selected focus mode supported? : " << camera->getCamera()->isFocusModeSupported( FOCUS_MODE );
                        camera->getCamera()->setFocusMode( FOCUS_MODE );
                        selectedFormat = ff[i];
                        user_params_ok = true;
                    }
                }
                //set max res:
                if (!ff.isEmpty()) {
                    full_res_width = ff[ff.length()-1].resolution().width();
                    full_res_height = ff[ff.length()-1].resolution().height();
                }
                qDebug() << "[Main][Debug] Maximum available resolution: (" << full_res_width << "," << full_res_height << ")";

                //if user-defined params didn't fit with any of the supported resolutions, take the first one available:
                if( !user_params_ok && !ff.isEmpty() ){
                    qCritical() << "[Main][Error] User-defined camera parameters invalid, taking default parameters instead." ;
                    camera_width = ff[0].resolution().width();
                    camera_height = ff[0].resolution().height();
                    format = ff[0].pixelFormat();
                    selectedFormat = ff[0];
                }
                //Pass values to GLWidget (window):
                qDebug() << "[Main][Debug] Camera width set to: "  << camera_width;
                qDebug() << "[Main][Debug] Camera height set to: "  << camera_height;
                qDebug() << "[Main][Debug] Camera format set to: "  << format;
                window.setCameraImageResolution( camera_width, camera_height );
                window.setPixelFormat( format );
                //TODO, maybe verif this doesn't fail?
                camera->getCamera()->setCameraFormat( selectedFormat );
                assert(grabber->setSource(camera->getCamera()));
                //show focus modes information:
                qDebug() << "[Main][Debug] Focus mode set to:" << camera->getCamera()->focusMode();
                break;
            }
        }

        SenderVideo * vs = new SenderVideo( nullptr, thisaddress, PORT_SEND_VIDEO, camera_width, camera_height, full_res_width, full_res_height );
        SenderCommand * cs = new SenderCommand( nullptr, thisaddress, PORT_SEND_COMMANDS );
        vs->setCamera( camera->getCamera() );

        //Copy frames to buffer every time the QCamera has one available:
        QObject::connect( grabber, SIGNAL( videoFrameProbed( const QVideoFrame &) ), vs, SLOT( copy_frame( const QVideoFrame &) ) );

        //create threads handler:
        th = new ThreadsHandler();
        th->setReceiver( receiver );
        th->setSenderVideo( vs );
        th->setSenderCommand( cs );
        th->startSenders();
        th->startReceiver();

        //connect signals and slots:
        QObject::connect( receiver, SIGNAL( new_image_received() ),                             &window,    SLOT( update_widget() ) );
        QObject::connect( grabber,  SIGNAL( videoFrameProbed( const QVideoFrame &)),            &window,    SLOT( new_camera_frame( const QVideoFrame & ) ) );
        // QCamera in Qt6 does not have videoFrameProbed signal, and it's redundant with grabber frame probing.
        QObject::connect( &window,  SIGNAL( toggleSenderSignal( bool ) ),                       th,         SLOT( toggleSender( bool ) ) );
        QObject::connect( &window,  SIGNAL( toggleReceiverSignal( bool ) ),                     th,         SLOT( toggleReceiver( bool ) ) );
        QObject::connect( th,       SIGNAL( serverFound() ),                                    &window,    SLOT( serverFound() ) );
        QObject::connect( receiver, SIGNAL( serverFound() ),                                    &window,    SLOT( serverFound() ) );
        QObject::connect( &window,  SIGNAL( exitSignal() ),                                     th,         SLOT( quit() ) );
        QObject::connect( th,       SIGNAL( readyToExit() ),                                    &app,       SLOT( quit() ) );
        QObject::connect( &window,  SIGNAL( freezeCamera(bool) ),                               camera,     SLOT( freezeCamera(bool) ) );
        
        // Commands:
        QObject::connect( &window,  SIGNAL( anatomyToggledSignal( int, bool ) ),                cs,         SLOT( toggleAnatomy( int, bool ) ) );
        QObject::connect( &window,  SIGNAL( setQuadViewSignal( bool ) ),                        cs,         SLOT( toggleQuadView( bool ) ) );
        QObject::connect( &window,  SIGNAL( navigateSliceSignal( SliceView, double, double ) ), cs,         SLOT( navigateSlice( SliceView, double, double ) ) );
        QObject::connect( &window,  SIGNAL( reregisterARSignal( double, double, double ) ),     cs,         SLOT( reregisterAR( double, double, double ) ) );
        QObject::connect( &window,  SIGNAL( rotateViewSignal( double, double, double ) ),       cs,         SLOT( rotateView( double, double, double ) ) );
        QObject::connect( &window,  SIGNAL( freezeFrameSignal( bool ) ),                        cs,         SLOT( freezeFrame( bool ) ) );
        QObject::connect( &window,  SIGNAL( resetReregistrationSignal() ),                      cs,         SLOT( resetReregistration() ) );
        QObject::connect( &window,  SIGNAL( sendPointSetSignal( std::vector<std::pair<int,int>> ) ), cs,    SLOT( sendPointSet( std::vector<std::pair<int,int>> ) ) );

        window.setThreadsHandler( th );
        //Give threadhandler context access to QML:
        //TODO: This should be fixed
        window.getEngine()->rootContext()->setContextProperty( "th", th );

        //launch:
#if defined(Q_OS_IOS)
        if (QGuiApplication::applicationState() == Qt::ApplicationActive) {
            window.show();
        } else {
            QObject::connect(&app, &QGuiApplication::applicationStateChanged, &window, [&window](Qt::ApplicationState state) {
                static bool shown = false;
                if (state == Qt::ApplicationActive && !shown) {
                    window.show();
                    shown = true;
                }
            });
        }
#else
        window.show();
#endif
    };

    QCameraPermission cameraPermission;
    if (app.checkPermission(cameraPermission) == Qt::PermissionStatus::Granted) {
        setupAndStart();
    } else if (app.checkPermission(cameraPermission) == Qt::PermissionStatus::Denied) {
        qWarning() << "[Main] Camera permission previously denied.";
        setupAndStart();
    } else {
        app.requestPermission(cameraPermission, [&setupAndStart](const QPermission &p) {
            qDebug() << "[Main] Camera permission status updated to:" << p.status();
            setupAndStart();
        });
    }

    int exec_res = app.exec();

    if (th) {
        delete th;
    }
    delete camera;

    qDebug() << "Exiting normally.";

    return exec_res;
}
