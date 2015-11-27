#include "Request.h"

/*************************************************
 *This function is used to get the request methods,
 *only 'GET' and 'POST' can be deal with.
 *Return true on success,or false when the methods 
 *cannot  be deal with.
 ************************************************/

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

void Request::handler(int client)
{
    Request r(client);
    r.work();
}

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
    std::string file("htdocs");
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
        
        sprintf(out,"%d",infds[1]);
        sprintf(in,"%d",outfds[0]);
        
        char* argument[] = {(char*)strrchr(file.c_str(),'/'),in,out,0};
        execve(file.c_str(),argument,0);
    }
   
    //parent go on
    close(infds[1]);
    close(outfds[0]);

    int length = getLengthAndDiscardHeaders(); 
    
    //read post datas.
    char buf[1024] = {0};
    int res = 0;
    int nums = 0;
    std::string tmp;
    while(((res = read(m_client,buf,sizeof(buf)-1)) != -1) || (nums != length))
    {
        if(res == -1)
        {
            continue;
        }
        nums += res;
        write(outfds[1],buf,res);
        bzero(buf,sizeof(buf));
    }
    close(outfds[1]);

    
    //recv result from cgi
    bzero(buf,sizeof(buf));
    std::string result;
    
    while(res = read(infds[0],buf,sizeof(buf)))
    {
        result += buf;
        bzero(buf,sizeof(buf));
    }
    wait(0);

    char re[] = {
                 "HTTP/1.1 200 OK \r\n" \
                 "Server: TinxHttpd v1.0 \r\n" \
                 "Content-Type: text/html \r\n" \
                 "\r\n" \
                };
    result = re + result;
  
    send(m_client,result.c_str(),result.length(),0);
    shutdown(m_client,O_RDWR);
    close(m_client);
}


/**************************************************
 * read content-length is necessary ,
 * when large data in socket,they may in differnt 
 * "frames" and the socket is non-blocking we may 
 * lose some of them discard headers.
 * ***********************************************/

int Request::getLengthAndDiscardHeaders()
{
    bool flag = false;
    int contentlength = 0;
    char buf[1024] = {0};
    while(getline(buf,sizeof(buf)) && strncmp(buf,"\r\n",2))
    {
        buf[15] = 0;
        if(!strcasecmp(buf,"Content-Length:"))
        {
            contentlength = atoi(&buf[16]);
            if(contentlength < 0)
            {
                errRequest();
                return -1;
            }
            else
            {
                flag = true;
            }
        }
        bzero(buf,sizeof(buf));
    }

    if(!flag)
    {
        errRequest();
        return -1;
    }
    else
        return contentlength;
}


//getline  
int Request::getline(char* buf, int size)
{
    int i = 0;
    char c = 0;
    int n;

    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(m_client, &c, 1, 0);
        if (n > 0)
        {
            buf[i] = c;
            i++;
        }
		else
            break;
    }
    buf[i] = 0;
    return i;
}

void Request::errRequest()
{
    char header[] = {
                 "HTTP/1.1 400 BAD REQUEST \r\n" \
                 "Server: TinxHttpd v1.0 \r\n" \
                 "Content-Type: text/html \r\n" \
                 "\r\n" \
                 "Content-length is need in post.\n"
                };
    shutdown(m_client,O_RDWR);
    close(m_client);
}
