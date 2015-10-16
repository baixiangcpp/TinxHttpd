#ifndef __DAEMONIZE_H_
#define __DAEMONIZE_H_

/*********************************************
 * I do not use becomeDaemon(),
 * because I want to run this program in 
 * "/www" not "/"
 * ******************************************/
#include "MessageLog.h"

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

class Daemonize{
public:
    static bool beDaemon();
};

#endif
