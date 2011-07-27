#include "log.h"
#include "settings.h"
#include <iostream>
#include <fstream>

QMutex mutex;
std::ofstream flog;

Log * Log::m_instance = NULL;

Log & Log::instance()
{
    if (m_instance == NULL)
        m_instance = new Log();
    return * m_instance;
}

Log::Log()
{
    show_log = Settings::instance().value("httpd/show_log", true).toBool();
    QString filename = Settings::instance().value("httpd/logfile", DEFAULT_HTTPD_LOGFILE).toString();

    if (filename != "")
    {
        flog.open(filename.toUtf8().data(), std::ios_base::app);
        log_file = flog.is_open();
    }
    else
        log_file = false;
}

Log & Log::operator <<(QString str)
{
    mutex.lock();
    if (show_log)
        std::cout << str.toUtf8().data();
    if (log_file)
        flog << str.toUtf8().data();
    mutex.unlock();
    return *this;
}

Log & Log::operator << (int num)
{
    mutex.lock();
    if (show_log)
        std::cout << num;
    if (log_file)
        flog << num;
    mutex.unlock();
    return *this;
}

Log & Log::operator << (const char * str)
{
    mutex.lock();
    if (show_log)
        std::cout << str;
    if (log_file)
        flog << str;
    mutex.unlock();
    return *this;
}

Log & Log::operator << (const char chr)
{
    mutex.lock();
    if (show_log)
        std::cout << chr;
    if (log_file)
        flog << chr;
    mutex.unlock();
    return *this;
}
