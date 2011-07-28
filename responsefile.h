#ifndef RESPONSEFILE_H
#define RESPONSEFILE_H

#include "common.h"
#include "response.h"

class ResponseFile : public Response
{
public:
    ResponseFile(QTcpSocket * socket, quint16 http_status_code, QMap<QString, QString> & header, QString filename);
    virtual void response();

protected:
    QString m_filename;

private:
    static void initialize();

    static bool s_initialized;
    static quint64 s_buffer_size;
};

#endif // RESPONSEFILE_H
