#ifndef __MESSAGELOG_H_
#define __MESSAGELOG_H_

#include <syslog.h>
#include <stdlib.h>

/*********************************
 * sigleton pattern
 * one process one logger 
********************************/

class MessageLog {
private:
    MessageLog();
    static MessageLog* logInstance;
public:
    static MessageLog* getLogger();
    ~MessageLog();
    void errLog(const char*);
    void warningLog(const char*);
    void messageLog(const char*);
};

#endif
