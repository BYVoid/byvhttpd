#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "server.h"
#include <QThread>

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(QObject *parent = 0);
    virtual ~ServerThread();
    virtual void run();
    void setPort(quint16 port);

private:
    Server * server;
    quint16 m_port;

signals:

public slots:

};

#endif // SERVERTHREAD_H
