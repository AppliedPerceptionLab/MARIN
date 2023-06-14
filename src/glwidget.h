#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QImage>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

#include "receiver.h"
#include "gestureHandler.h"
#include "constants.h"

//the AR field cannot be less than RESIZE_LIMIT pixels of radius
#define RESIZE_LIMIT 10

class QmlMainWindow;

class GLWidget : public QOpenGLWidget, GestureHandler{
    Q_OBJECT

public:
    GLWidget( QWidget *parent, int w, int h );
    void setReceiver( Receiver * r );
    void setCameraImageResolution( int w, int h );
    void setPixelFormat( QVideoFrame::PixelFormat format );
    void changeGaussian( float val );
    void changeRadius( float inner, float outer );
    ~GLWidget() override;
    bool isServerConnectionAlive();
    void pinch() override;
    void pan() const override;
    void swipePrevious() const override;
    void swipeNext() const override;
    void setDisplayMode( int mode );
    bool getRotateView(){ return rotateView; }
    bool getReregister(){ return reregister; }
    bool getRotate(){ return rotate; }
    bool getTranslate(){ return translate; }

public slots:
    void animate();
    void initializeGL() override;
    void resizeGL( int x, int y ) override;
    void paintGL() override;
    void set_camera_image( QVideoFrame * qvf );
    void moveCursor( int x, int y );
    void TrackingStatusChanged(bool status);
    void setReregister(bool b) { reregister = b; }
    void setRotateView(bool b) { rotateView = b; }
    void setRotate(bool b) { rotate = b; }
    void setTranslate(bool b) { translate = b; }

protected:
    bool event(QEvent *event) override;

private:
    QmlMainWindow * window;
    QOpenGLFunctions * f;
    std::string readFile(const char *filePath);
    bool loadShaderSource( const std::string& filename, std::string &out );
    bool debug = true;
    bool init = false;

    QImage * img = new QImage( ":/resources/warning_blue.jpg", Q_NULLPTR );
    QVideoFrame * qvf_camera_image;
    Receiver * receiver;
    int screen_width = -99;
    int screen_height = -99;
    int image_width = img->width();
    int image_height = img->height();
    uint err = 99;
    int camera_width = -99;
    int camera_height = -99;
    uchar * rgb;
    uchar * argb;
    //image parameters:
    QVideoFrame::PixelFormat pixel_format = QVideoFrame::PixelFormat::Format_Invalid;
    //opengl vars:
    QOpenGLVertexArrayObject * m_vao1;
    QOpenGLBuffer * m_positionBuffer;
    GLuint shader_programme;
    GLuint vs;
    GLuint fs;
    GLuint vbo;
    GLuint ebo;
    GLuint augm_texture;
    GLuint camera_texture;
    GLuint Yplane;
    GLuint Uplane;
    GLuint Vplane;
    GLint texLoc;
    //gradient and transparency shading:
    bool UseTransparency = true;
    bool UseGradient = true;
    bool ShowMask = true;
    float TransparencyPosition[2];
    float TransparencyRadius[2];
    float Resolution[2];
    bool trackerOK = true;
    float GaussianFactor = 0.05;
    bool reregister = false;
    bool rotate = false;
    bool translate = false;
    bool rotateView = false;
    
    int DisplayMode = DisplayMode::AugmentedReality;
    int AlphaChannelColour = ALPHA_CHANNEL_COLOUR;
};

#endif
