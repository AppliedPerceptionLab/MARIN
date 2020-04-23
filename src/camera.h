#ifndef CAMERA_H
#define CAMERA_H

#include <QCamera>
#include <QCameraImageCapture>

class Camera: public QObject{
    Q_OBJECT

public:
    Camera();
    QCamera * getCamera();
    void setCamera(QCamera * cam);

public slots:
    void freezeCamera(bool freeze);
    void ChangeFocusMode();

private:
    QCamera * camera;
    QCameraFocus * FocusObj;
};

#endif
