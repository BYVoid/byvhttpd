#include "common.h"
#include "serverthread.h"
#include "request.h"
#include "log.h"
#include "settings.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <QVariantList>

void startServer(quint16 port)
{
    ServerThread * server_thread = new ServerThread();
    server_thread->setPort(port);
    server_thread->start();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    Log::instance() << APPLICATION_NANE << Log::NEWLINE << Log::FLUSH;

    QVariantList port_list = Settings::instance().value("httpd/port").toList();
    if (port_list.size() == 0)
    {
        Log::instance() << "No port specified to listen." << Log::NEWLINE << Log::FLUSH;
        return 1;
    }

    for (QVariantList::iterator i = port_list.begin(); i != port_list.end(); ++i)
    {
        startServer(i->toUInt());
    }

    return a.exec();
}
