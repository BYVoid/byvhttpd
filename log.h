#ifndef LOG_H
#define LOG_H

#include <QString>

class Log
{
public:
    Log & operator << (QString logstr);
    Log & operator << (const char * str);
    Log & operator << (const char chr);
    Log & operator << (int num);

    static Log & instance();

private:
    Log();
    static Log * m_instance;

    bool log_file;
    bool show_log;
};

#endif // LOG_H
