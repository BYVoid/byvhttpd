#ifndef LOG_H
#define LOG_H

#include "common.h"
#include <sstream>
#include <QThreadStorage>

class Log
{
public:
    enum ctrl_t {FLUSH, NEWLINE};

    virtual ~Log();
    Log & operator << (QString logstr);
    Log & operator << (const char * str);
    Log & operator << (const char chr);
    Log & operator << (int num);
    Log & operator << (ctrl_t code);

    static Log & instance();

private:
    Log();
    static Log * m_instance;

    QThreadStorage<std::ostringstream *> buffer;
    bool log_file;
    bool show_log;
};

#endif // LOG_H
