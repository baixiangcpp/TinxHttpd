#include "Epoll.h"

Epoll::Epoll(int fd,std::function<void(int)> worker)
    :m_sfd(fd),m_logger(MessageLog::getLogger()),m_worker(worker)
{
    m_pool = new ThreadPool(m_worker);
    m_pool->startThreads();

    if(!setNonBlock(m_sfd))
    {
        m_logger->errLog("Cannot set server fd non-block.it is quiting...\n");
    }
    
    m_efd = epoll_create(1);
    if(m_efd < 0)
    {
        m_logger->errLog("Cannot create a epoll fd.it is quiting...\n");
    }

    struct epoll_event epevent;
    epevent.data.fd = m_sfd;
    epevent.events = EPOLLIN | EPOLLET;
    if(epoll_ctl(m_efd,EPOLL_CTL_ADD,m_sfd,&epevent) == -1)
    {
        m_logger->errLog("Cannot add server fd into epoll.It is quit...\n");
    }

    m_nums = 1;

    pEvents = new struct epoll_event[1024*2];

}

Epoll::~Epoll()
{
    if(pEvents)
        delete pEvents;
    if(m_pool)
        delete m_pool;
}

bool Epoll::setNonBlock(int fd)
{
    int flag = fcntl(fd,F_GETFL);
    if(flag == -1)
        return false;
    flag |= O_NONBLOCK;
    if (fcntl(fd,F_SETFL,flag) == -1)
        return false;
    return true;
}

//use epoll()
void Epoll::wait()
{
    struct epoll_event epevent;
    int nfds = 0;
    for(;;)
    {
        nfds = epoll_wait(m_efd,pEvents,m_nums,-1); // no timeout
        for(int i=0;i<nfds;++i)
        {
            //new connection.
            if(( pEvents[i].data.fd == m_sfd) && pEvents[i].events & EPOLLIN)
            {
                //not need IP in this version
                int client = accept(m_sfd,0,0);
                m_logger->messageLog("A new client connected.\n");
                
                // if this client can not connect,ignore this.
                if(client == -1)
                    continue;
                
                if(setNonBlock(client))
                {
                    epevent.data.fd = client;
                    epevent.events = EPOLLIN|EPOLLET|EPOLLONESHOT|EPOLLRDHUP|EPOLLERR;//Edge Triggered
                    if(epoll_ctl(m_efd,EPOLL_CTL_ADD,client,&epevent) == -1)
                        continue;//ignore this too.
                    //m_logger->messageLog("A new client.\n");
                }
                // if cannot set the client fd non-block,ignore too.
            }
            //new request
            else if(pEvents[i].events &EPOLLIN) 
            {
                m_pool->addTask(pEvents[i].data.fd);
            }
            //EPOLLRDHUP or EPOLLERR
            else
            {
                if(epoll_ctl(m_efd,EPOLL_CTL_DEL,pEvents[i].data.fd,&pEvents[i])==-1)
                {
                    m_logger->warningLog("A client cannot be remove.\n");
                }
                close(pEvents[i].data.fd);
            }
        }
    }
}
