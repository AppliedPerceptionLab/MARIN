#include "camera.h"

Camera::Camera(){
}

QCamera * Camera::getCamera(){
    return camera;
}

void Camera::setCamera(QCamera* cam){
    camera = cam;
    FocusObj = cam->focus();
}

void Camera::freezeCamera(bool freeze){
    qDebug() << "freezeCamera: " << freeze;
    if(freeze){
        camera->searchAndLock();
    } else{
        camera->unlock();
    }
}

void Camera::ChangeFocusMode(){
    qInfo() << "TODO";
    //TODO
}
