#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include <QTcpServer>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = 0);
    bool start(QHostAddress address, quint16 port);

protected:
    void incomingConnection(int socketDescriptor);

};

#endif // SERVER_H
