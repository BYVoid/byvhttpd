#ifndef REQUEST_H
#define REQUEST_H

#include <QString>
#include <QThread>
#include <QTcpSocket>

class Request : public QThread
{
    Q_OBJECT
public:
    Request(int socketDescriptor, QObject *parent = 0);
    virtual void run();

    static void setRootPath(QString root_path);
    static void setBufferSize(quint64 buffer_size);

private:
    int socketDescriptor;
    QTcpSocket * socket;

    static QString s_root_path;
    static quint64 s_buffer_size;

public slots:
    void onReadyRead();
    void onDisconnected();
};

#endif // REQUEST_H
