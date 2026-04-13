import argparse
import json
import math
import socket
import sys
import time
from typing import Callable, Iterable, Optional

try:
    import serial  # type: ignore
except ImportError:
    serial = None


def generate_payloads(
    center_x: float,
    center_y: float,
    radius: float,
    depth_base: float,
    depth_amplitude: float,
    rate_hz: float,
    count: int,
) -> Iterable[bytes]:
    interval = 1.0 / rate_hz
    for index in range(count):
        phase = index * 0.18
        x = center_x + math.cos(phase) * radius
        y = center_y + math.sin(phase) * (radius * 0.65)
        depth = depth_base + (math.sin(phase * 0.7) * depth_amplitude)
        payload = json.dumps({"x": round(x, 3), "y": round(y, 3), "depthMm": round(depth, 3)}) + "\n"
        yield payload.encode("utf-8")
        time.sleep(interval)


def send_tcp(host: str, port: int, payloads: Iterable[bytes]) -> None:
    with socket.create_connection((host, port), timeout=5.0) as sock:
        for payload in payloads:
            sock.sendall(payload)


def send_serial(port: str, baud_rate: int, payloads: Iterable[bytes]) -> None:
    if serial is None:
        raise RuntimeError(
            "pyserial is required for serial mode. Install it with: "
            'python -m pip install -r scripts/requirements-demo.txt'
        )

    with serial.Serial(port=port, baudrate=baud_rate, timeout=1) as device:
        for payload in payloads:
            device.write(payload)
            device.flush()


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Stream demo telemetry into SurgiView over TCP or serial.")
    parser.add_argument("--mode", choices=["tcp", "serial"], default="tcp")
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=45454, help="TCP port")
    parser.add_argument("--serial-port", default="COM3")
    parser.add_argument("--baud-rate", type=int, default=115200)
    parser.add_argument("--rate-hz", type=float, default=20.0)
    parser.add_argument("--count", type=int, default=300)
    parser.add_argument("--center-x", type=float, default=220.0)
    parser.add_argument("--center-y", type=float, default=170.0)
    parser.add_argument("--radius", type=float, default=65.0)
    parser.add_argument("--depth-base", type=float, default=12.0)
    parser.add_argument("--depth-amplitude", type=float, default=4.5)
    return parser


def main() -> int:
    parser = build_parser()
    args = parser.parse_args()

    if args.rate_hz <= 0.0:
        parser.error("--rate-hz must be > 0")
    if args.count <= 0:
        parser.error("--count must be > 0")

    payloads = generate_payloads(
        center_x=args.center_x,
        center_y=args.center_y,
        radius=args.radius,
        depth_base=args.depth_base,
        depth_amplitude=args.depth_amplitude,
        rate_hz=args.rate_hz,
        count=args.count,
    )

    sender: Callable[..., None]
    try:
        if args.mode == "tcp":
            print(f"Sending {args.count} telemetry samples to tcp://{args.host}:{args.port}")
            sender = send_tcp
            sender(args.host, args.port, payloads)
        else:
            print(f"Sending {args.count} telemetry samples to serial://{args.serial_port} @ {args.baud_rate}")
            sender = send_serial
            sender(args.serial_port, args.baud_rate, payloads)
    except OSError as exc:
        print(f"Telemetry send failed: {exc}", file=sys.stderr)
        return 1
    except RuntimeError as exc:
        print(str(exc), file=sys.stderr)
        return 1

    print("Telemetry stream completed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
