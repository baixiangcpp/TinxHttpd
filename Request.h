#ifndef __REQUEST_H_
#define __REQUEST_H_

#include <unistd.h>
#include <string>
#include <vector>
#include <strings.h>    //for strcasecmp()
#include <string.h>     //for strrchr()
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h> 
#include "MessageLog.h"

class Request {
private:
    int m_client;
    bool m_iscgi;
    std::string m_method;
    std::string m_path;

    MessageLog* m_logger;
private:
    void discardHeaders();
    char getline();
public:
    Request(int);
    Request(const Request&) = delete;
    Request& operator=(const Request&) = delete;
    
    void getPath();
    void work();  
    void sendNotFound();
    void sendFile(std::string);
    void execCGI(std::string);
<<<<<<< HEAD

    static void handler(int);
=======
>>>>>>> e972e68a5cddb4f8815309c824fa6977b59f5033
//    std::string GetFormData();
    
};

#endif
