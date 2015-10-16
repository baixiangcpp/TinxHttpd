#ifndef __DAEMONIZE_H_
#define __DAEMONIZE_H_

/*********************************************
 * I do not use becomeDaemon(),
 * because I want to run this program in 
 * "/www" not "/"
 * ******************************************/
<<<<<<< HEAD
#include "MessageLog.h"
=======

>>>>>>> e972e68a5cddb4f8815309c824fa6977b59f5033
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

class Daemonize{
public:
    static bool beDaemon();
};

#endif
