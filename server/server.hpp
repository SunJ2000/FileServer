#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string>
#include <iostream>
#include <workflow/WFHttpServer.h>
#include <workflow/WFFacilities.h>
#include<workflow/HttpUtil.h>
class FileServer :public WFHttpServer
{
public:
    FileServer():WFHttpServer([this](WFHttpTask *task){this->process(task);}){
    }
   void process(WFHttpTask *server_task);
private:
    // don't support "bytes=-y";
    void parse_range(std::string& value,size_t &st, size_t &end){
        for(int i=0;i<value.length();i++){
            if(value[i]=='='){
                if(value[i+1]!='-'){
                    st=atoi(&value[i+1]);
                }else{
                    st=0;
                }
            }
            if(value[i]=='-'){
                if((i+1)<value.length()){
                    end=atoi(&value[i+1]);
                }
            }
        }        
    }
};

void FileServer::process(WFHttpTask *server_task){
    protocol::HttpRequest *req = server_task->get_req();
	protocol::HttpResponse *resp = server_task->get_resp();
    std::string method=req->get_method();
    std::string file_name = req->get_request_uri();
    if (method == "GET")
    {
        struct stat st;
        stat(file_name.c_str(), &st);
        char *file_buff;
        int fd = open(file_name.c_str(), O_RDONLY);
        size_t start=0,end=st.st_size;
        protocol::HttpHeaderCursor req_cursor(req);
        std::string range_value;
        if(req_cursor.find("Range",range_value)){
           parse_range(range_value,start,end);
        }
        file_buff=new char[end-start+10];
        lseek(fd,start,SEEK_SET);
        read(fd, file_buff, end-start);
        resp->append_output_body(file_buff, end-start);
        server_task->set_callback([file_buff](WFHttpTask *server_task)
                                  { delete[] file_buff;});
    }
    else if(method=="POST"){
        int fd = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 777);
        const void *body;
        size_t body_len;
        req->get_parsed_body(&body, &body_len);
        write(fd, body, body_len);
    }
}



