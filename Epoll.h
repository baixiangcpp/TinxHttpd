#ifndef __EPOLL_H_
#define __EPOLL_H_

#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <functional> //c++11
#include "ThreadPool.h"
#include "MessageLog.h"

class Epoll {
private:
    int m_efd;
    int m_sfd;
    // to record how many fd int a epoll tree.
    int m_nums; 
    
    struct epoll_event* pEvents;
    MessageLog* m_logger;
    ThreadPool* m_pool;

    std::function<void(int)> m_worker;
private:
    bool setNonBlock(int);
public:
    Epoll(int,std::function<void(int)>);
    Epoll(const Epoll&) = delete;
    Epoll& operator=(const Epoll&) = delete;
    ~Epoll();
    void wait();
};

#endif
