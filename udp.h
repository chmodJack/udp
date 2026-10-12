#ifndef UDP_H
#define UDP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Both ports must be known and nonzero; peer_ip is a numeric IPv4/IPv6 address. */
int udp_init(const char *peer_ip, unsigned short peer_port,
             unsigned short local_port);

/* Send or receive one datagram. Return its byte count, or -1 with errno set. */
int udp_send(int fd, const void *data, size_t size);
int udp_recv(int fd, void *data, size_t size);
void udp_close(int fd);

#ifdef __cplusplus
}
#endif

#endif
