#include <QQuickWidget>
#include <QQmlEngine>
#include "qmlmainwindow.h"
#include "glwidget.h"

QmlMainWindow::QmlMainWindow(){
    engine = new QQmlEngine(this);
    paint();
}

QmlMainWindow::QmlMainWindow( int width, int height ){
    point_set = std::vector<std::pair<int, int>>();
    screen_width = width;
    screen_height = height;
    setFixedSize( screen_width, screen_height );
    GL_widget = new GLWidget( this, screen_width, screen_height );
    engine = new QQmlEngine(this);
    paint();
}

void QmlMainWindow::paint(){
    cameraButtonView = new QQuickWidget(engine, this);
    cameraButtonView->setSource(QUrl("qrc:/qml/CameraButton.qml"));
    cameraButtonView->setProperty("visible", false);
    cameraButtonView->setAttribute(Qt::WA_TranslucentBackground, true);
    cameraButtonView->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    cameraButtonView->setClearColor(Qt::transparent);
    cameraButtonView->rootObject()->setProperty("x", getWidth()-70);
    cameraButtonView->rootObject()->setProperty("y", getHeight()/2-70);
    cameraButtonView->rootObject()->setProperty("width", getWidth() + 10);
    cameraButtonView->rootObject()->setProperty("height", getHeight() + 10);

    QObject *cameraButtonObject = cameraButtonView->rootObject();

    profileInfoView = new QQuickWidget(engine, this);
    profileInfoView->setSource(QUrl("qrc:/qml/ProfileInfo.qml"));
    profileInfoView->setAttribute(Qt::WA_TranslucentBackground, true);
    profileInfoView->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    profileInfoView->setClearColor(Qt::transparent);
    profileInfoView->setGeometry(105, 30, 200, 38);
    profileInfoView->rootObject()->setProperty("patientIdText","Patient ID");
    profileInfoView->rootObject()->setProperty("patientInfoText","01/01/1960");

    filtersView = new QQuickWidget(engine, this);
    filtersView->setSource(QUrl("qrc:/qml/FiltersPanel.qml"));
    filtersView->setProperty("visible", "false");
    filtersView->setAttribute(Qt::WA_TranslucentBackground, true);
    filtersView->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    filtersView->setClearColor(Qt::transparent);
    filtersView->rootObject()->setProperty("x", 530);
    filtersView->rootObject()->setProperty("y", 25);
    filtersView->rootObject()->setProperty("width", getWidth() + 10);

    QObject *filtersObject = filtersView->rootObject();
    QObject::connect(filtersObject, SIGNAL(enableFilter(int)),
                    this, SLOT(enableFilter(int)));
    QObject::connect(filtersObject, SIGNAL(disableFilter(int)),
                    this, SLOT(disableFilter(int)));
    
    reregisterView = new QQuickWidget(engine, this);
    reregisterView->setSource(QUrl("qrc:/qml/ReregisterPanel.qml"));
    reregisterView->setProperty("visible", "false");
    reregisterView->setAttribute(Qt::WA_TranslucentBackground, true);
    reregisterView->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    reregisterView->setClearColor(Qt::transparent);
    reregisterView->rootObject()->setProperty("x", 530);
    reregisterView->rootObject()->setProperty("y", 25);
    reregisterView->rootObject()->setProperty("width", getWidth() + 10);

    QObject *reregisterObject = reregisterView->rootObject();
    QObject::connect(reregisterObject, SIGNAL(enableMode(int)),
                     this, SLOT(enableMode(int)));
    QObject::connect(reregisterObject, SIGNAL(disableMode(int)),
                     this, SLOT(disableMode(int)));

    settingsPanelView = new QQuickWidget(engine, this);
    settingsPanelView->setSource(QUrl("qrc:/qml/SettingsPanel.qml"));
    settingsPanelView->setProperty("visible", false);
    settingsPanelView->setAttribute(Qt::WA_TranslucentBackground, true);
    settingsPanelView->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    settingsPanelView->setClearColor(Qt::transparent);
    settingsPanelView->rootObject()->setProperty("x", 79);
    settingsPanelView->rootObject()->setProperty("width", getWidth());
    settingsPanelView->rootObject()->setProperty("height", getHeight());

    QObject *settingsPanelObject = settingsPanelView->rootObject();
    QObject::connect(settingsPanelObject, SIGNAL(triggerAction(int)),
                    this, SLOT(executeAction(int)));

    toolBarView = new QQuickWidget(engine, this);
    toolBarView->setSource(QUrl("qrc:/qml/ToolsBar.qml"));
    toolBarView->setAttribute(Qt::WA_TranslucentBackground, true);
    toolBarView->setAttribute(Qt::WA_AlwaysStackOnTop, true);
    toolBarView->setClearColor(Qt::transparent);

    QObject *toolBarObject = toolBarView->rootObject();
    toolBarObject->setProperty("height", getHeight());

    QObject::connect(toolBarObject, SIGNAL(triggerAction(int)), this, SLOT(executeAction(int)));

   serverConnectionView = new QQuickWidget(engine, this);
   serverConnectionView->setSource(QUrl("qrc:/qml/ServerConnection.qml"));
   serverConnectionView->setProperty("visible", true);
   serverConnectionView->setAttribute(Qt::WA_TranslucentBackground, true);
   serverConnectionView->setAttribute(Qt::WA_AlwaysStackOnTop, true);
   serverConnectionView->setClearColor(Qt::transparent);
   serverConnectionView->rootObject()->setProperty("width", getWidth());
   serverConnectionView->rootObject()->setProperty("height", getHeight());

   QObject *ServerDialogObject = serverConnectionView->rootObject();

   QObject::connect(ServerDialogObject, SIGNAL(closeMessage(int)),
                    this, SLOT(executeAction(int)));
}

void QmlMainWindow::executeAction(const int &action){
    qDebug() <<  "executeAction:" << action;

    switch(action){
    case 1:
        executeOpenSettingsPanel();
        break;
    case 2:
        executeCloseSettingsPanel();
        break;
    case 3:
        executeOpenAnatomyPlanes();
        break;
    case 4:
        executeCloseAnatomyPlanes();
        break;
    case 5:
        executeShowFilters();
        break;
    case 6:
        executeHideFilters();
        break;
    case 7:
        executeStartReregister();
        break;
    case 8:
        executeStopReregister();
        break;
    case 9:
        executeSetDisplayMode( DisplayMode::AugmentationOnly );
        break;
    case 10:
        executeSetDisplayMode( DisplayMode::CameraOnly );
        break;
    case 11:
        executeShowServerConnectionError();
        break;
    case 12:
        executeHideServerConnectionError();
        break;
    case 13:
        executeExit();
        break;
    case 14:
        executeStartFreeze();
        break;
    case 15:
        executeStopFreeze();
        break;
    case 16:
        executeStartSender();
        break;
    case 17:
        executeStopSender();
        break;
    case 18:
        executeStartReceiver();
        break;
    case 19:
        executeStopReceiver();
        break;
    case 20:
        executeSetDisplayMode( DisplayMode::AugmentedReality );
        break;
    case 21:
        executeStartPhotoRegistration();
        break;
    case 22:
        executeStopPhotoRegistration();
        break;
    case 23:
        executeStartPickPoints();
        break;
    case 24:
        executeStopPickPoints();
        break;
    default:
        qCritical() << "[QmlMainWindow::executeAction] Command does not exist.";
        break;
    }
}

void QmlMainWindow::executeOpenSettingsPanel(){
    qDebug() <<  "Debug: executeOpenSettingsPanel ";
    settingsPanelView->setProperty("visible", "true");
    setMoveView( false );
}

void QmlMainWindow::executeCloseSettingsPanel(){
    qDebug() <<  "Debug: executeCloseSettingsPanel ";
    settingsPanelView->setProperty("visible", "false");
    setMoveView( true );
}

void QmlMainWindow::executeOpenAnatomyPlanes(){
    qDebug() <<  "Debug: executeOpenAnatomyPlanes ";
    emit setQuadViewSignal( true );
    getWidget()->setDisplayMode( DisplayMode::AugmentationOnly );
}

void QmlMainWindow::executeCloseAnatomyPlanes(){
    qDebug() <<  "Debug: executeCloseAnatomyPlanes ";
    emit setQuadViewSignal( false );
    getWidget()->setDisplayMode( DisplayMode::AugmentedReality );
}

void QmlMainWindow::executeShowFilters(){
    qDebug() <<  "Debug: executeShowFilters ";
    filtersView->setProperty("visible", "true");
}

void QmlMainWindow::executeHideFilters(){
    qDebug() <<  "Debug: executeHideFilters ";
    filtersView->setProperty("visible", "false");
}

void QmlMainWindow::executeShowReregisterOptions(){
    qDebug() <<  "Debug: executeShowReregisterOptions ";
    reregisterView->setProperty("visible", "true");
}

void QmlMainWindow::executeHideReregisterOptions(){
    qDebug() <<  "Debug: executeHideReregisterOptions ";
    reregisterView->setProperty("visible", "false");
}

void QmlMainWindow::executeSetDisplayMode( int mode ){
    qDebug() <<  "Debug: executeSetRenderMode";
    getWidget()->setDisplayMode( mode );
}

void QmlMainWindow::executeShowServerConnectionError(){
    qDebug() <<  "Debug: executeShowServerConnectionError ";
    serverConnectionView->setProperty("visible", false);
}

void QmlMainWindow::executeHideServerConnectionError(){
    qDebug() <<  "Debug: executeShowServerConnectionError ";
    serverConnectionView->setProperty("visible", false);
}

void QmlMainWindow::enableFilter(const int &filter){
    qDebug() <<  "enableFilter: " << filter;
    switch(filter){
        case 1:
            enableTumourFilter();
            break;
        case 2:
            enableSkinFilter();
            break;
        case 3:
            enableVesselFilter();
            break;
    }
}

void QmlMainWindow::enableMode(const int &mode){
    qDebug() <<  "enableMode: " << mode;
    switch(mode){
        case 1:
            enableRotateMode();
            break;
        case 2:
            enableTranslateMode();
            break;
        case 3:
            resetReregistration();
            break;
    }
}

void QmlMainWindow::enableTumourFilter(){
    emit anatomyToggledSignal(1, true);
    qDebug() <<  "enableTumourFilter: ";
}

void QmlMainWindow::enableSkinFilter(){
    emit anatomyToggledSignal(2, true);
    qDebug() <<  "enableSkinFilter: ";
}

void QmlMainWindow::enableVesselFilter(){
    emit anatomyToggledSignal(3, true);
    qDebug() <<  "enableVesselFilter";
}

void QmlMainWindow::disableTumourFilter(){
    emit anatomyToggledSignal(1, false);
    qDebug() <<  "disableTumourFilter";
}

void QmlMainWindow::disableSkinFilter(){
    emit anatomyToggledSignal(2, false);
    qDebug() <<  "disableSkinFilter";
}

void QmlMainWindow::disableVesselFilter(){
    emit anatomyToggledSignal(3, false);
    qDebug() <<  "disableVesselFilter";
}

void QmlMainWindow::enableRotateMode(){
    getWidget()->setRotate( true );
    qDebug() <<  "enableRotate: ";
}

void QmlMainWindow::enableTranslateMode(){
    getWidget()->setTranslate( true );
    qDebug() <<  "enableTranslate: ";
}

void QmlMainWindow::disableRotateMode(){
    getWidget()->setRotate( false );
    qDebug() <<  "disableRotate";
}

void QmlMainWindow::disableTranslateMode(){
    getWidget()->setTranslate( false );
    qDebug() <<  "disableTranslate";
}

void QmlMainWindow::resetReregistration(){
    emit resetReregistrationSignal();
    qDebug() <<  "resetReregistration";
}

void QmlMainWindow::disableFilter(const int &filter){
    qDebug() <<  "disableFilter: " << filter;

    switch(filter){
    case 1:
        disableTumourFilter();
        break;
    case 2:
        disableSkinFilter();
        break;
    case 3:
        disableVesselFilter();
        break;
    }
}

void QmlMainWindow::disableMode( const int &mode ){
    qDebug() <<  "disableMode: " << mode;
    
    switch(mode){
        case 1:
            disableRotateMode();
            break;
        case 2:
            disableTranslateMode();
            break;
        case 3:
            resetReregistration();
            break;
    }
}

void QmlMainWindow::executeStartReregister(){
    getWidget()->setReregister( true );
    getWidget()->setRotate( true );
    getWidget()->setTranslate( true );
    setMoveView( false );
    executeShowReregisterOptions();
    qDebug() <<  "Debug: executeStartReregister";
}

void QmlMainWindow::executeStopReregister(){
    getWidget()->setReregister( false );
    getWidget()->setRotate( false );
    getWidget()->setTranslate( false );
    setMoveView( true );
    executeHideReregisterOptions();
    qDebug() <<  "Debug: executeStopReregister";
}

void QmlMainWindow::executeStartFreeze(){
    //TODO: test
    getWidget()->setRotateView( true );
    getWidget()->setDisplayMode( DisplayMode::AugmentationOnly );
    emit freezeFrameSignal( true );
    qDebug() <<  "Debug: executeStartFreeze";
}

void QmlMainWindow::executeStopFreeze(){
    //TODO: test
    getWidget()->setRotateView( false );
    getWidget()->setDisplayMode( DisplayMode::AugmentedReality );
    emit freezeFrameSignal( false );
    qDebug() <<  "Debug: executeStopFreeze";
}

void QmlMainWindow::executeStartSender(){
    //TODO: test
    emit toggleSenderSignal( true );
    qDebug() <<  "Debug: executeStartSender";
}

void QmlMainWindow::executeStopSender(){
    //TODO: test
    emit toggleSenderSignal( false );
    qDebug() <<  "Debug: executeStopSender";
}

void QmlMainWindow::executeStartReceiver(){
    //TODO: test
    emit toggleReceiverSignal( true );
    qDebug() <<  "Debug: executeStartReceiver";
}

void QmlMainWindow::executeStopReceiver(){
    //TODO: test
    emit toggleReceiverSignal( false );
    qDebug() <<  "Debug: executeStopReceiver";
}

void QmlMainWindow::executeStartPhotoRegistration(){
    qDebug() <<  "Debug: executeStartPhotoRegistration";
    cameraButtonView->setProperty("visible", "true");
    //TODO
}

void QmlMainWindow::executeStopPhotoRegistration(){
    qDebug() <<  "Debug: executeStartPhotoRegistration";
    cameraButtonView->setProperty("visible", "false");
    //TODO
}

//TODO: rework
void QmlMainWindow::executeStartPickPoints(){
    //TODO: test
    qDebug() <<  "Debug: executeStartPickPoints";
    picking_points = true;
    point_set = std::vector<std::pair<int, int>>();
}

void QmlMainWindow::executeStopPickPoints(){
    //TODO: test
    qDebug() <<  "Debug: executeStopPickPoints";
    picking_points = false;
    emit sendPointSetSignal( point_set );
}

void QmlMainWindow::executeExit(){
    emit exitSignal();
}

void QmlMainWindow::setReceiver( Receiver * r ){
    GL_widget->setReceiver( r );
}
void QmlMainWindow::setCameraImageResolution( int w, int h ){
    GL_widget->setCameraImageResolution( w, h );
}
void QmlMainWindow::setPixelFormat( QVideoFrame::PixelFormat format ){
    GL_widget->setPixelFormat( format );
}

void QmlMainWindow::new_camera_frame( QVideoFrame qvf ){
    GL_widget->set_camera_image( &qvf );
}

void QmlMainWindow::update_widget(){
    GL_widget->animate();
}

void QmlMainWindow::setMoveView( bool b ){
    if( b && !(getWidget()->getRotate()) && !(getWidget()->getReregister()) ){
        move_view_on = true;
    }else{
        move_view_on = false;
    }
}

GLWidget * QmlMainWindow::getWidget(){
    return GL_widget;
}

QmlMainWindow::~QmlMainWindow(){
    delete engine;
    delete toolBarView;
    delete profileInfoView;
    delete settingsPanelView;
    delete filtersView;
    delete serverConnectionView;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// MOUSE EVENTS ////////////////////////////////////////////////
void QmlMainWindow::mousePressEvent(QMouseEvent * event){
    int x = event->x();
    int y = event->y();
    if( move_view_on ){
        GL_widget->moveCursor( x, y );
    }
    if( picking_points ){
        point_set.push_back( std::pair<int,int>(x,y) );
    }
}
void QmlMainWindow::mouseMoveEvent(QMouseEvent * event){
    int x = event->x();
    int y = event->y();
    if( move_view_on ){
        GL_widget->moveCursor( x, y );
    }
}
