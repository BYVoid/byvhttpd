#ifndef RESPONSE_H
#define RESPONSE_H

#include "common.h"
#include <QMap>
#include <QTcpSocket>

class Response
{
public:
    Response(QTcpSocket * socket, quint16 http_status_code, QMap<QString, QString> & header);
    virtual void response() = 0;

protected:
    void responseHeader();

    QTcpSocket * m_socket;
    quint16 m_http_status_code;
    QMap<QString, QString> m_header;
};

#endif // RESPONSE_H
