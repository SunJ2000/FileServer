#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string>
#include <iostream>
#include <workflow/WFHttpServer.h>
#include <workflow/WFFacilities.h>


class FileServer :public WFHttpServer
{
public:
    FileServer():WFHttpServer([this](WFHttpTask *task){this->process(task);}){
        WFHttpServer::start(8888);
    }
   void process(WFHttpTask *task);
private:
    
};