#!/usr/bin/env python3
"""Extract one exact RGBA City 1 CTM6 texture to PNG for visual review."""
import argparse
import hashlib
import struct
import zlib
from pathlib import Path

EXPECTED_SHA256 = "fbcafe5e6861feff4e58f039a03c5461913989c869d895ff3dc04bbb819c3ac3"

def chunk(kind, payload):
    return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", zlib.crc32(kind + payload) & 0xffffffff)

def write_png(path, width, height, rgba):
    rows = b"".join(b"\0" + rgba[y * width * 4:(y + 1) * width * 4] for y in range(height))
    png = (b"\x89PNG\r\n\x1a\n" +
           chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0)) +
           chunk(b"IDAT", zlib.compress(rows, 9)) + chunk(b"IEND", b""))
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(png)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("asset", type=Path)
    parser.add_argument("index", type=int)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    data = args.asset.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != EXPECTED_SHA256 or data[:4] != b"CTM6":
        raise SystemExit("refusing extraction: unexpected City 1 scene asset")
    vertices, faces, texture_count, materials = struct.unpack_from("<4I", data, 4)
    if (vertices, faces, texture_count, materials) != (14250, 9880, 149, 240):
        raise SystemExit("refusing extraction: City 1 CTM6 layout changed")
    if not 0 <= args.index < texture_count:
        raise SystemExit("texture index outside City 1 CTM6 table")
    offset = 32 + vertices * 12 + faces * 44 + materials * 12
    for index in range(texture_count):
        width, height = struct.unpack_from("<II", data, offset)
        offset += 8
        size = width * height * 4
        pixels = data[offset:offset + size]
        if len(pixels) != size:
            raise SystemExit("truncated City 1 texture payload")
        if index == args.index:
            write_png(args.output, width, height, pixels)
            print(f"PASS exact source texture{index}: {width}x{height} RGBA, sha256={hashlib.sha256(pixels).hexdigest()}, rom-derived CTM6 sha256={digest}, png={args.output}")
            return
        offset += size
    raise SystemExit("texture not found")

if __name__ == "__main__":
    main()