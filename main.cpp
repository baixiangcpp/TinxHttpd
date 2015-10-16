#include "Daemonize.h"
#include "WebServer.h"
#include "MessageLog.h"

int main()
{
    if(!Daemonize::beDaemon())
    {
        MessageLog* logger = MessageLog::getLogger();
        logger->errLog("This process cannnot be daemon.\n");
    }
    WebServer wser;
    wser.run();
}
