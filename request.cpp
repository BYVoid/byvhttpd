#include <QHostAddress>
#include <QTextStream>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QMutex>
#include "request.h"
#include "log.h"

QString Request::s_root_path;
quint64 Request::s_buffer_size = 65536;

QMutex mutex_log;

bool getRequestProperties(QTcpSocket * socket, QMap<QString, QString> & properties)
{
    QByteArray buffer = socket->readAll();
    QTextStream reader(buffer, QIODevice::ReadOnly);

    QStringList sec = reader.readLine().split(' ');
    if (sec.count() != 3)
        return false;

    properties["_method"] = sec[0];
    properties["_path"] = sec[1];
    properties["_protocol"] = sec[2];

    while (!reader.atEnd())
    {
        QString line = reader.readLine();
        int pos = line.indexOf(':');
        if (pos == -1)
            continue;
        QString key = line.mid(0, pos).toLower();
        QString value = line.mid(pos + 2);
        properties[key] = value;
    }

    return true;
}

void writeResponseHeader(QTcpSocket * socket, quint16 code, QMap<QString, QString> & properties)
{
    if (code == 200)
        socket->write("HTTP/1.0 200 Ok\r\n");
    else if (code == 404)
        socket->write("HTTP/1.0 404 Not Found\r\n");

    properties["Server"] = "byvhttpd";
    properties["Connection"] = "close";
    properties["Date"] = QDateTime::currentDateTimeUtc().toString("ddd, d MMM yyyy hh:mm:ss") + " GMT";

    QMap<QString,QString>::iterator it;
    for (it = properties.begin(); it != properties.end(); ++it)
    {
        socket->write(it.key().toAscii());
        socket->write(": ");
        socket->write(it.value().toAscii());
        socket->write("\r\n");
    }
    socket->write("\r\n");
}

QString getMimeType(QFile & file)
{
    QFileInfo file_info(file);
    QString ext = file_info.suffix();
    if (ext == "html" || ext == "htm")
        return "text/html";
    else if (ext == "jpg")
        return "image/jpeg";
    else
        return "application/octet-stream";
}

void Request::setRootPath(QString root_path)
{
    s_root_path = root_path;
}

void Request::setBufferSize(quint64 buffer_size)
{
    s_buffer_size = buffer_size;
}

Request::Request(int socketDescriptor, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = socketDescriptor;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(terminated()), this, SLOT(deleteLater()));
}

void Request::run()
{
    socket = new QTcpSocket();
    if (!socket->setSocketDescriptor(socketDescriptor))
        return;

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::DirectConnection);
    exec();
}

void Request::onReadyRead()
{
    QMap<QString, QString> request_properties, response_properties;
    getRequestProperties(socket, request_properties);


    mutex_log.lock();
    Log::instance()
            << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") + ' '
            << '[' << socket->peerAddress().toString() << ']'
            << ' ' << request_properties["_method"] << ' '
            << request_properties["_path"] << ' '
            << '[' << (int)QThread::currentThreadId() << ']' << ' ';

    QFile file(s_root_path + request_properties["_path"]);
    if (file.open(QFile::ReadOnly))
    {
        Log::instance() << "200 Ok\n";
        mutex_log.unlock();

        response_properties["Content-Type"] = getMimeType(file);
        response_properties["Content-Length"] = QString("%1").arg(file.size());
        writeResponseHeader(socket, 200, response_properties);

        char * buffer = new char[s_buffer_size];
        while (!file.atEnd())
        {
            qint64 len = file.read(buffer, s_buffer_size);
            socket->write(buffer, len);
            socket->flush();
        }
        delete[] buffer;
    }
    else
    {
        Log::instance() << "404 Not Found\n";
        mutex_log.unlock();

        response_properties["Content-Type"] = "text/html";
        writeResponseHeader(socket, 404, response_properties);
        socket->write("<h1>byvhttpd</h1><p>404 Not Fount.</p>");
    }

    socket->close();
}

void Request::onDisconnected()
{
    socket->deleteLater();
    quit();
}
