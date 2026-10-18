#!/usr/bin/env python3
"""Read-only PNG color and wrap-edge audit for a generated material candidate."""
import argparse
import hashlib
import json
import struct
import zlib
from pathlib import Path


def read_png(path):
    data = path.read_bytes()
    if not data.startswith(b"\x89PNG\r\n\x1a\n"):
        raise ValueError(f"not PNG: {path}")
    pos, compressed, header = 8, bytearray(), None
    while pos < len(data):
        size = struct.unpack_from(">I", data, pos)[0]
        kind = data[pos + 4:pos + 8]
        chunk = data[pos + 8:pos + 8 + size]
        pos += size + 12
        if kind == b"IHDR":
            header = struct.unpack(">IIBBBBB", chunk)
        elif kind == b"IDAT":
            compressed.extend(chunk)
        elif kind == b"IEND":
            break
    width, height, depth, color, compression, filt, interlace = header
    if depth != 8 or color not in (2, 6) or compression or filt or interlace:
        raise ValueError(f"unsupported PNG format: {header}")
    channels = 3 if color == 2 else 4
    bpp = channels
    stride = width * channels
    raw = zlib.decompress(compressed)
    rows, previous, offset = [], bytearray(stride), 0
    for _ in range(height):
        mode = raw[offset]
        scan = bytearray(raw[offset + 1:offset + 1 + stride])
        offset += stride + 1
        for i in range(stride):
            left = scan[i - bpp] if i >= bpp else 0
            up = previous[i]
            upper_left = previous[i - bpp] if i >= bpp else 0
            if mode == 1:
                scan[i] = (scan[i] + left) & 255
            elif mode == 2:
                scan[i] = (scan[i] + up) & 255
            elif mode == 3:
                scan[i] = (scan[i] + ((left + up) // 2)) & 255
            elif mode == 4:
                p = left + up - upper_left
                pa, pb, pc = abs(p - left), abs(p - up), abs(p - upper_left)
                predictor = left if pa <= pb and pa <= pc else up if pb <= pc else upper_left
                scan[i] = (scan[i] + predictor) & 255
            elif mode != 0:
                raise ValueError(f"unsupported PNG filter {mode}")
        rows.append(scan)
        previous = scan
    pixels = [tuple(rows[y][x * channels:x * channels + 3])
              for y in range(height) for x in range(width)]
    n = len(pixels)
    means = [sum(p[c] for p in pixels) / n for c in range(3)]
    edges = []
    for y in range(height):
        edges.append((rows[y][:3], rows[y][-channels:]))
    for x in range(width):
        edges.append((rows[0][x * channels:x * channels + 3],
                      rows[-1][x * channels:x * channels + 3]))
    edge_mae = sum(sum(abs(a[c] - b[c]) for c in range(3)) for a, b in edges) / (3 * len(edges))
    return {"path": str(path), "sha256": hashlib.sha256(data).hexdigest(),
            "width": width, "height": height, "color_type": color,
            "mean_rgb": [round(x, 2) for x in means],
            "opposite_edge_rgb_mae": round(edge_mae, 2)}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    report = {"source": read_png(args.source), "candidate": read_png(args.candidate)}
    report["mean_rgb_absolute_difference"] = round(sum(
        abs(a - b) for a, b in zip(report["source"]["mean_rgb"],
                                   report["candidate"]["mean_rgb"])) / 3, 2)
    report["warning"] = "Edge MAE is a first-pass seam risk signal, not proof of seamless tiling or visual fidelity. Human inspection and in-engine repetition are still required."
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
