#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#define THREADNUMS 20

#include <pthread.h>
#include <queue>
#include <vector>
#include <functional>
#include "MessageLog.h"

class ThreadPool {
private:
    pthread_mutex_t  m_mutex;
    pthread_cond_t   m_cond;
    
    //client fds
    std::queue<int> m_tasks;
    std::vector<pthread_t> m_threads;
    
    bool m_ready;
    MessageLog* m_logger;
    std::function<void(int)>  m_worker;
public:
    void addTask(int fd);
    int getTask();
    
    void startThreads();   
    ThreadPool(std::function<void(int)>);
    
    //pthread_create need void*(*runtine)(void*)
    //but if it is not static
    //its type is void* (*routine)(ThreadPool::)(void*)
    static void* thread_routine(void *args);
};

#endif
