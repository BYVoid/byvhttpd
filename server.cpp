#include "request.h"
#include "server.h"

Server::Server(QObject *parent) :
    QTcpServer(parent)
{
}

bool Server::start(quint16 port)
{
    return listen(QHostAddress::Any, port);
}

void Server::incomingConnection(int socketDescriptor)
{
    Request * task = new Request(socketDescriptor);
    task->start();
}
