"""Small connected-UDP wrapper matching the C implementation in ``udp.c``.

``udp_init`` returns a :class:`socket.socket`; this is Python's equivalent of
the C file descriptor.  The other functions also accept a raw descriptor when
one is supplied by existing C/Python code.
"""

from __future__ import annotations

import socket
from typing import ByteString

__all__ = ["udp_init", "udp_send", "udp_recv", "udp_close"]


def _socket_from_handle(handle: socket.socket | int) -> tuple[socket.socket, bool]:
    """Return a socket and whether this call temporarily owns its wrapper."""

    if isinstance(handle, socket.socket):
        return handle, False
    if isinstance(handle, int):
        # socket.socket(fileno=...) takes ownership: detach it before returning
        # so the temporary object cannot close the caller's descriptor.
        sock = socket.socket(fileno=handle)
        return sock, True
    raise TypeError("fd must be a socket.socket or an integer file descriptor")


def udp_init(peer_ip: str, peer_port: int, local_port: int) -> socket.socket:
    """Bind *local_port* and connect a UDP socket to numeric *peer_ip*.

    Both ports must be in ``1..65535``.  Only numeric IPv4 and IPv6 addresses
    are accepted, just like ``inet_pton`` in ``udp.c``.
    """

    if not peer_ip or not 1 <= peer_port <= 65535 or not 1 <= local_port <= 65535:
        raise ValueError("peer_ip and non-zero ports are required")

    try:
        socket.inet_pton(socket.AF_INET, peer_ip)
        family = socket.AF_INET
        peer = (peer_ip, peer_port)
        local = ("0.0.0.0", local_port)
    except (OSError, TypeError):
        try:
            socket.inet_pton(socket.AF_INET6, peer_ip)
        except (OSError, TypeError) as error:
            raise ValueError("peer_ip must be a numeric IPv4 or IPv6 address") from error
        family = socket.AF_INET6
        peer = (peer_ip, peer_port)
        local = ("::", local_port)

    sock = socket.socket(family, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    try:
        sock.bind(local)
        sock.connect(peer)
    except Exception:
        sock.close()
        raise
    return sock


def udp_send(
    fd: socket.socket | int, data: ByteString, size: int | None = None
) -> int:
    """Send one datagram and return the number of bytes sent.

    ``size`` defaults to the complete buffer and is retained for source-level
    compatibility with ``udp_send(fd, data, size)`` in C.
    """

    payload = memoryview(data)
    if size is None:
        size = len(payload)
    if not 0 <= size <= len(payload):
        raise ValueError("size must be between zero and the buffer length")

    sock, temporary = _socket_from_handle(fd)
    try:
        return sock.send(payload[:size])
    finally:
        if temporary:
            sock.detach()


def udp_recv(
    fd: socket.socket | int, data: bytearray | memoryview | int, size: int | None = None
) -> int | bytes:
    """Receive one datagram.

    With a writable buffer, return its byte count like ``udp_recv`` in C.  If
    *data* is an integer and *size* is omitted, treat it as the receive buffer
    size and return the received :class:`bytes`.
    """

    sock, temporary = _socket_from_handle(fd)
    try:
        if size is None:
            if isinstance(data, int):
                if data < 0:
                    raise ValueError("size must be non-negative")
                return sock.recv(data)
            buffer = memoryview(data)
            if buffer.readonly:
                raise TypeError("data must be a writable buffer")
            return sock.recv_into(buffer)
        if size < 0:
            raise ValueError("size must be non-negative")
        buffer = memoryview(data)
        if buffer.readonly:
            raise TypeError("data must be a writable buffer")
        if size > len(buffer):
            raise ValueError("size exceeds the buffer length")
        return sock.recv_into(buffer, size)
    finally:
        if temporary:
            sock.detach()


def udp_close(fd: socket.socket | int) -> None:
    """Close a socket or a raw file descriptor; ignore the C sentinel ``-1``."""

    if isinstance(fd, socket.socket):
        fd.close()
    elif isinstance(fd, int):
        if fd >= 0:
            try:
                socket.close(fd)
            except OSError:
                pass
    else:
        raise TypeError("fd must be a socket.socket or an integer file descriptor")
