#include "Daemonize.h"
#include "WebServer.h"
#include "MessageLog.h"

int main()
{
<<<<<<< HEAD
    if(!Daemonize::beDaemon())
    {
        MessageLog* logger = MessageLog::getLogger();
        logger->errLog("This process cannnot be daemon.\n");
    }
=======
    /*if(!Daemonize::beDaemon())
    {
        MessageLog* logger = MessageLog::getLogger();
        logger->errLog("This process cannnot be daemon.\n");
    }*/
>>>>>>> e972e68a5cddb4f8815309c824fa6977b59f5033
    WebServer wser;
    wser.run();
}
