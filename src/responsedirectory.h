#ifndef RESPONSEDIRECTORY_H
#define RESPONSEDIRECTORY_H

#include "common.h"
#include "response.h"

class ResponseDirectory : public Response
{
public:
    ResponseDirectory(QTcpSocket * socket, QMap<QString, QString> & header, QString dir_name, QString url_path);
    virtual void response();

protected:
    QString m_dir_name;
    QString m_url_path;
};

#endif // RESPONSEDIRECTORY_H
