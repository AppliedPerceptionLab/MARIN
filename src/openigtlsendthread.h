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

private:
    Sender * s;

public slots:
    void end();
};
#endif
