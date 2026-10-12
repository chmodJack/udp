#include "udp.h"

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int udp_init(const char *peer_ip, unsigned short peer_port,
             unsigned short local_port)
{
    struct sockaddr_storage peer = {0}, local = {0};
    socklen_t addrlen;

    if (!peer_ip || !peer_port || !local_port) {
        errno = EINVAL;
        return -1;
    }

    struct sockaddr_in *p4 = (struct sockaddr_in *)&peer;
    struct sockaddr_in *l4 = (struct sockaddr_in *)&local;
    if (inet_pton(AF_INET, peer_ip, &p4->sin_addr) == 1) {
        p4->sin_family = l4->sin_family = AF_INET;
        p4->sin_port = htons(peer_port);
        l4->sin_port = htons(local_port);
        l4->sin_addr.s_addr = htonl(INADDR_ANY);
        addrlen = sizeof(*p4);
    } else {
        struct sockaddr_in6 *p6 = (struct sockaddr_in6 *)&peer;
        struct sockaddr_in6 *l6 = (struct sockaddr_in6 *)&local;
        if (inet_pton(AF_INET6, peer_ip, &p6->sin6_addr) != 1) {
            errno = EINVAL;
            return -1;
        }
        p6->sin6_family = l6->sin6_family = AF_INET6;
        p6->sin6_port = htons(peer_port);
        l6->sin6_port = htons(local_port);
        l6->sin6_addr = in6addr_any;
        addrlen = sizeof(*p6);
    }

    int fd = socket(peer.ss_family, SOCK_DGRAM, 0);
    if (fd < 0)
        return -1;
    if (bind(fd, (struct sockaddr *)&local, addrlen) < 0 ||
        connect(fd, (struct sockaddr *)&peer, addrlen) < 0) {
        int error = errno;
        close(fd);
        errno = error;
        return -1;
    }
    return fd;
}

int udp_send(int fd, const void *data, size_t size)
{
    ssize_t n;
    do {
        n = send(fd, data, size, 0);
    } while (n < 0 && errno == EINTR);
    return n < 0 ? -1 : (int)n;
}

int udp_recv(int fd, void *data, size_t size)
{
    ssize_t n;
    do {
        n = recv(fd, data, size, 0);
    } while (n < 0 && errno == EINTR);
    return n < 0 ? -1 : (int)n;
}

void udp_close(int fd)
{
    if (fd >= 0)
        close(fd);
}
