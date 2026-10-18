#!/usr/bin/env python3
"""Extract an exact RGBA City 1/2 CTM6 texture to PNG for visual review."""
import argparse
import hashlib
import struct
import zlib
from pathlib import Path

CTM_SHA256 = {
    "city1": "fbcafe5e6861feff4e58f039a03c5461913989c869d895ff3dc04bbb819c3ac3",
    "city2": "098e772c426570a766ed281df0dcb4faa96fbb9f6f339b6b2b483c1c2ca235fe",
}

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
    parser.add_argument("scene", choices=("city1", "city2"))
    parser.add_argument("index", type=int)
    parser.add_argument("output", type=Path)
    parser.add_argument("--project", type=Path, default=Path(__file__).resolve().parents[1])
    args = parser.parse_args()
    asset = args.project / "assets" / f"{args.scene}.ctm"
    data = asset.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    if digest != CTM_SHA256[args.scene] or data[:4] != b"CTM6":
        raise SystemExit("refusing extraction: scene CTM6 hash/revision mismatch")
    vertices, faces, texture_count, materials = struct.unpack_from("<4I", data, 4)
    if not 0 <= args.index < texture_count:
        raise SystemExit("texture index outside scene CTM6 table")
    offset = 32 + vertices * 12 + faces * 44 + materials * 12
    for index in range(texture_count):
        width, height = struct.unpack_from("<II", data, offset)
        offset += 8
        size = width * height * 4
        pixels = data[offset:offset + size]
        if not width or not height or len(pixels) != size:
            raise SystemExit(f"invalid/truncated CTM6 texture payload {index}")
        if index == args.index:
            write_png(args.output, width, height, pixels)
            print(f"PASS exact {args.scene} texture{index}: {width}x{height} RGBA, pixel_sha256={hashlib.sha256(pixels).hexdigest()}, ctm6_sha256={digest}, png={args.output}")
            return
        offset += size
    raise SystemExit("texture not found")

if __name__ == "__main__":
    main()