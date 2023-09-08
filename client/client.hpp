#pragma once
#include <fcntl.h>
#include <unistd.h>
#include<sys/stat.h>
#include <string>
#include <iostream>
#include <workflow/HttpMessage.h>
#include <workflow/WFTaskFactory.h>
#include <workflow/WFFacilities.h>

class client
{
public:
    client() : url("http://127.0.0.1:8888"){};
    void save_file(std::string get_file_path, std::string save_file_path)
    {
       save_file(get_file_path,save_file_path,-1,0);
    }
    void save_file(std::string get_file_path, std::string save_file_path,size_t st,size_t end){
        WFHttpTask *task = WFTaskFactory::create_http_task(url, 5, 5,
                                                           [this,save_file_path](WFHttpTask *task)
                                                           {
                                                               this->save_file_callback(task,save_file_path);
                                                           });
        auto req = task->get_req();
        req->set_request_uri(get_file_path);
        if(st<=end){
            req->add_header_pair("Range","bytes="+std::to_string(st)+"-"+std::to_string(end));
        }
        task->start();
    }
    void send_file(std::string source_file_path,std::string send_file_path ){
        WFHttpTask *task = WFTaskFactory::create_http_task(url, 5, 5, [](WFHttpTask *task) {});
        protocol::HttpRequest *req = task->get_req();
        req->set_method("POST");
        req->set_request_uri(send_file_path);
        struct stat st;
        stat(source_file_path.c_str(), &st);
        char *file_buff = new char[st.st_size];
        int fd = open(source_file_path.c_str(), O_RDONLY);
        read(fd, file_buff, st.st_size);
        req->append_output_body(file_buff, st.st_size);
        task->start();
    }

private:
    void save_file_callback(WFHttpTask *task,std::string save_file_path);
    std::string url;
};

void client::save_file_callback(WFHttpTask *task,std::string save_file_path)
{
    protocol::HttpResponse *resp = task->get_resp();
    int fd = open(save_file_path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 777);
    const void *body;
    size_t body_len;
    resp->get_parsed_body(&body, &body_len);
    write(fd, body, body_len);
}