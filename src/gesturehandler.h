// Created by Jonatan Reyes.

#ifndef gestureHandler_h
#define gestureHandler_h

#include <QGestureEvent>

class QWidget;
class QSwipeGesture;
class QPinchGesture;

class GestureHandler{
        
public:
    GestureHandler();
    GestureHandler(QWidget * widget);
    void registerGestures(QList<Qt::GestureType> gestures);

    virtual void swipePrevious() const;
    virtual void swipeNext() const;
    virtual void pinch();
    virtual void pan() const;

    bool gestureEvent(QGestureEvent *event);
    
protected:
    double scaleFactor;
    double currentStepScaleFactor;
    QWidget * widget;
    
    double rotationAngleDelta;
    double rotationAngle;
    double horizontalOffsetDelta;
    double verticalOffsetDelta;
    double horizontalOffset;
    double verticalOffset;

private:
    void swipeTriggered(QSwipeGesture *gesture);
    void pinchTriggered(QPinchGesture *gesture);
    void panTriggered(QPanGesture *gesture);
    
};

#endif
