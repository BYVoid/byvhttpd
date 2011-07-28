#include "common.h"
#include "serverthread.h"
#include "request.h"
#include "log.h"
#include "settings.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <QVariantList>

void startInstance(quint16 port)
{
    ServerThread * server_thread = new ServerThread();
    server_thread->setPort(port);
    server_thread->start();
}

bool start()
{
    QVariantList port_list;
    bool ok;
    QVariant t_port = Settings::instance().value("httpd/port").toUInt(&ok);
    if (ok)
        port_list.push_back(t_port);
    else
        port_list = t_port.toList();

    if (port_list.size() == 0)
    {
        Log::instance() << "No port specified to listen." << Log::NEWLINE << Log::FLUSH;
        return false;
    }

    for (QVariantList::iterator i = port_list.begin(); i != port_list.end(); ++i)
    {
        startInstance(i->toUInt());
    }

    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    Log::instance() << APPLICATION_NANE << Log::NEWLINE << Log::FLUSH;

    if (!start())
        return -1;

    return a.exec();
}
