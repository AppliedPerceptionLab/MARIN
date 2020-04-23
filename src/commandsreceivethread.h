#ifndef COMMANDSRECEIVETHREAD_H
#define COMMANDSRECEIVETHREAD_H

#pragma once

#include <QObject>
#include <QThread>

#include "receiver.h"

class CommandsReceiveThread : public QThread{
    Q_OBJECT
public:
    explicit CommandsReceiveThread(QObject *parent = nullptr );
    void setReceiver(Receiver * receiver);
    void run() Q_DECL_OVERRIDE;

signals:

private:
    Receiver * receiver;

public slots:
    void end();

};

#endif
