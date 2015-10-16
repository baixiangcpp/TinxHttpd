#include <regex>
#include <iostream>
#include <string>
<<<<<<< HEAD
#include <strings.h> //for bzero()

int main(int argc,char** argv)
{
    int out = stoi(std::string(argv[1])); //C++11
    int in = stoi(std::string(argv[2]));

    char buf[1024] = {0};
    int res =0;
    //0.pattern
    while(res = read(in,buf,sizeof(buf)))
    {
    }
=======

int main()
{
    //0.pattern
    std::string pattern;
    getline(std::cin,pattern);
>>>>>>> e972e68a5cddb4f8815309c824fa6977b59f5033
    
    //1.choose types
    std::string type;
    std::cin >> type;
    std::regex_constants::syntax_option_type rextype;
    if(type == std::string("egrep"))
        rextype = std::regex_constants::egrep;
    else if(type == std::string("basic"))
        rextype = std::regex_constants::basic;
    else if(type == std::string("extended"))
        rextype = std::regex_constants::extended;
    else if(type == std::string("awk"))
        rextype = std::regex_constants::awk;
    else if(type == std::string("grep"))
        rextype = std::regex_constants::grep;
    else
        rextype = std::regex_constants::ECMAScript;
    
    //2.text to be matched
    std::string tmp,html;
    
    while(getline(std::cin,tmp))
    {
        tmp += '\n';
        html += tmp;
    }
    //3.match and output
    pattern = "[[:alpha:]]*" + pattern + "[[:alpha:]]*";
    std::regex r(pattern);
    for (std::sregex_iterator it(html.begin(), html.end(), r), end;
        it != end;
        ++it)
    {
        std::cout << it->str() << std::endl;
    }
}

