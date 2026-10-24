
#include<iostream>
#include<string.h>
#include<unistd.h>
#include"udp.h"
using namespace std;

int main(int argc,char* argv[])
{
	int fd = udp_init("127.0.0.1", 9001, 9000);
	if (fd < 0)
	{
		perror("udp_init");
		return 1;
	}
	
	char buffer[65536] = {0};
	while(1)
	{
		int n = udp_recv(fd, buffer, 65536);
		printf("recv %d bytes, data: ",n);
		if(n > 0)
			for(int i=0;i<n;i++)
				printf("%c",buffer[i]);
		printf("\n");
	}
	udp_close(fd);
    return 0;
}
