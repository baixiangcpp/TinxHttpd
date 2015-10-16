#include "Daemonize.h"

bool Daemonize::beDaemon()
{
    MessageLog* logger = MessageLog::getLogger();
    /*to be background process*/
    pid_t pid = fork();
    if(pid < 0)
    {
        logger->errLog("Failed to fork().\n");
        return false;
    }
    if(pid >0)
    {
        exit(0);
    }

    /*create a new session */
    if(setsid() == -1)
    {
        logger->errLog("Failed to setsid().\n");
        return false;
    }

    /* ensure not be the leader of this session*/

    pid = fork();
    if(pid < 0)
    {
        logger->errLog("Failed to fork().");
        return false;
    }
    if(pid >0)
    {
        exit(0);
    }
    
    umask(0);

    if(chdir("/www")  == -1 )
    {
        return false;
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int fd = open("/dev/null",O_RDWR);
    if(fd  != STDIN_FILENO)
    {
        return false;
    }

    if(dup2(STDIN_FILENO,STDOUT_FILENO) != STDOUT_FILENO)
    {
        return false;
    }

    if(dup2(STDIN_FILENO,STDERR_FILENO) != STDERR_FILENO)
    {
        return false;
    }
    
    return true;
}

