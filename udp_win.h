#ifndef UDP_WIN_H
#define UDP_WIN_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <stddef.h>
#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Both ports must be known and nonzero; peer_ip is a numeric IPv4/IPv6 address. */
SOCKET udp_init(const char *peer_ip, unsigned short peer_port,
                unsigned short local_port);

/* Return the byte count, or SOCKET_ERROR; inspect WSAGetLastError() on error. */
int udp_send(SOCKET fd, const void *data, size_t size);
int udp_recv(SOCKET fd, void *data, size_t size);
void udp_close(SOCKET fd);

#ifdef __cplusplus
}
#endif

#endif
