#ifndef COMMON_H
#define COMMON_H

#include <QString>

const QString APPLICATION_NANE = "byvhttpd";
const QString APPLICATION_VERSION = "0.0.1";
const QString APPLICATION_IDENTIFIER = APPLICATION_NANE + '/' + APPLICATION_VERSION;
const QString CONFIG_FILE = "byvhttpd.ini";
const QString MIME_TYPE_FILE = "mime.txt";

const quint64 DEFAULT_HTTPD_BUFFER_SIZE = 65536;
const QString DEFAULT_HTTPD_LOGFILE = "byvhttpd.log";

#endif // COMMON_H
