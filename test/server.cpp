
#include<iostream>
#include<string.h>
#include<unistd.h>
#include"udp.h"
using namespace std;

const char* msg = 
"%04d \
udp(7)                                                                                                     Miscellaneous Information Manual                                                                                                    udp(7)\n\
\n\
NAME\n\
       udp - User Datagram Protocol for IPv4\n\
\n\
SYNOPSIS\n\
       #include <sys/socket.h>\n\
       #include <netinet/in.h>\n\
       #include <netinet/udp.h>\n\
\n\
       udp_socket = socket(AF_INET, SOCK_DGRAM, 0);\n\
\n\
DESCRIPTION\n\
       This  is  an  implementation  of  the  User Datagram Protocol described in RFC 768.  It implements a connectionless, unreliable datagram packet service.  Packets may be reordered or duplicated before they arrive.  UDP generates and checks\n\
       checksums to catch transmission errors.\n\
\n\
       When a UDP socket is created, its local and remote addresses are unspecified.  Datagrams can be sent immediately using sendto(2) or sendmsg(2) with a valid destination address as an argument.  When connect(2) is called on the socket,  the\n\
       default destination address is set and datagrams can now be sent using send(2) or write(2) without specifying a destination address.  It is still possible to send to other destinations by passing an address to sendto(2) or sendmsg(2).  In\n\
       order  to  receive packets, the socket can be bound to a local address first by using bind(2).  Otherwise, the socket layer will automatically assign a free local port out of the range defined by /proc/sys/net/ipv4/ip_local_port_range and\n\
       bind the socket to INADDR_ANY.\n\
\n\
       All receive operations return only one packet.  When the packet is smaller than the passed buffer, only that much data is returned; when it is bigger, the packet is truncated and the MSG_TRUNC flag is set.  MSG_WAITALL is not supported.\n\
\n\
       IP options may be sent or received using the socket options described in ip(7).  They are processed by the kernel only when the appropriate /proc parameter is enabled (but still passed to the user even when it is turned off).  See ip(7).\n\
\n\
       When the MSG_DONTROUTE flag is set on sending, the destination address must refer to a local interface address and the packet is sent only to that interface.\n\
\n\
       By default, Linux UDP does path MTU (Maximum Transmission Unit) discovery.  This means the kernel will keep track of the MTU to a specific target IP address and return EMSGSIZE when a UDP packet write exceeds it.  When this  happens,  the\n\
       application  should  decrease  the  packet size.  Path MTU discovery can be also turned off using the IP_MTU_DISCOVER socket option or the /proc/sys/net/ipv4/ip_no_pmtu_disc file; see ip(7) for details.  When turned off, UDP will fragment\n\
       outgoing UDP packets that exceed the interface MTU.  However, disabling it is not recommended for performance and reliability reasons.\n\
";

int main(int argc,char* argv[])
{
	int fd = udp_init("127.0.0.1", 9000, 9001);
	if (fd < 0)
	{
		perror("udp_init");
		return 1;
	}
	
	int cnt = 0;
	char buffer[65536];
	while(1)
	{
		sprintf(buffer, msg, cnt++);
		int n = udp_send(fd, buffer, strlen(buffer));
		printf("send %d bytes\n",n);
		sleep(1);
	}
	udp_close(fd);
    return 0;
}
