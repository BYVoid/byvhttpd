#include "response.h"
#include "log.h"
#include "httpstatus.h"
#include <QDateTime>
#include <QLocale>

Response::Response(QTcpSocket * socket, quint16 http_status_code, QMap<QString, QString> & header):
    m_socket(socket),
    m_http_status_code(http_status_code),
    m_header(header)
{
    m_header["Server"] = APPLICATION_IDENTIFIER;
    m_header["Connection"] = "close";
    m_header["Date"] =  QLocale(QLocale::English).toString(QDateTime::currentDateTimeUtc(), "ddd, d MMM yyyy hh:mm:ss") + " GMT";
}

void Response::responseHeader()
{
    m_socket->write("HTTP/1.0 ");

    switch (m_http_status_code)
    {
    case 200:
        m_socket->write(HTTP_STATUS_200);
        Log::instance() << HTTP_STATUS_200 << Log::NEWLINE << Log::FLUSH;
        break;
    case 301:
        m_socket->write(HTTP_STATUS_301);
        Log::instance() << HTTP_STATUS_301 << Log::NEWLINE << Log::FLUSH;
        break;
    case 403:
        m_socket->write(HTTP_STATUS_403);
        Log::instance() << HTTP_STATUS_403 << Log::NEWLINE << Log::FLUSH;
        break;
    case 404:
        m_socket->write(HTTP_STATUS_404);
        Log::instance() << HTTP_STATUS_404 << Log::NEWLINE << Log::FLUSH;
        break;
    default:
        m_socket->write(QString("%1").arg(m_http_status_code).toAscii());
    }

    m_socket->write("\r\n");

    QMap<QString,QString>::iterator i;
    for (i = m_header.begin(); i != m_header.end(); ++i)
    {
        m_socket->write(i.key().toAscii());
        m_socket->write(": ");
        m_socket->write(i.value().toAscii());
        m_socket->write("\r\n");
    }
    m_socket->write("\r\n");
}
