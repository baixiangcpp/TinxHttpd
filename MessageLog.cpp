#include "MessageLog.h"
MessageLog* MessageLog::logInstance = 0;

MessageLog* MessageLog::getLogger()
{
    if(logInstance == 0)
        return (logInstance = new MessageLog());
}

MessageLog::MessageLog()
{
    openlog("TinxHttpd",LOG_ODELAY,LOG_USER);
}

MessageLog::~MessageLog()
{
    delete logInstance;
    closelog();
}

void MessageLog::errLog(const char* m)
{
    syslog(LOG_ERR,m);
    exit(-1);
}

void MessageLog::warningLog(const char* m)
{
    syslog(LOG_WARNING,m);
}

void MessageLog::messageLog(const char* m)
{
    syslog(LOG_INFO,m);
}

