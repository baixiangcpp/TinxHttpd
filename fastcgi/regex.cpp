#include <regex>
#include <iostream>
#include <string>
#include <strings.h> //for bzero()
#include <unistd.h>
#include "json/json.h"

int main(int argc,char** argv)
{
    //stoi() ,from C++11
    int in = stoi(std::string(argv[1]));
    int out = stoi(std::string(argv[2]));

    char buf[1024] = {0};
    int res =0;
    
    std::string text;
    
    //read from parent process
    while(res = read(in,buf,sizeof(buf)))
    {
        text += buf;
        bzero(buf,sizeof(buf));
    }

    Json::Reader reader;
    Json::Value value;

    std::string selectpattern;
    std::string regexpression;
    std::string testmatched;
    if (reader.parse(text, value))
    {   
        selectpattern = value["selectpattern"].asString();
        regexpression = value["regexpression"].asString();
        testmatched = value["testmatched"].asString();
    }
    
    std::regex_constants::syntax_option_type rextype;
    if(selectpattern == std::string("egrep"))
        rextype = std::regex_constants::egrep;
    else if(selectpattern == std::string("basic"))
        rextype = std::regex_constants::basic;
    else if(selectpattern == std::string("extended"))
        rextype = std::regex_constants::extended;
    else if(selectpattern == std::string("awk"))
        rextype = std::regex_constants::awk;
    else if(selectpattern == std::string("grep"))
        rextype = std::regex_constants::grep;
    else
        rextype = std::regex_constants::ECMAScript;
    

    regexpression = "[[:alpha:]]*" + regexpression + "[[:alpha:]]*";
    
    std::regex r(regexpression);
    for (std::sregex_iterator it(testmatched.begin(), testmatched.end(), r), end;
        it != end;
        ++it)
    {
        // it->str().length() cannot work well,always return 0.
        // maybe it's a bug in REGEX library
        std::string str = it->str();
        str += '\n';
        write(out,str.c_str(),str.length());
    }
    close(out);
}

