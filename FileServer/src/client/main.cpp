#include<string>
#include<sstream>
#include"client.hpp"

static WFFacilities::WaitGroup wait_group(1);
int main(){
    client cl;
    std::string command;
    while (std::getline(std::cin,command))
    {
        std::istringstream istrcommand(command);
        std::string method,para1,para2;
        istrcommand>>method>>para1>>para2;
        if(method=="get"){
            cl.save_file(para1,para2);
        }else if(method=="send"){
           cl.send_file(para1,para2);
        }else{
            std::cout<<"error command"<<std::endl;
        }
        
    } 
    wait_group.wait();
	return 0;
}