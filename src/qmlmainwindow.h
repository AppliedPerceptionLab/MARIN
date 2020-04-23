#ifndef QMLMAINWINDOW_H
#define QMLMAINWINDOW_H

#include <QObject>
#include <QMainWindow>
#include "receiver.h"

class GLWidget;
class QQuickWidget;
class QQmlEngine;

class QmlMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QmlMainWindow();
    QmlMainWindow( int width, int height );
    ~QmlMainWindow();

    void paint();
    void executeSetDisplayMode( int i );
    void executeStartReregister();
    void executeStopReregister();
    void executeStartFreeze();
    void executeStopFreeze();
    void executeOpenSettingsPanel();
    void executeCloseSettingsPanel();
    void executeOpenAnatomyPlanes();
    void executeCloseAnatomyPlanes();
    void executeShowFilters();
    void executeHideFilters();
    void executeShowReregisterOptions();
    void executeHideReregisterOptions();
    void executeShowServerConnectionError();
    void executeHideServerConnectionError();
    void executeStartSender();
    void executeStopSender();
    void executeStartReceiver();
    void executeStopReceiver();
    void executeStartPhotoRegistration();
    void executeStopPhotoRegistration();
    void executeStartPickPoints();
    void executeStopPickPoints();
    void executeExit();

    void enableTumourFilter();
    void enableSkinFilter();
    void enableVesselFilter();
    void disableTumourFilter();
    void disableSkinFilter();
    void disableVesselFilter();
    
    void enableRotateMode();
    void enableTranslateMode();
    void disableRotateMode();
    void disableTranslateMode();
    void resetReregistration();

    void anatomy1Toggled( bool b ){ emit anatomyToggledSignal( 1, b ); }
    void anatomy2Toggled( bool b ){ emit anatomyToggledSignal( 2, b ); }
    void anatomy3Toggled( bool b ){ emit anatomyToggledSignal( 3, b ); }
    void navigateSlice( SliceView sv, double x, double y ){ emit navigateSliceSignal( sv, x, y ); }
    void reregisterAR( double dx, double dy, double a ){ emit reregisterARSignal( dx, dy, a ); }
    void rotateView( double dx, double dy, double z ){ emit rotateViewSignal( dx, dy, z ); }
    void freezeFrame( bool value ){ emit freezeFrameSignal( value ); }
    
    void setReceiver( Receiver * r );
    void setCameraImageResolution( int w, int h );
    void setPixelFormat( QVideoFrame::PixelFormat format );

    QQmlEngine * getEngine() { return engine; }
    
    GLWidget * getWidget();

    void setMoveView( bool b );

public slots:
    void new_camera_frame( QVideoFrame qvf );
    void update_widget();
    void executeAction(const int &action);
    void enableFilter(const int &filter);
    void disableFilter(const int &filter);
    void enableMode(const int &mode);
    void disableMode(const int &mode);
    void serverFound(){
        if( server_found ){
            return;
        }else{
            executeHideServerConnectionError();
            server_found = true;
        }
    }

signals:
    void anatomyToggledSignal( int, bool );
    void setQuadViewSignal( bool );
    void navigateSliceSignal( SliceView, double, double );
    void freezeCamera( bool freeze );
    void reregisterARSignal( double, double, double );
    void rotateViewSignal( double, double, double );
    void freezeFrameSignal( bool );
    void toggleSenderSignal( bool );
    void toggleReceiverSignal( bool );
    void resetReregistrationSignal();
    void sendPointSetSignal( std::vector<std::pair<int, int>> );
    void exitSignal();

private:
    
    GLWidget * GL_widget;
    
    int screen_width = -99;
    int screen_height = -99;
    bool move_view_on = true;
    bool picking_points = true;
    std::vector<std::pair<int, int>> point_set;
    
    bool server_found = false;
    
    QVideoFrame * qvf = new QVideoFrame();
    
    QQmlEngine * engine;

    QQuickWidget * toolBarView;
    QQuickWidget * profileView;
    QQuickWidget * profileInfoView;
    QQuickWidget * settingsPanelView;
    QQuickWidget * filtersView;
    QQuickWidget * reregisterView;
    QQuickWidget * serverConnectionView;
    QQuickWidget * mainWindowWrapper;
    QQuickWidget * cameraButtonView;
    
protected:
    void mousePressEvent( QMouseEvent * event );
    void mouseMoveEvent( QMouseEvent * event );
    
    int getWidth() { return screen_width; }
    int getHeight() { return screen_height; }
    
};

#endif
