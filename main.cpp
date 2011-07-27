#include "common.h"
#include "serverthread.h"
#include "request.h"
#include "settings.h"
#include "log.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <QVariantList>

void initialize()
{
    Log::instance() << APPLICATION_NANE << Log::NEWLINE << Log::FLUSH;

    QString root_path = Settings::instance().value("site/root_path").toString();
    Request::setRootPath(root_path);

    quint64 buffer_size = Settings::instance().value("httpd/buffer_size", DEFAULT_HTTPD_BUFFER_SIZE).toULongLong();
    Request::setBufferSize(buffer_size);
}

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

    initialize();

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
