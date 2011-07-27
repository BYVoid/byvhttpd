#include "serverthread.h"
#include "log.h"
#include <QMutex>

extern QMutex mutex_log;

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
        mutex_log.lock();
        Log::instance() << "failed to listen port " << m_port << '\n';
        mutex_log.unlock();
        quit();
    }
    mutex_log.lock();
    Log::instance() << "listening port " << m_port << "\n";
    mutex_log.unlock();
    exec();
}

void ServerThread::setPort(quint16 port)
{
    m_port = port;
}
