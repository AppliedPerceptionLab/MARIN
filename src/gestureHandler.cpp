//  Created by Jonatan Reyes.

#include "gestureHandler.h"
#include <QDebug>
#include <QSwipeGesture>
#include <QPinchGesture>
#include <QWidget>

GestureHandler::GestureHandler(){

}

GestureHandler::GestureHandler(QWidget * widget){
    this->widget = widget;

    rotationAngle = 0;
    scaleFactor = 1;
    currentStepScaleFactor = 1;
    horizontalOffset = 0;
    verticalOffset = 0;
}

void GestureHandler::registerGestures(QList<Qt::GestureType> gestures){
    foreach (Qt::GestureType gesture, gestures) {
        widget->grabGesture(gesture);
    }
}

bool GestureHandler::gestureEvent(QGestureEvent *event){
    qDebug() << "gestureEvent(): " << event->gestures().size();
    if(QGesture *swipe = event->gesture(Qt::SwipeGesture)){
        swipeTriggered(static_cast<QSwipeGesture *>(swipe));
    } else if (QGesture *pan = event->gesture(Qt::PanGesture)){
        panTriggered(static_cast<QPanGesture *>(pan));
    }
    if(QGesture *pinch = event->gesture(Qt::PinchGesture)){
        pinchTriggered(static_cast<QPinchGesture *>(pinch));
    }
    return true;
}

void GestureHandler::swipeTriggered(QSwipeGesture *gesture){
    if(gesture->state() == Qt::GestureFinished){
        if(gesture->horizontalDirection() == QSwipeGesture::Left ||
           gesture->verticalDirection() == QSwipeGesture::Up){
            swipePrevious();
            qDebug() << "swipeTriggered(): swipe to previous";
        }else{
            swipeNext();
            qDebug() << "swipeTriggered(): swipe to next";
        }
        widget->update();
    }
}

void GestureHandler::pinchTriggered(QPinchGesture *gesture){
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();

    if( changeFlags & QPinchGesture::RotationAngleChanged ){
        const qreal value = gesture->property("rotationAngle").toReal();
        const qreal lastValue = gesture->property("lastRotationAngle").toReal();
        rotationAngleDelta = value - lastValue;
        rotationAngle += rotationAngleDelta;
        qDebug() << "pinchTriggered(): rotation by" << rotationAngleDelta << rotationAngle;
    }
    if ( changeFlags & QPinchGesture::ScaleFactorChanged ) {
        qreal value = gesture->property("scaleFactor").toReal();
        currentStepScaleFactor = value;
        
        qDebug() << "pinchTriggered(): scaled by" << currentStepScaleFactor;
    }
    if (gesture->state() == Qt::GestureFinished) {
        scaleFactor *= currentStepScaleFactor;
//        scaleFactor = currentStepScaleFactor;
        currentStepScaleFactor = 1;
    }
    pinch();
    widget->update();
}

void GestureHandler::panTriggered(QPanGesture *gesture){
#ifndef QT_NO_CURSOR
    switch (gesture->state()) {
        case Qt::GestureStarted:
        case Qt::GestureUpdated:
            widget->setCursor(Qt::SizeAllCursor);
            break;
        default:
            widget->setCursor(Qt::ArrowCursor);
    }
#endif
    QPointF delta = gesture->delta();
    qDebug() << "panTriggered():" << gesture;
    horizontalOffset += delta.x();
    verticalOffset += delta.y();
    horizontalOffsetDelta = delta.x();
    verticalOffsetDelta = delta.y();
    pan();
    widget->update();
}

void GestureHandler::pinch(){
    // No default behavior specified.
    // Waiting for the the consumer class to define a pinch action.
}

void GestureHandler::pan() const{
    // No default behavior specified.
    // Waiting for the the consumer class to define a pan action.
}

void GestureHandler::swipePrevious() const{
    // No default behavior specified.
    // Waiting for the the consumer class to define a swipePrevious action.
}

void GestureHandler::swipeNext() const{
    // No default behavior specified.
    // Waiting for the the consumer class to define a swipeNext action.
}
