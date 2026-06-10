#include "camera.h"

#include <QCamera>
#include <QDebug>

Camera::Camera(){
}

QCamera * Camera::getCamera(){
    return camera;
}

void Camera::setCamera(QCamera* cam){
    camera = cam;
}

void Camera::freezeCamera(bool freeze){
    qDebug() << "freezeCamera: " << freeze;
    if(freeze){
        camera->setFocusMode(QCamera::FocusModeManual);
        //TODO setFocusDistance( dst );
    } else{
        camera->setFocusMode(QCamera::FocusModeAutoNear);
    }
}

void Camera::ChangeFocusMode(){
    qInfo() << "TODO";
    //TODO
}
