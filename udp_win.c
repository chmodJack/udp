#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "udp_win.h"
#include <limits.h>
#include <ws2tcpip.h>

#ifdef _MSC_VER
#pragma comment(lib, "Ws2_32.lib")
#endif

static int udp_error(int error)
{
    WSASetLastError(error);
    return SOCKET_ERROR;
}

static SOCKET udp_init_error(int error)
{
    WSACleanup();
    WSASetLastError(error);
    return INVALID_SOCKET;
}

SOCKET udp_init(const char *peer_ip, unsigned short peer_port,
                unsigned short local_port)
{
    WSADATA wsa_data;
    int error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (error != 0) {
        WSASetLastError(error);
        return INVALID_SOCKET;
    }

    if (!peer_ip || !peer_port || !local_port)
        return udp_init_error(WSAEINVAL);

    struct sockaddr_storage peer = {0}, local = {0};
    int addrlen;
    struct sockaddr_in *p4 = (struct sockaddr_in *)&peer;
    struct sockaddr_in *l4 = (struct sockaddr_in *)&local;
    if (InetPtonA(AF_INET, peer_ip, &p4->sin_addr) == 1) {
        p4->sin_family = l4->sin_family = AF_INET;
        p4->sin_port = htons(peer_port);
        l4->sin_port = htons(local_port);
        l4->sin_addr.s_addr = htonl(INADDR_ANY);
        addrlen = (int)sizeof(*p4);
    } else {
        struct sockaddr_in6 *p6 = (struct sockaddr_in6 *)&peer;
        struct sockaddr_in6 *l6 = (struct sockaddr_in6 *)&local;
        if (InetPtonA(AF_INET6, peer_ip, &p6->sin6_addr) != 1)
            return udp_init_error(WSAEINVAL);
        p6->sin6_family = l6->sin6_family = AF_INET6;
        p6->sin6_port = htons(peer_port);
        l6->sin6_port = htons(local_port);
        l6->sin6_addr = in6addr_any;
        addrlen = (int)sizeof(*p6);
    }

    SOCKET native_socket = socket(peer.ss_family, SOCK_DGRAM, IPPROTO_UDP);
    if (native_socket == INVALID_SOCKET) {
        error = WSAGetLastError();
        return udp_init_error(error);
    }
    if (bind(native_socket, (struct sockaddr *)&local, addrlen) == SOCKET_ERROR ||
        connect(native_socket, (struct sockaddr *)&peer, addrlen) == SOCKET_ERROR) {
        error = WSAGetLastError();
        closesocket(native_socket);
        return udp_init_error(error);
    }

    return native_socket;
}

int udp_send(SOCKET fd, const void *data, size_t size)
{
    if (size > INT_MAX)
        return udp_error(WSAEMSGSIZE);

    int n;
    do {
        n = send(fd, (const char *)data, (int)size, 0);
        if (n == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error != WSAEINTR)
                return udp_error(error);
        }
    } while (n == SOCKET_ERROR);
    return n;
}

int udp_recv(SOCKET fd, void *data, size_t size)
{
    if (size > INT_MAX)
        size = INT_MAX;

    int n;
    do {
        n = recv(fd, (char *)data, (int)size, 0);
        if (n == SOCKET_ERROR) {
            int error = WSAGetLastError();
            if (error != WSAEINTR)
                return udp_error(error);
        }
    } while (n == SOCKET_ERROR);
    return n;
}

void udp_close(SOCKET fd)
{
    if (fd != INVALID_SOCKET) {
        closesocket(fd);
        WSACleanup();
    }
}
