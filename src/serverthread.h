#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "common.h"
#include "server.h"
#include <QThread>
#include <QHostAddress>

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(QObject *parent = 0);
    virtual ~ServerThread();
    virtual void run();
    void setPort(quint16 port);
    void setAddress(QHostAddress address);

private:
    Server * server;
    quint16 m_port;
    QHostAddress m_address;

signals:

public slots:

};

#endif // SERVERTHREAD_H
