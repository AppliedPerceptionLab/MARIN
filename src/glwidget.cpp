#include "glwidget.h"
#include "qmlmainwindow.h"
#include <QDebug>

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  CONSTRUCTOR   //////////////////////////////////////////////
GLWidget::GLWidget( QWidget * parent, int w, int h ) : QOpenGLWidget( parent ), GestureHandler(this){
    qDebug() << "GLWidget() >> ";
    this->window = dynamic_cast<QmlMainWindow*>(parent);
    screen_width = w;
    screen_height = h;
    setFixedSize( screen_width, screen_height );
    setAutoFillBackground( false );
    TransparencyPosition[0] = screen_width / 2.;
    TransparencyPosition[1] = screen_height / 2.;
    TransparencyRadius[0] = 500.;
    TransparencyRadius[1] = 600.;
    Resolution[0] = screen_width;
    Resolution[1] = screen_height;

    QList<Qt::GestureType> gestures;
    gestures << Qt::SwipeGesture << Qt::PinchGesture << Qt::PanGesture;

    registerGestures(gestures);

    qDebug() << "<< GLWidget()";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////  DESTRUCTOR   /////////////////////////////////////////////
GLWidget::~GLWidget(){
    delete[] rgb;
    delete[] argb;
    delete m_vao1;
    // Note: deallocation of "receiver" and "qvf_camera_image" is handled in main.cpp
    // delete receiver;
    // delete qvf_camera_image;
    //TODO: The lines below report deallocation memory errors when invoked. Will deal with these later.
    // delete f;
    // delete img;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  ANIMATE   //////////////////////////////////////////////
void GLWidget::animate(){
    
    img = receiver->get_current_image();
    f->glBindTexture(GL_TEXTURE_2D, augm_texture);
    //if the image size is not the same as from the last frame, we update:
    if ( img->width() != image_width || img->height() != image_height ){
        qDebug() << "[GlWidget][Debug] Previous image size was" << image_width << "x" << image_height << " and is now " << img->width() << ", " << img->height();
        image_width = img->width();
        image_height = img->height();
        //TODO: in this case, we create a new texture, but does initializeGL() need to be called again?
        f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, CAMERA_WIDTH, CAMERA_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, img->bits());
    }else{
        f->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, img->width(), img->height(), GL_BGRA, GL_UNSIGNED_BYTE, img->bits());
    }
    f->glBindTexture(GL_TEXTURE_2D, 0);

    //TODO: Perhaps, we should use GLTEXSUBIMAGE2D instead when updating, to test.
    // (and only delete/recreate the texture when the size is changed. Not sure it's an issue anyway.)

    //render:
    update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////  SET_RECEIVER  /////////////////////////////////////////////
void GLWidget::setReceiver(Receiver * r){
    receiver = r;
    connect( r, SIGNAL(TrackingStatusChanged(bool)), this, SLOT(TrackingStatusChanged(bool)));
}

#include <convert.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  SET_CAMERA_IMAGE ///////////////////////////////////////////
void GLWidget::set_camera_image(QVideoFrame * qvf){

    //might get called before everything has been initialized.
    //if so it would crash on GL calls, so we quit since the frame is useless anyway at this point
    if(!init) return;

    //TODO: Maybe I can use the handle() function instead to get the opengl texture id directly and do everything in gpu without having to map it to cpu to give it back to opengl?
    //edit: ... In fact, I'm not sure this is doable...
//    qInfo() << "Handle: " << qvf_camera_image->handle() << ";;type: " << qvf_camera_image->handleType();
    //the above command returns NoHandle, as type, which apparently means it needs to be mapped in order to be accessed.
    //Could be investigated further.

    //map frame:
    qvf_camera_image = qvf;
    qvf_camera_image->map(QAbstractVideoBuffer::ReadOnly);
    //check the format and transform accordingly:
    pixel_format = qvf_camera_image->pixelFormat();
    //QTBUG-79935 (affects versions prior to 5.14.2): No matter what I set in constants.h, the camera resolution is always 1920x1080, which is a problem, as it doesn't have the same aspect ratio.
    //for now format should be Format_ARGB32
    if (pixel_format == QVideoFrame::Format_ARGB32){
        //NO NEED TO CONVERT, JUST PASS THE ARGB IMAGE TO OPENGL
    }else if (pixel_format == QVideoFrame::Format_NV12){
        libyuv::NV12ToARGB(qvf_camera_image->bits(0), camera_width, qvf_camera_image->bits(1), camera_width/2, argb, camera_width*4, camera_width, camera_height);
        //TODO: test
        qCritical() << "[GLWidget][Debug] This format is not yet supported.";
    }else if (pixel_format == QVideoFrame::Format_NV21){
        libyuv::NV21ToARGB(qvf_camera_image->bits(0), camera_width, qvf_camera_image->bits(1), camera_width/2, argb, camera_width*4, camera_width, camera_height);
        //TODO: Never used this, so still need to test.
    }else{
        qCritical() << "[GLWidget][Debug] This format is not yet supported.";
    }

    f->glBindTexture(GL_TEXTURE_2D, camera_texture);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, CAMERA_WIDTH, CAMERA_HEIGHT, 0, GL_BGRA, GL_UNSIGNED_BYTE, qvf_camera_image->bits());
    f->glBindTexture(GL_TEXTURE_2D, 0);

    //release memory and call render:
    qvf_camera_image->unmap();
    update();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////   INITIALIZE_GL   /////////////////////////////////////////////
void GLWidget::initializeGL(){
    //get pointer to functions and print version:
    f = QOpenGLContext::currentContext()->functions();
    if (debug){    //give debug info error codes:
        qInfo() << "GL_VENDOR:" << QString((char*)f->glGetString(GL_VENDOR));
        qInfo() << "GL_RENDERER:" << QString((char*)f->glGetString(GL_RENDERER));
        qInfo() << "GL_VERSION:" << QString((char*)f->glGetString(GL_VERSION));
        qInfo() << "GL_SHADING_LANGUAGE_VERSION:" << QString((char*)f->glGetString(GL_SHADING_LANGUAGE_VERSION));
        qInfo() << "GL_EXTENSIONS:" << QString((char*)f->glGetString(GL_EXTENSIONS));
        qInfo() << "\t" << GL_NO_ERROR << "::= GL_NO_ERROR\n"
                << GL_INVALID_ENUM << "::= GL_INVALID_ENUM\n"
                << GL_INVALID_VALUE << "::= GL_INVALID_VALUE\n"
                << GL_INVALID_OPERATION << "::= GL_INVALID_OPERATION\n"
                << GL_OUT_OF_MEMORY << "::= GL_OUT_OF_MEMORY\n";
    }

    //init:
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if( debug ){ err = f->glGetError(); if (err!=0) qInfo() << "[GL] glclear:" << err; }

    float aspect_ratio = 1.*AUGMENTATION_WIDTH/AUGMENTATION_HEIGHT;
    qDebug() << "[GlWidget][Debug] aspect ratio:" << aspect_ratio;
    //init vertices and triangles:
    GLfloat vertices[] = {
        1.0f,  1.0f, 0.0f,    1.0f, 1.0f, // Top Right
        1.0f, -1.0f, 0.0f,    1.0f, 0.0f, // Bottom Right
        -1.0f, -1.0f, 0.0f,    0.0f, 0.0f,  // Bottom Left
        -1.0f,  1.0f, 0.0f,    0.0f, 1.0f   // Top Left
    };
    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    //Probably not necessary, but it can't hurt...
    f->glClearColor(1.,1.,1.,1.);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] Clear color:" << err; }

    //create VBO:
    f->glGenBuffers(1, &vbo);
    f->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    f->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] Create VBO:" << err; }
    //create EBO:
    f->glGenBuffers(1, &ebo);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    qInfo() << "SIZE OF INDICES: " << sizeof(indices);
    f->glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] Create EBO:" << err; }
    //create VAO:
    m_vao1 = new QOpenGLVertexArrayObject( this );
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] New VAO:" << err; }
    m_vao1->create();
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] Create VAO:" << err; }
    m_vao1->bind();
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] Bind VAO:" << err; }
    //create attrib arrays for vertices and texCoords:
    f->glVertexAttribPointer(0, 3, GL_FLOAT,GL_FALSE, 5 * sizeof(GLfloat), NULL);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] VERTEX_ATTRIB_POINTER:" << err; }
    f->glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] VERTEX_ATTRIB_POINTER:" << err; }
    f->glEnableVertexAttribArray(0);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] ENABLE_VERTEX_ATTRIB:" << err; }
    f->glEnableVertexAttribArray(1);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] ENABLE_VERTEX_ATTRIB:" << err; }

    std::string vertShaderStr = readFile(":/resources/vert.glsl");
    std::string fragShaderStr = readFile(":/resources/frag.glsl");

    const char * vertex_shader  = vertShaderStr.c_str();
    const char * fragment_shader  = fragShaderStr.c_str();

    //compile shaders:
    vs = f->glCreateShader(GL_VERTEX_SHADER);
    f->glShaderSource(vs, 1, &vertex_shader, NULL);
    f->glCompileShader(vs);
    fs = f->glCreateShader(GL_FRAGMENT_SHADER);
    f->glShaderSource(fs, 1, &fragment_shader, NULL);
    f->glCompileShader(fs);
    err = f->glGetError();
    if (debug && err!=0) qWarning() << "[GL] Compile fragment shader:" << err;
    //create program:
    shader_programme = f->glCreateProgram();
    f->glAttachShader(shader_programme, fs);
    f->glAttachShader(shader_programme, vs);
    f->glLinkProgram(shader_programme);
    f->glUseProgram(shader_programme);
    //after linking they aren't used anymore:
    f->glDeleteShader(vs);
    f->glDeleteShader(fs);
    err = f->glGetError();
    if (debug && err != 0) qWarning() << "[GL] Delete  shaders:" << err;

    *img = img->scaled( QSize( AUGMENTATION_WIDTH, AUGMENTATION_HEIGHT ), Qt::IgnoreAspectRatio, Qt::FastTransformation );
    image_width = img->width();
    image_height = img->height();
    qInfo() << "initial image size:" << img->depth() << "," << img->width() << "x" << img->height();
    //init and create textures:
    //create a texture for the image from Ibis:
    texLoc = f->glGetUniformLocation(shader_programme, "IbisImg");
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] UNIFORMLOCATION:" << err; }
    f->glUniform1i(texLoc, 0);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] UNIFORM1i:" << err; }
    f->glGenTextures(1, &augm_texture);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] GEN_TEXTURE:" << err; }
    f->glBindTexture(GL_TEXTURE_2D, augm_texture);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] BIND_TEXTURE:" << err; }
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] TEXPARAMETER:" << err; }
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] TEXPARAMETER:" << err; }

    *img = img->convertToFormat(QImage::Format_ARGB32);
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img->width(), img->height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img->bits());
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] TEXIMAGE2D:" << err; }
    f->glBindTexture(GL_TEXTURE_2D, 0);
    texLoc = f->glGetUniformLocation(shader_programme, "cameraImg");
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] UNIFORMLOCATION:" << err; }
    f->glUniform1i(texLoc, 1);
        if (debug){ err = f->glGetError(); if (err!=0) qWarning() << "[GL] UNIFORM1i:" << err; }
    f->glGenTextures(1, &camera_texture);
    f->glBindTexture(GL_TEXTURE_2D, camera_texture);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    f->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //make bogus image with the size of the actual image:
    QImage * bogus_image = new QImage( CAMERA_WIDTH, CAMERA_HEIGHT, QImage::Format_RGB888 );
    f->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, CAMERA_WIDTH, CAMERA_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, bogus_image->bits());
        if (debug){ err = f->glGetError(); if (err != 0) qWarning() << "[GL] TEXIMAGE2D:" << err;}
    f->glBindTexture(GL_TEXTURE_2D, 0);
        if (debug){ err = f->glGetError(); if (err != 0) qWarning() << "[GL] UNBIND_TEXTURE:" << err;}
    //debug shader compilation:
    if(debug){
        //debug shader compile output:
        GLint success;
        GLchar infoLog[512];
        f->glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
        if(!success){
            f->glGetShaderInfoLog(vs, 512, NULL, infoLog);
            qCritical() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog;
        }else{
            qInfo() << "[GlWidget][Debug] Vertex shader compilation success.";
        }
        f->glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
        if(!success){
            f->glGetShaderInfoLog(fs, 512, NULL, infoLog);
            qCritical() << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog;
        }else{
            qInfo() << "[GlWidget][Debug] Fragment shader compilation success.";
        }
        //debug shader linking output:
        f->glGetProgramiv(shader_programme, GL_LINK_STATUS, &success);
        if(!success) {
            f->glGetProgramInfoLog(shader_programme, 512, NULL, infoLog);
            qCritical() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog;
        }else{
            qInfo() << "[GlWidget][Debug] Shader program linking success.";
        }
    }

    //define UseTransparency:
    int bogus = f->glGetUniformLocation( shader_programme, "UseTransparency" );
    if( bogus != -1 ){
        f->glUniform1i( bogus, UseTransparency );
    }else{
        qWarning("UseTransparency couldn't be loaded.");
    }
    //define UseGradient
    bogus = f->glGetUniformLocation( shader_programme, "UseGradient" );
    if( bogus != -1 ){
        f->glUniform1i( bogus, UseGradient );
    }else{
        qWarning("UseGradient couldn't be loaded.");
    }
    //define show mask:
    bogus = f->glGetUniformLocation( shader_programme, "ShowMask" );
    if( bogus != -1 ){
        f->glUniform1i( bogus, ShowMask );
    }else{
        qWarning("ShowMask couldn't be loaded.");
    }
    //define TransparencyPosition:
    bogus = f->glGetUniformLocation( shader_programme, "TransparencyPosition" );
//    qInfo() << bogus4;
    err = f->glGetError();
    if (err != 0) qWarning() << "glGetUniformLocation error" << err;
    if( bogus != -1 ){
        f->glUniform2f( bogus, TransparencyPosition[0], TransparencyPosition[1] );
        if (debug){ err = f->glGetError(); if (err != 0) qInfo() << "glUniform2f error" << err; }
    }else{
        qWarning("[GlWidget][Warning] TransparencyPosition couldn't be loaded.");
    }
    //define TransparencyRadius:
    //((Window*)parent)->radiusChanged();
    bogus = f->glGetUniformLocation( shader_programme, "TransparencyRadius" );
    if( bogus != -1 ){
        f->glUniform2f( bogus, TransparencyRadius[0], TransparencyRadius[1] );
    }else{
        qWarning("[GlWidget][Warning] TransparencyRadius couldn't be loaded.");
    }
    //define resolution:
    bogus = f->glGetUniformLocation( shader_programme, "Resolution" );
    if( bogus != -1 ){
        f->glUniform2f( bogus, Resolution[0], Resolution[1] );
    }else{
        qWarning("[GlWidget][Warning] Resolution couldn't be loaded.");
    }
    bogus = f->glGetUniformLocation( shader_programme, "Screen_resolution" );
    if( bogus != -1 ){
        f->glUniform2f( bogus, Resolution[0], Resolution[1] );
    }else{
        qWarning("[GlWidget][Warning] Screen_resolution couldn't be loaded.");
    }
    bogus = f->glGetUniformLocation( shader_programme, "Camera_resolution" );
    if( bogus != -1 ){
        f->glUniform2f( bogus, CAMERA_WIDTH, CAMERA_HEIGHT );
    }else{
        qWarning("[GlWidget][Warning] Camera_resolution couldn't be loaded.");
    }
    //define aspect ratios:
    bogus = f->glGetUniformLocation( shader_programme, "aspect_ratio_orig_cam" );
    if( bogus != -1 ){
        f->glUniform1f( bogus, 1.*CAMERA_WIDTH/CAMERA_HEIGHT );
    }else{
        qWarning("[GlWidget][Warning] Aspect ratio cam couldn't be loaded.");
    }
    bogus = f->glGetUniformLocation( shader_programme, "aspect_ratio_screen" );
    if( bogus != -1 ){
        f->glUniform1f( bogus, 1.*AUGMENTATION_WIDTH/AUGMENTATION_HEIGHT );
    }else{
        qWarning("[GlWidget][Warning] Aspect ratio screen couldn't be loaded.");
    }
    //define trackerStatus:
    bogus = f->glGetUniformLocation( shader_programme, "TrackerOK" );
    if( bogus != -1 ){
        f->glUniform1i( bogus, true );
    }else{
        qWarning("[GlWidget][Warning] TrackerOK couldn't be loaded.");
    }
    //define GaussianFactor:
    bogus = f->glGetUniformLocation( shader_programme, "GaussianFactor" );
    if( bogus != -1 ){
        f->glUniform1f( bogus, GaussianFactor );
    }else{
        qWarning("[GlWidget][Warning] GaussianFactor couldn't be loaded.");
    }
    //define DisplayMode:
    bogus = f->glGetUniformLocation( shader_programme, "DisplayMode" );
    if( bogus != -1 ){
        f->glUniform1i( bogus, DisplayMode );
    }else{
        qWarning("[GlWidget][Warning] DisplayMode couldn't be loaded.");
    }
    //define AlphaChannelColour:
    bogus = f->glGetUniformLocation( shader_programme, "AlphaChannelColour" );
    if( bogus != -1 ){
        f->glUniform1i( bogus, AlphaChannelColour );
    }else{
        qWarning("[GlWidget][Warning] AlphaChannelColor couldn't be loaded.");
    }

    m_vao1->release();
    err = f->glGetError();
            if (debug && err!=0) qWarning() << "VAO_RELEASE:" << err;
    init = true;
    qDebug() << "[GlWidget][Debug] InitGL done.";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  RESIZE_GL   /////////////////////////////////////////////
void GLWidget::resizeGL( int x, int y ){
    assert( screen_width == x && screen_height == y);
    if( debug ){ qInfo() << "[GLWidget] GL screen resize function was called with params:" << x << "," << y; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////  SET_CAMERA_IMAGE_SIZE   ////////////////////////////////////////
void GLWidget::setCameraImageResolution( int w, int h ){
    camera_width = w;
    camera_height = h;
//    rgb = new uchar[ camera_width * camera_height * 3 ];
    argb = new uchar[ camera_width * camera_height * 4 ];
    if( debug ){ qInfo() << "[GLWidget] Camera image resolution set to:" << w << "," << h; }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  SET_CAMERA_PIXEL_FORMAT   ///////////////////////////////////////
void GLWidget::setPixelFormat( QVideoFrame::PixelFormat format ){
    pixel_format = format;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////  UPDATE_TRACKING_STATUS   ////////////////////////////////////////
void GLWidget::TrackingStatusChanged( bool status ){
    qInfo() << "[GLWidget] tracker status updated to:" << status;
    //change values:
    trackerOK = status;
    f->glUseProgram(shader_programme);
    //define TransparencyPosition:
    int bogus4 = f->glGetUniformLocation( shader_programme, "TrackerOK" );
    err = f->glGetError();
    if (err != 0) qInfo() << "glGetUniformLocation error" << err;
    if( bogus4 != -1 ){
        f->glUniform1i( bogus4, trackerOK );
        err = f->glGetError();
        if (err != 0) qInfo() << "glUniform1i error" << err;
    }else{
        qWarning("[GlWidget][Warning] TrackerOK couldn't be loaded.");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////  PAINT_GL   /////////////////////////////////////////////
void GLWidget::paintGL(){
    //draw:
    f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    f->glUseProgram(shader_programme);
    m_vao1->bind();
    f->glActiveTexture(GL_TEXTURE0);
    f->glBindTexture(GL_TEXTURE_2D, augm_texture);
    f->glUniform1i(f->glGetUniformLocation(shader_programme, "IbisImg"), 0);
    f->glActiveTexture(GL_TEXTURE1);
    f->glBindTexture(GL_TEXTURE_2D, camera_texture);
    f->glUniform1i(f->glGetUniformLocation(shader_programme, "cameraImg"), 1);
    f->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    f->glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    m_vao1->release();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////  READ_FILE   /////////////////////////////////////////////
#include <iostream>
 std::string GLWidget::readFile(const char * filePath) {
    std::string content = "";
    QFile mFile(filePath);
    if(!mFile.open(QFile::ReadOnly | QFile::Text)){
        qCritical() << "[GlWidget][Critical] Could not open file " << filePath;
        return "";
    }
    QTextStream in(&mFile);
    std::string line = "";
    while(!in.atEnd()){
        line = in.readLine().toUtf8().constData();
        content.append(line + "\n");
    }
    mFile.close();
    return content;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////  MOVE_TRANSPARENCY   /////////////////////////////////////////
void GLWidget::moveCursor(int x, int y){
    if(DisplayMode==DisplayMode::AugmentedReality){
        //change values:
        TransparencyPosition[0] = (float)x;
        TransparencyPosition[1] = (float)y;
        f->glUseProgram( shader_programme );
        //define TransparencyPosition:
        int bogus4 = f->glGetUniformLocation( shader_programme, "TransparencyPosition" );
        err = f->glGetError();
        if( err != 0 ) qInfo() << "glGetUniformLocation error" << err;
        if( bogus4 != -1 ){
            f->glUniform2fv( bogus4, 1, TransparencyPosition );
            err = f->glGetError();
            if (err != 0) qInfo() << "glUniform2f error" << err;
        }else{
            qWarning("[GLWidget::moveCursor][Warning] TransparencyPosition couldn't be loaded.");
        }
    }else if(DisplayMode==DisplayMode::AugmentationOnly){
        //we convert screen position to normalized coordinates for each frame then send the command:
        double x_norm = -99.;
        double y_norm = -99.;
        if( x < screen_width/2 && y < screen_height/2 ){            //top-left view (transverse?)
            x_norm = 1. * x / (screen_width/2.);
            y_norm = 1. * y / (screen_height/2.);
            qInfo() << "[GLWidget::moveCursor][DEBUG] Moving transverse view (" << x_norm << ", " << y_norm << ")";
            window->navigateSlice( SliceView::Transverse, x_norm, y_norm );
        }else if( x < screen_width/2 && y >= screen_height/2 ){     //bottom-left view (coronal?)
            x_norm = 1. * x / (screen_width/2.);
            y_norm = 1. * ( y - screen_height/2. ) / (screen_height/2.);
            qInfo() << "[GLWidget::moveCursor][DEBUG] Moving coronal view (" << x_norm << ", " << y_norm << ")";
            window->navigateSlice( SliceView::Coronal, x_norm, y_norm );
        }else if( x >= screen_width/2 && y >= screen_height/2 ){    //bottom-right view (sagittal)
            x_norm = ( 1. * x - screen_width/2. ) / (screen_width/2.);
            y_norm = ( 1. * y - screen_height/2. ) / (screen_height/2.);
            qInfo() << "[GLWidget::moveCursor][DEBUG] Moving sagittal view (" << x_norm << ", " << y_norm << ")";
            window->navigateSlice( SliceView::Sagittal, x_norm, y_norm );
        }else{
            qInfo() << "[GLWidget::moveCursor][DEBUG] Nothing to move here.";
        }
    }else if(DisplayMode==DisplayMode::CameraOnly){
        //TODO
    }else{
        qWarning() << "[GLWidget::moveCursor] DisplayMode doesn't exist.";
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////"//////  SET_DISPLAY_MODE   /////////////////////////////////////////
void GLWidget::setDisplayMode( int mode ){
    //change values:
    DisplayMode = mode;
    f->glUseProgram(shader_programme);
    int bogus = f->glGetUniformLocation( shader_programme, "DisplayMode" );
    err = f->glGetError();
    if (err != 0) qInfo() << "glGetUniformLocation error" << err;
    if( bogus != -1 ){
        f->glUniform1i( bogus, DisplayMode );
        err = f->glGetError();
        if (err != 0) qInfo() << "glUniform2i error" << err;
    }else{
        qWarning() << "[GlWidget][Warning] DisplayMode couldn't be loaded.";
    }
}

#include <cmath>
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////  CHANGE_GAUSSIAN_FACTOR   /////////////////////////////////////////
void GLWidget::changeGaussian( float val ){
    GaussianFactor = val;
    f->glUseProgram(shader_programme);
    int bogus = f->glGetUniformLocation( shader_programme, "GaussianFactor" );
    err = f->glGetError();
    if (err != 0) qInfo() << "glGetUniformLocation error" << err;
    if( bogus != -1 ){
        f->glUniform1f( bogus, GaussianFactor );
        err = f->glGetError();
        if (err != 0) qInfo() << "glUniform1f error" << err;
    }else{
        qWarning() << "[GlWidget][Warning] GaussianFactor couldn't be loaded.";
    }
}

void GLWidget::changeRadius( float inner, float outer ){
    TransparencyRadius[0] = inner;
    TransparencyRadius[1] = outer;
    f->glUseProgram(shader_programme);
    int bogus = f->glGetUniformLocation( shader_programme, "TransparencyRadius" );
    err = f->glGetError();
    if (err != 0) qInfo() << "glGetUniformLocation error" << err;
    if( bogus != -1 ){
        f->glUniform2f( bogus, TransparencyRadius[0], TransparencyRadius[1] );
        err = f->glGetError();
        if (err != 0) qInfo() << "glUniform2f error" << err;
    }else{
        qWarning() << "[GlWidget][Warning] TransparencyRadius couldn't be loaded.";
    }
    //TODO
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////  GESTURES ///////////////////////////////////////////

bool GLWidget::event(QEvent *event){
    if(event->type() == QEvent::Gesture){
        return gestureEvent(static_cast<QGestureEvent *>(event));
    }
    return QWidget::event(event);
}

void GLWidget::pinch(){
    //when in reregister mode, pinching is used to rotate the target
    if( reregister && rotate ){
        window->reregisterAR( 0, 0, rotationAngleDelta );
        qInfo() << "[GLWidget] Sending ReregisterAR command with rotation by: " << rotationAngleDelta;
    //otherwise, it's used to change the size of the AR transparency window
    }else if( reregister && !rotate ){
        //do nothing
    }else if( rotateView ){
        //TODO: test
        window->rotateView( 0, 0, currentStepScaleFactor );
    }else{
        //hard limits on downsizing and upsizing:
        if ( ( TransparencyRadius[1] < RESIZE_LIMIT && currentStepScaleFactor < 1 )
            || ( TransparencyRadius[1] > Resolution[0]*2 && currentStepScaleFactor > 1 ) ){
            //do nothing
        }else{
            changeRadius( static_cast<float>( RATIO_INNER_OUTER_AR_WINDOW * TransparencyRadius[1]), static_cast<float>( currentStepScaleFactor * TransparencyRadius[1]) );
            qInfo() << "[GLWidget] Rescaling AR transparency radius: " << currentStepScaleFactor << " (outer: " << TransparencyRadius[1] << ")";
        }
    }
}

void GLWidget::pan() const{
    if( reregister && translate ){
        window->reregisterAR( horizontalOffsetDelta, verticalOffsetDelta, 0. );
        qInfo() << "[GLWidget] Sending ReregisterAR command with translation by: (" << horizontalOffsetDelta << ", " << verticalOffsetDelta << ")";
    }else if( rotateView ){
        window->rotateView( horizontalOffsetDelta, verticalOffsetDelta, 0. );
        qInfo() << "[GLWidget] Sending RotateView command with parameters: (" << horizontalOffsetDelta << ", " << verticalOffsetDelta << ")";
    }else{
        qDebug() << "Nothing to pan here.";
    }
}

void GLWidget::swipePrevious() const{
    //TODO
}

void GLWidget::swipeNext() const{
    //TODO
}
