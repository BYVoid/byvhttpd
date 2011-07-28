#include "responsefile.h"
#include "mime.h"
#include "settings.h"
#include <QFile>
#include <QFileInfo>

bool ResponseFile::s_initialized = false;
quint64 ResponseFile::s_buffer_size = DEFAULT_HTTPD_BUFFER_SIZE;

void ResponseFile::initialize()
{
    s_initialized = true;
    s_buffer_size = Settings::instance().value("httpd/buffer_size", DEFAULT_HTTPD_BUFFER_SIZE).toULongLong();
}

ResponseFile::ResponseFile(QTcpSocket * socket, quint16 http_status_code, QMap<QString, QString> & header, QString filename):
    Response(socket, http_status_code, header),
    m_filename(filename)
{
    if (!s_initialized)
        initialize();
    if (m_http_status_code != 200)
    {
        m_filename = QString("response/%1.html").arg(m_http_status_code);
    }
    QFileInfo file_info(m_filename);
    m_header["Content-Type"] = Mime::instance().getMimeType(file_info.suffix());
    m_header["Content-Length"] = QString("%1").arg(file_info.size());
}

void ResponseFile::response()
{
    responseHeader();

    QFile file(m_filename);
    if (!file.open(QFile::ReadOnly))
        ;//TODO error

    char * buffer = new char[s_buffer_size];
    while (!file.atEnd())
    {
        qint64 len = file.read(buffer, s_buffer_size);
        m_socket->write(buffer, len);
        m_socket->flush();
    }
    delete[] buffer;
}

