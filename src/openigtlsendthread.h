#ifndef OPENIGTLSENDTHREAD_H
#define OPENIGTLSENDTHREAD_H

#pragma once
#include <QThread>
#include <QObject>
#include "sender.h"

class OpenIGTLsendThread : public QThread{
    Q_OBJECT
public:
    explicit OpenIGTLsendThread(QObject *parent = nullptr );
    void setSender(Sender * sender);
    void run() Q_DECL_OVERRIDE;
    void stop();
    void restart();
    void anatomyToggled( int i, bool b ){ s->toggleAnatomy( i, b ); }
    void quadviewToggled( bool b ){ s->toggleQuadView( b ); }
    void navigateSlice( SliceView sv, double x, double y ){ s->navigateSlice( sv, x, y ); }
    void reregisterAR( double dx, double dy, double a ){ s->reregisterAR( dx, dy, a ); }
    void rotateView( double dx, double dy, double z ){ s->rotateView( dx, dy, z ); }
    void freezeFrame( bool b ){ s->freezeFrame( b ); }
    void resetReregistration( ){ s->resetReregistration( ); }
    void sendPointSet( std::vector<std::pair<int,int>> points ){ s->sendPointSet( points ); }

private:
    Sender * s;

public slots:
    void end();
};
#endif
