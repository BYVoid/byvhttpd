#include "request.h"
#include "server.h"

Server::Server(QObject *parent) :
    QTcpServer(parent)
{
}

bool Server::start(QHostAddress address, quint16 port)
{
    return listen(address, port);
}

void Server::incomingConnection(int socketDescriptor)
{
    Request * task = new Request(socketDescriptor);
    task->start();
}
