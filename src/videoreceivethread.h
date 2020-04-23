#ifndef VIDEORECEIVETHREAD_H
#define VIDEORECEIVETHREAD_H

#pragma once

#include <QObject>
#include <QThread>
#include "receiver.h"

class VideoReceiveThread : public QThread{
    Q_OBJECT
public:
    explicit VideoReceiveThread(QObject *parent = nullptr );
    void setReceiver(Receiver * receiver);
    void run() Q_DECL_OVERRIDE;

signals:

private:
    Receiver * s;

public slots:
    void end();
};

#endif
