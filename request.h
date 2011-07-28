#ifndef REQUEST_H
#define REQUEST_H

#include "common.h"
#include <QThread>
#include <QTcpSocket>
#include <QFile>
#include <QStringList>

class Request : public QThread
{
    Q_OBJECT
public:
    Request(int socketDescriptor, QObject *parent = 0);
    virtual void run();

private:
    int socketDescriptor;
    QTcpSocket * socket;
    QMap<QString, QString> request_header, response_header;
    quint16 response_code;
    QString response_filename;

    static bool s_initialized;
    static QString s_root_path;
    static quint64 s_buffer_size;
    static QStringList s_index;

    bool getRequestHeader();
    void writeResponseHeader();
    void responseFile();
    void tryResponseFile(QString filename);

    static void initialize();

public slots:
    void onReadyRead();
    void onDisconnected();
};

#endif // REQUEST_H
