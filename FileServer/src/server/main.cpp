#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<string>
#include<iostream>
#include<workflow/WFHttpServer.h>
#include<workflow/WFFacilities.h>

void process(WFHttpTask * server_task){
    protocol::HttpRequest *req = server_task->get_req();
	protocol::HttpResponse *resp = server_task->get_resp();
    std::string method=req->get_method();
    std::string file_name = req->get_request_uri();
    if (method == "GET")
    {
        struct stat st;
        stat(file_name.c_str(), &st);
        char *file_buff = new char[st.st_size];
        int fd = open(file_name.c_str(), O_RDONLY);
        read(fd, file_buff, st.st_size);
        resp->append_output_body(file_buff, st.st_size);
        server_task->set_callback([file_buff](WFHttpTask *server_task)
                                  { delete[] file_buff; });
    }
    else if(method=="POST"){
        int fd = open(file_name.c_str(), O_RDWR | O_CREAT | O_TRUNC, 777);
        const void *body;
        size_t body_len;
        req->get_parsed_body(&body, &body_len);
        write(fd, body, body_len);
    }
}
static WFFacilities::WaitGroup wait_group(1);

int main(){
    WFHttpServer server(process);
    if (server.start(8888) == 0)	{
		wait_group.wait();
		server.stop();
	}
	else
	{
		perror("Cannot start server");
		exit(1);
	}
	return 0;
}