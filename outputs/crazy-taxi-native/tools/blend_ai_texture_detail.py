#!/usr/bin/env python3
"""Blend only fine AI-generated texture detail over a faithful bicubic source upscale.

This deliberately rejects generated low-frequency structure so the ROM-derived
material layout, seams, colors, and coarse shapes remain source-led.
"""
import argparse
import hashlib
import json
import math
import struct
import zlib
from pathlib import Path


def read_png(path):
    data = path.read_bytes()
    if not data.startswith(b"\x89PNG\r\n\x1a\n"):
        raise ValueError("expected PNG")
    pos, idat, ihdr = 8, bytearray(), None
    while pos < len(data):
        n = struct.unpack_from(">I", data, pos)[0]
        kind = data[pos + 4:pos + 8]
        chunk = data[pos + 8:pos + 8 + n]
        pos += n + 12
        if kind == b"IHDR":
            ihdr = struct.unpack(">IIBBBBB", chunk)
        elif kind == b"IDAT":
            idat.extend(chunk)
        elif kind == b"IEND":
            break
    w, h, depth, color, compression, filt, interlace = ihdr
    if depth != 8 or color not in (2, 6) or compression or filt or interlace:
        raise ValueError(f"unsupported PNG: {ihdr}")
    channels = 3 if color == 2 else 4
    bpp, stride = channels, w * channels
    raw, rows, previous, offset = zlib.decompress(idat), [], bytearray(stride), 0
    for _ in range(h):
        mode = raw[offset]
        row = bytearray(raw[offset + 1:offset + 1 + stride])
        offset += stride + 1
        for i in range(stride):
            left = row[i - bpp] if i >= bpp else 0
            up = previous[i]
            upper_left = previous[i - bpp] if i >= bpp else 0
            if mode == 1:
                row[i] = (row[i] + left) & 255
            elif mode == 2:
                row[i] = (row[i] + up) & 255
            elif mode == 3:
                row[i] = (row[i] + (left + up) // 2) & 255
            elif mode == 4:
                p = left + up - upper_left
                ds = (abs(p - left), abs(p - up), abs(p - upper_left))
                predictor = left if ds[0] <= ds[1] and ds[0] <= ds[2] else up if ds[1] <= ds[2] else upper_left
                row[i] = (row[i] + predictor) & 255
            elif mode != 0:
                raise ValueError(f"unsupported PNG row filter {mode}")
        rows.append(row)
        previous = row
    rgb, alpha = bytearray(w * h * 3), bytearray(w * h)
    for y, row in enumerate(rows):
        for x in range(w):
            src = x * channels
            dst = (y * w + x) * 3
            rgb[dst:dst + 3] = row[src:src + 3]
            alpha[y * w + x] = row[src + 3] if channels == 4 else 255
    return w, h, rgb, alpha


def chunk(kind, payload):
    return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", zlib.crc32(kind + payload) & 0xffffffff)


def write_png(path, w, h, rgb):
    rows = bytearray()
    stride = w * 3
    for y in range(h):
        rows.append(0)
        rows.extend(rgb[y * stride:(y + 1) * stride])
    payload = (b"\x89PNG\r\n\x1a\n" +
               chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0)) +
               chunk(b"IDAT", zlib.compress(rows, 9)) + chunk(b"IEND", b""))
    path.write_bytes(payload)


def blur_channel(data, w, h, radius):
    horizontal = bytearray(w * h)
    for y in range(h):
        base = y * w
        prefix = [0]
        for x in range(w):
            prefix.append(prefix[-1] + data[base + x])
        for x in range(w):
            lo, hi = max(0, x - radius), min(w - 1, x + radius)
            horizontal[base + x] = (prefix[hi + 1] - prefix[lo]) // (hi - lo + 1)
    output = bytearray(w * h)
    for x in range(w):
        prefix = [0]
        for y in range(h):
            prefix.append(prefix[-1] + horizontal[y * w + x])
        for y in range(h):
            lo, hi = max(0, y - radius), min(h - 1, y + radius)
            output[y * w + x] = (prefix[hi + 1] - prefix[lo]) // (hi - lo + 1)
    return output


def main():
    p = argparse.ArgumentParser()
    p.add_argument("base_bicubic", type=Path)
    p.add_argument("ai_detail_source", type=Path)
    p.add_argument("output", type=Path)
    p.add_argument("report", type=Path)
    p.add_argument("--text-free-approval", type=Path, required=True,
                   help="reviewed manifest that allowlists these exact text-free input image hashes")
    p.add_argument("--radius", type=int, default=28)
    p.add_argument("--strength", type=float, default=0.18)
    p.add_argument("--edge-fade", type=int, default=24)
    a = p.parse_args()
    approval = json.loads(a.text_free_approval.read_text(encoding="utf-8"))
    if approval.get("status") != "APPROVED_TEXT_FREE_DIAGNOSTIC":
        raise SystemExit("refusing upscale: approval manifest is missing or not approved")
    policy = approval.get("text_policy", {})
    if (policy.get("source_contains_text") is not False or
            policy.get("generated_detail_contains_text") is not False or
            policy.get("exclude_text_fonts_logos_signage") is not True or
            policy.get("reviewed_by_human") is not True):
        raise SystemExit("refusing upscale: both inputs must be human-reviewed as text-free")
    expected = approval.get("input_sha256", {})
    actual_base_hash = hashlib.sha256(a.base_bicubic.read_bytes()).hexdigest()
    actual_detail_hash = hashlib.sha256(a.ai_detail_source.read_bytes()).hexdigest()
    if (actual_base_hash != expected.get("source_base") or
            actual_detail_hash != expected.get("generated_detail")):
        raise SystemExit("refusing upscale: input image hash is not in the text-free allowlist")
    w, h, base, base_alpha = read_png(a.base_bicubic)
    aw, ah, detail, detail_alpha = read_png(a.ai_detail_source)
    if (w, h) != (aw, ah) or any(v != 255 for v in base_alpha + detail_alpha):
        raise SystemExit("inputs must be equal-sized, opaque RGB textures")
    blurred = [blur_channel(bytes(detail[c::3]), w, h, a.radius) for c in range(3)]
    output = bytearray(base)
    max_abs = 0
    abs_sum = 0
    for y in range(h):
        for x in range(w):
            distance = min(x, y, w - 1 - x, h - 1 - y)
            fade = min(1.0, distance / max(1, a.edge_fade))
            i = y * w + x
            for c in range(3):
                detail_value = detail[i * 3 + c] - blurred[c][i]
                delta = round(detail_value * a.strength * fade)
                max_abs = max(max_abs, abs(delta))
                abs_sum += abs(delta)
                output[i * 3 + c] = max(0, min(255, base[i * 3 + c] + delta))
    a.output.parent.mkdir(parents=True, exist_ok=True)
    write_png(a.output, w, h, output)
    report = {
        "status": "PASS_AI_HIGH_FREQUENCY_BLEND",
        "text_free_approval_manifest": str(a.text_free_approval),
        "text_exclusion_enforced": True,
        "excluded_content": ["text", "fonts", "logos", "signage", "lettering"],
        "base_bicubic_sha256": hashlib.sha256(a.base_bicubic.read_bytes()).hexdigest(),
        "ai_detail_source_sha256": hashlib.sha256(a.ai_detail_source.read_bytes()).hexdigest(),
        "output_sha256": hashlib.sha256(a.output.read_bytes()).hexdigest(),
        "dimensions": [w, h], "blur_radius": a.radius,
        "detail_strength": a.strength, "edge_fade_pixels": a.edge_fade,
        "mean_absolute_rgb_change_from_bicubic_base": round(abs_sum / (w * h * 3), 4),
        "maximum_channel_change": max_abs,
        "interpretation": "Only low-amplitude high-frequency AI detail is blended over the source bicubic upscale. Large generated structure is removed by the blur difference; added detail fades out at tile edges. This does not prove style match, tile seamlessness, or gameplay integration."
    }
    a.report.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
