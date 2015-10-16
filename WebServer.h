#ifndef __WEBSERVER_H_
#define __WEBSERVER_H_

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "MessageLog.h"
#include "Epoll.h"
#include "Request.h"

class WebServer{
public:
    WebServer();
    void run();
private:
    int m_sfd;
    MessageLog* m_logger;
};

#endif
