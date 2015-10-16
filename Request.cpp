#include "Request.h"
<<<<<<< HEAD

/*************************************************
 *This function is used to get the request methods,
 *only 'GET' and 'POST' can be deal with.
 *Return true on success,or false when the methods 
 *cannot  be deal with.
 ************************************************/
=======
#include <string.h>/////////////////////////////
#include <iostream>
>>>>>>> e972e68a5cddb4f8815309c824fa6977b59f5033

Request::Request(int client)
    :m_client(client),m_iscgi(false)
{
    char c=0;
    std::string method;
    int res = 0;
    
    for(;;)
    {
        res = read(m_client,&c,1);
        if(res < 0)
            return;
        if(c == ' ')
            break;
        method += c;
    }
    
    if(!strcasecmp(method.c_str(),"Get"))
        m_method = std::string("GET");
    else if(!strcasecmp(method.c_str(),"POST")) {
        m_method = std::string("POST");
        m_iscgi = true;
    }

    m_logger = MessageLog::getLogger();
}

<<<<<<< HEAD
void Request::handler(int client)
{
    Request r(client);
    r.work();
}

=======
/*************************************************
 *This function is used to get the request methods,
 *only 'GET' and 'POST' can be deal with.
 *Return true on success,or false when the methods 
 *cannot  be deal with.
 ************************************************/
>>>>>>> e972e68a5cddb4f8815309c824fa6977b59f5033

void Request::getPath()
{
    char c=0;
    int res;
    
    for(;;)
    {
        res = read(m_client,&c,1);
        if(res < 0)
            return;
        if(c == ' ')
            break;
        m_path += c;
    }
}

void Request::work()
{
    getPath();
<<<<<<< HEAD
    std::string file("htdocs");
=======
    std::string file("/www/htdocs");//////////////////////////////
>>>>>>> e972e68a5cddb4f8815309c824fa6977b59f5033
    file += m_path;
    
    if(file[file.length()-1] == '/')
        file += "index.html";
    
    struct stat st= {0};
    
    if(stat(file.c_str(),&st) == -1)
        sendNotFound();
    else if(st.st_mode & 0111)
        execCGI(file);
    else
        sendFile(file);
}

void Request::sendNotFound()
{
    const char* file = "htdocs/404.html";
    int fd = open(file,O_RDONLY);
    if(fd == -1)
    {
        m_logger->warningLog("Can not find 404 html.\n");
        return ;
    }
    char header[] = {"HTTP/1.1 404 NOT FOUND \r\n" \
                        "Server: TinxHttpd v1.0 \r\n" \
                        "Content-Type: text/html \r\n" \
                        "\r\n"};
    send(m_client,header,sizeof(header),0);
   
     
    char buf[1024] = {0};
    int res =0;
    while((res=read(fd,buf,sizeof(buf)))>0)
    {
        send(m_client,buf,res,0);
        bzero(buf,sizeof(buf));
    }
    
    shutdown(m_client,O_RDWR);
    close(fd);
    close(m_client);
}

void Request::sendFile(std::string file)
{
    int fd = open(file.c_str(),O_RDONLY);
    if(fd == -1)
    {
        m_logger->warningLog("Can not find html.\n");
        return ;
    }
    char header[] = {"HTTP/1.1 200 OK \r\n" \
                        "Server: TinxHttpd v1.0 \r\n" \
                        "Content-Type: text/html \r\n" \
                        "\r\n"};
    send(m_client,header,sizeof(header),0);
   
     
    char buf[1024] = {0};
    int res =0;
    while((res=read(fd,buf,sizeof(buf)))>0)
    {
        send(m_client,buf,res,0);
        bzero(buf,sizeof(buf));
    }
    
    shutdown(m_client,O_RDWR);
    close(fd);
    close(m_client);
}

void Request::execCGI(std::string file)
{
    
    if(m_method != std::string("POST"))
    {
        sendNotFound();
        close(m_client);
        return ;
    }

    /************************************************
     *           child              child
     *             |                  ↑
     *             | infds            | outfds
     *             ↓                  |
     *           parent             parent
     * *********************************************/
    
    int infds[2];
    int outfds[2];

    pipe(infds);
    pipe(outfds);

    pid_t pid = fork();
    
    if(pid < 0)
    {
        // do not exit()
        return ; 
    }

    //child process
    else if(pid == 0)
    {
        close(infds[0]);
        close(outfds[1]);
        char in[8] = {0};
        char out[8] ={0};
        
        sprintf(in,"%d",infds[1]);
        sprintf(out,"%d",outfds[0]);

        char* argument[] = {(char*)strrchr(file.c_str(),'/'),in,out,0};
        execve(file.c_str(),argument,0);
    }
    //parent go on
    close(infds[1]);
    close(outfds[0]);
    discardHeaders();
    
    // Read post datas
    char buf[1024] = {0};
    
    int res = 0;
    while(res = read(m_client,buf,sizeof(buf)))
    {
        write(1,buf,res);
        bzero(buf,sizeof(buf));
    }
    
    // recv result from cgi
    while(res = read(infds[0],buf,sizeof(buf)))
    {
        write(m_client,buf,res);
        bzero(buf,sizeof(buf));
    }

    wait(0);

    shutdown(m_client,O_RDWR);
    close(m_client);
}


//discard headers.
void Request::discardHeaders()
{
    char c;
    while(getline() != '\r')
        ;
}


//getline  
char Request::getline()
{
    char firstchar =0;
	char c = 0;
	int n;
    int i=0;
	while (c != '\n')
	{
		n = recv(m_client, &c, 1, 0);
        if(n<0)
            return -1;
        if(i == 0)
            firstchar = c;
        ++i;
	}
    return firstchar;
}

