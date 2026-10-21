"""Receive datagrams sent by ``server.py``."""

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from udp import udp_close, udp_init, udp_recv


def main() -> None:
    # Keep the same ports and direction as test/client.cpp.
    fd = udp_init("127.0.0.1", 9001, 9000)
    buffer = bytearray(65536)
    try:
        while True:
            size = udp_recv(fd, buffer)
            data = bytes(buffer[:size])
            print(f"recv {size} bytes, data: {data.decode(errors='replace')}", flush=True)
    except KeyboardInterrupt:
        pass
    finally:
        udp_close(fd)


if __name__ == "__main__":
    main()
