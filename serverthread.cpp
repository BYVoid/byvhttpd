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
        Log::instance() << "Failed to listen port " << m_port << Log::NEWLINE << Log::FLUSH;
        quit();
    }
    Log::instance() << "Listening port " << m_port << Log::NEWLINE << Log::FLUSH;
    exec();
}

void ServerThread::setPort(quint16 port)
{
    m_port = port;
}
