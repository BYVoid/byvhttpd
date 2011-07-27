#include "request.h"
#include "log.h"
#include "mime.h"
#include <QHostAddress>
#include <QTextStream>
#include <QStringList>
#include <QFileInfo>
#include <QDateTime>

QString Request::s_root_path;
quint64 Request::s_buffer_size = DEFAULT_HTTPD_BUFFER_SIZE;

bool getRequestHeader(QTcpSocket * socket, QMap<QString, QString> & header)
{
    QByteArray buffer = socket->readAll();
    QTextStream reader(buffer, QIODevice::ReadOnly);

    QStringList sec = reader.readLine().split(' ');
    if (sec.count() != 3)
        return false;

    header["_method"] = sec[0];
    header["_path"] = sec[1];
    header["_protocol"] = sec[2];

    while (!reader.atEnd())
    {
        QString line = reader.readLine();
        int pos = line.indexOf(':');
        if (pos == -1)
            continue;
        QString key = line.mid(0, pos).toLower();
        QString value = line.mid(pos + 2);
        header[key] = value;
    }

    return true;
}

void writeResponseHeader(QTcpSocket * socket, quint16 code, QMap<QString, QString> & header)
{
    socket->write("HTTP/1.0 ");

    switch (code)
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
        socket->write(QString(code).toAscii());
    }

    socket->write("\r\n");

    header["Server"] = APPLICATION_IDENTIFIER;
    header["Connection"] = "close";
    header["Date"] = QDateTime::currentDateTimeUtc().toString("ddd, d MMM yyyy hh:mm:ss") + " GMT";

    QMap<QString,QString>::iterator it;
    for (it = header.begin(); it != header.end(); ++it)
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
    return Mime::instance().getMimeType(file_info.suffix());
}

void Request::responseFile(QFile & file, QMap<QString, QString> & header, quint16 code)
{
    header["Content-Type"] = getMimeType(file);
    header["Content-Length"] = QString("%1").arg(file.size());

    writeResponseHeader(socket, code, header);

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
}

void Request::responseFile(QString filename, QMap<QString, QString> & header, quint16 code)
{
    QFile file(filename);
    responseFile(file, header, code);
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
    QMap<QString, QString> request_header, response_header;
    getRequestHeader(socket, request_header);

    Log::instance()
            << QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss") + ' '
            << '[' << socket->peerAddress().toString() << ']'
            << ' ' << request_header["_method"] << ' '
            << request_header["_path"] << ' '
            << '[' << (int)QThread::currentThreadId() << ']' << ' ';

    QFile file(s_root_path + request_header["_path"]);
    QFileInfo file_info(file);
    if (file_info.exists())
    {
        if (file_info.isDir())
        {
            if (request_header["_path"].at(request_header["_path"].length() - 1) == '/')
            {
                responseFile("response/403.html", response_header, 403);
            }
            else
            {
                response_header["Location"] = "http://" + request_header["host"] + request_header["_path"] + '/';
                responseFile("response/301.html", response_header, 301);
            }
        }
        else if (file_info.isReadable())
        {
            responseFile(file, response_header, 200);
        }
        else
        {
            responseFile("response/403.html", response_header, 403);
        }
    }
    else
    {
        responseFile("response/404.html", response_header, 404);
    }

    socket->close();
}

void Request::onDisconnected()
{
    socket->deleteLater();
    quit();
}
