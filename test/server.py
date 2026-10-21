"""Send a datagram every second, like ``server.cpp``."""

from pathlib import Path
import sys
import time

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from udp import udp_close, udp_init, udp_send


def main() -> None:
    # Keep the same ports and direction as test/server.cpp.
    fd = udp_init("127.0.0.1", 9000, 9001)
    sequence = 0
    try:
        while True:
            data = f"{sequence:04d} udp test\n".encode()
            udp_send(fd, data, len(data))
            sequence += 1
            time.sleep(1)
    except KeyboardInterrupt:
        pass
    finally:
        udp_close(fd)


if __name__ == "__main__":
    main()
