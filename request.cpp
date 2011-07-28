#include "request.h"
#include "log.h"
#include "mime.h"
#include "settings.h"
#include <QHostAddress>
#include <QTextStream>
#include <QStringList>
#include <QFileInfo>
#include <QDateTime>

QString Request::s_root_path;
quint64 Request::s_buffer_size = DEFAULT_HTTPD_BUFFER_SIZE;
QStringList Request::s_index;
bool Request::s_initialized = false;

void Request::initialize()
{
    s_initialized = true;
    s_buffer_size = Settings::instance().value("httpd/buffer_size", DEFAULT_HTTPD_BUFFER_SIZE).toULongLong();
    s_root_path = Settings::instance().value("site/root_path").toString();
    s_index = Settings::instance().value("site/index").toStringList();
}

Request::Request(int socketDescriptor, QObject *parent) :
    QThread(parent)
{
    if (!s_initialized)
        initialize();
    this->socketDescriptor = socketDescriptor;
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(terminated()), this, SLOT(deleteLater()));
}

void Request::run()
{
    socket = new QTcpSocket();
    if (!socket->setSocketDescriptor(socketDescriptor))
        return; //TODO error

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::DirectConnection);
    exec();
}

bool Request::getRequestHeader()
{
    QByteArray buffer = socket->readAll();
    QTextStream reader(buffer, QIODevice::ReadOnly);

    QStringList sec = reader.readLine().split(' ');
    if (sec.count() != 3)
        return false;

    request_header["_method"] = sec[0];
    request_header["_path"] = sec[1];
    request_header["_protocol"] = sec[2];

    while (!reader.atEnd())
    {
        QString line = reader.readLine();
        int pos = line.indexOf(':');
        if (pos == -1)
            continue;
        QString key = line.mid(0, pos).toLower();
        QString value = line.mid(pos + 2);
        request_header[key] = value;
    }

    return true;
}

void Request::writeResponseHeader()
{
    socket->write("HTTP/1.0 ");

    switch (response_code)
    {
    case 200:
        socket->write(HTTP_STATUS_200);
        Log::instance() << HTTP_STATUS_200 << Log::NEWLINE << Log::FLUSH;
        break;
    case 301:
        socket->write(HTTP_STATUS_301);
        Log::instance() << HTTP_STATUS_301 << Log::NEWLINE << Log::FLUSH;
        break;
    case 403:
        socket->write(HTTP_STATUS_403);
        Log::instance() << HTTP_STATUS_403 << Log::NEWLINE << Log::FLUSH;
        break;
    case 404:
        socket->write(HTTP_STATUS_404);
        Log::instance() << HTTP_STATUS_404 << Log::NEWLINE << Log::FLUSH;
        break;
    default:
        socket->write(QString(response_code).toAscii());
    }

    socket->write("\r\n");

    response_header["Server"] = APPLICATION_IDENTIFIER;
    response_header["Connection"] = "close";
    response_header["Date"] = QDateTime::currentDateTimeUtc().toString("ddd, d MMM yyyy hh:mm:ss") + " GMT";

    QMap<QString,QString>::iterator i;
    for (i = response_header.begin(); i != response_header.end(); ++i)
    {
        socket->write(i.key().toAscii());
        socket->write(": ");
        socket->write(i.value().toAscii());
        socket->write("\r\n");
    }
    socket->write("\r\n");
}

void Request::responseFile()
{
    if (response_code != 200)
    {
        response_filename = QString("response/%1.html").arg(response_code);
    }

    QFile file(response_filename);
    QFileInfo file_info(file);
    response_header["Content-Type"] = Mime::instance().getMimeType(file_info.suffix());
    response_header["Content-Length"] = QString("%1").arg(file.size());

    writeResponseHeader();

    if (!file.open(QFile::ReadOnly))
        ;//TODO error

    char * buffer = new char[s_buffer_size];
    while (!file.atEnd())
    {
        qint64 len = file.read(buffer, s_buffer_size);
        socket->write(buffer, len);
        socket->flush();
    }
    delete[] buffer;

    socket->close();
}

void Request::tryResponseFile(QString filename)
{
    QFileInfo file_info(filename);

    if (file_info.isDir())
    {
        if (request_header["_path"].at(request_header["_path"].length() - 1) == '/')
        {
            if (s_index.size() != 0)
            {
                for (QStringList::Iterator i = s_index.begin(); i != s_index.end(); ++i)
                {
                    tryResponseFile(filename + *i);
                    if (response_code != 404)
                        return;
                }
                response_code = 404;
            }
            else
            {
                response_code = 403;
            }
        }
        else
        {
            response_header["Location"] = "http://" + request_header["host"] + request_header["_path"] + '/';
            response_code = 301;
        }
    }
    else if (file_info.exists())
    {
        if (file_info.isReadable())
        {
            response_filename = filename;
            response_code = 200;
        }
        else
        {
            response_code = 403;
        }
    }
    else
    {
        response_code = 404;
    }
}


void Request::onReadyRead()
{
    if (!getRequestHeader())
        return;

    Log::instance()
            << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") + ' '
            << '[' << socket->peerAddress().toString() << ']'
            << ' ' << request_header["_method"] << ' '
            << request_header["_path"] << ' '
            << '[' << (int)QThread::currentThreadId() << ']' << ' ';

    QString path = s_root_path + request_header["_path"];
    tryResponseFile(path);
    responseFile();
}

void Request::onDisconnected()
{
    socket->deleteLater();
    quit();
}
