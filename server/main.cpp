#include"server.hpp"

static WFFacilities::WaitGroup wait_group(1);

int main(){

    FileServer server;
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