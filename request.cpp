#include "log.h"
#include "settings.h"
#include "request.h"
#include "responsefile.h"
#include "responsedirectory.h"
#include <QHostAddress>
#include <QTextStream>
#include <QStringList>
#include <QFileInfo>
#include <QDateTime>

QString Request::s_root_path;
QStringList Request::s_index;
bool Request::s_initialized = false;

void Request::initialize()
{
    s_initialized = true;
    s_root_path = Settings::instance().value("site/root_path").toString();
    s_index = Settings::instance().value("site/index").toStringList();
}

Request::Request(int socketDescriptor, QObject *parent) :
    QThread(parent),
    response(NULL)
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
                response = new ResponseDirectory(socket, response_header, filename, request_header["_path"]);
                //response_code = 403;
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
            if (request_header.contains("if-modified-since"))
            {
                if (request_header["if-modified-since"] == Common::getTimeStampString(file_info.lastModified().toUTC()))
                {
                    response_code = 304;
                    return;
                }
            }

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
            << request_header["_path"] << ' ';

    tryResponseFile(s_root_path + request_header["_path"]);

    if (response == NULL)
    {
        response = new ResponseFile(socket, response_code, response_header, response_filename);
    }

    response->response();
    socket->close();
}

void Request::onDisconnected()
{
    socket->deleteLater();
    quit();
}
