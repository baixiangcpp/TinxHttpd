#include "ThreadPool.h"

ThreadPool::ThreadPool(std::function<void(int)> worker)
    :m_worker(worker),m_ready(false)
{
    m_logger = MessageLog::getLogger();

    m_threads.resize(THREADNUMS);

    pthread_mutex_init(&m_mutex,0);
    pthread_cond_init(&m_cond,0);

}

void ThreadPool::startThreads()
{
    int res = 0;
    for(int i=0;i<THREADNUMS;++i)
    {
        res = pthread_create(&m_threads[i],0,thread_routine,this);
        if(res< 0)
        {
            m_logger->errLog("Cannot create thread.\n");
        }
    }
}

void* ThreadPool::thread_routine(void *arg)
{
    ThreadPool* pt = (ThreadPool*)arg;
    std::function<void*(void*)> worker;
    for(;;)
    {
        pthread_mutex_lock(&pt->m_mutex);
        while(!pt->m_ready)
        {
            pthread_cond_wait(&pt->m_cond, &pt->m_mutex) ;
        }
        int client = pt->getTask();
        pthread_mutex_unlock(&pt->m_mutex);
        pt->m_worker(client);
    }
}

void ThreadPool::addTask(int fd)
{
    pthread_mutex_lock(&m_mutex);
    m_tasks.push(fd);
    m_ready = true;
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}

int ThreadPool::getTask()
{
    int fd = m_tasks.front();
    m_tasks.pop();
    if(m_tasks.empty())
        m_ready = false;
    return fd;
}

