
#include<iostream>
#include<string.h>
#include<unistd.h>
#include"udp.h"
using namespace std;

int main(int argc,char* argv[])
{
	int fd = udp_init("127.0.0.1", 9000, 9001);
	
	int n = 0;
	char buffer[512];
	while(1)
	{
		sprintf(buffer, "hello udp, num is %d", n++);
		udp_send(fd, buffer, strlen(buffer));
		sleep(1);
	}
	udp_close(fd);
    return 0;
}
