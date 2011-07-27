#include "serverthread.h"
#include "log.h"

ServerThread::ServerThread(QObject *parent) :
    QThread(parent),
    m_port(8000)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(terminated()), this, SLOT(deleteLater()));
}

ServerThread::~ServerThread()
{
    delete server;
}

void ServerThread::run()
{
    server = new Server();
    if (!server->start(m_port))
    {
        Log::instance() << "failed to listen port " << m_port << '\n';
        quit();
    }
    Log::instance() << "listening port " << m_port << "\n";
    exec();
}

void ServerThread::setPort(quint16 port)
{
    m_port = port;
}
