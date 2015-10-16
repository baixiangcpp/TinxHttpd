#include "WebServer.h"

//init this web server
//get socket,bind it,then listen

WebServer::WebServer()
    :m_logger(MessageLog::getLogger())
{
    m_sfd = socket(AF_INET,SOCK_STREAM,0);
    if(m_sfd < 0)
    {
        m_logger->errLog("Cannnot get a server socket,It is quiting...");
        exit(-1);
    }
    
    struct sockaddr_in saddr;
    bzero(&saddr,sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(80);
    inet_pton(AF_INET,"192.168.226.129",&saddr.sin_addr.s_addr);
    
    if(bind(m_sfd,(struct sockaddr*)&saddr,sizeof(saddr)) < 0)
    {
        m_logger->errLog("Cannnot  bind the socket,it is quiting...");
        exit(-1);
    }

    listen(m_sfd,100);
}

void WebServer::run()
{
    Epoll ep(m_sfd,Request::handler);
    ep.wait();
}
