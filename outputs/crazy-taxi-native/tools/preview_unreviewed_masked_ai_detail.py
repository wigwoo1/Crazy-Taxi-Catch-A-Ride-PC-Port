#!/usr/bin/env python3
"""Make a work-only masked AI detail preview; never changes runtime assets."""
import argparse
import hashlib
import json
from pathlib import Path

import numpy as np
from PIL import Image, ImageFilter

SOURCE_RGBA_SHA256 = "501fded4c78c3e6d694b0f153b09c36c5872adb56a26724a4534e3e7665a1e69"

def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
    p = argparse.ArgumentParser()
    p.add_argument("source", type=Path)
    p.add_argument("detail", type=Path)
    p.add_argument("--project", type=Path, default=Path(__file__).resolve().parents[1])
    p.add_argument("--output", type=Path, default=Path("work/city2-texture139-masked-detail-preview.png"))
    p.add_argument("--report", type=Path, default=Path("reports/city2-texture139-masked-detail-preview.json"))
    p.add_argument("--strength", type=float, default=0.08)
    p.add_argument("--blur-radius", type=float, default=48)
    p.add_argument("--edge-fade", type=int, default=64)
    a = p.parse_args()
    project = a.project.resolve()
    output = (project / a.output).resolve() if not a.output.is_absolute() else a.output.resolve()
    report_path = (project / a.report).resolve() if not a.report.is_absolute() else a.report.resolve()
    if not output.is_relative_to(project / "work") or not report_path.is_relative_to(project / "reports"):
        raise SystemExit("diagnostic preview and report must remain under work/ and reports/")
    source_rgba = Image.open(a.source).convert("RGBA")
    if source_rgba.size != (128, 64):
        raise SystemExit("expected exact City 2 texture139 source dimensions 128x64")
    source_bytes = source_rgba.tobytes()
    pixel_hash = hashlib.sha256(source_bytes).hexdigest()
    if pixel_hash != SOURCE_RGBA_SHA256:
        raise SystemExit("source pixels do not match reviewed City 2 texture139 CTM6 material")
    if not (0 < a.strength <= 0.25 and 1 <= a.edge_fade <= 256 and 1 <= a.blur_radius <= 256):
        raise SystemExit("diagnostic blend parameters outside safe range")
    src = np.asarray(source_rgba.convert("RGB"), dtype=np.uint8)
    r, g, b = (src[:, :, c].astype(np.int32) for c in range(3))
    luma = (299 * r + 587 * g + 114 * b) // 1000
    chroma = np.maximum(np.maximum(r, g), b) - np.minimum(np.minimum(r, g), b)
    xs = np.arange(128)[None, :]
    # Only muted mid-gray road interior; white/yellow paint and outer shoulders stay out.
    mask = (xs >= 20) & (xs <= 95) & (chroma <= 18) & (luma >= 95) & (luma <= 185)
    bright_or_colored_paint = ((luma > 210) | (chroma > 40))
    if not np.any(mask):
        raise SystemExit("asphalt mask selected no source pixels; refusing empty preview")
    paint_masked = int(np.count_nonzero(mask & bright_or_colored_paint))
    if paint_masked:
        raise SystemExit(f"road paint mask safety failed: {paint_masked} painted source pixels selected")
    width, height = 4096, 2048
    base_image = source_rgba.convert("RGB").resize((width, height), Image.Resampling.BICUBIC)
    detail = Image.open(a.detail).convert("RGB").resize((width, height), Image.Resampling.LANCZOS)
    blurred = detail.filter(ImageFilter.GaussianBlur(radius=a.blur_radius))
    base = np.asarray(base_image, dtype=np.int16)
    detail_array = np.asarray(detail, dtype=np.int16)
    low = np.asarray(blurred, dtype=np.int16)
    high = detail_array - low
    yy, xx = np.mgrid[0:height, 0:width]
    edge_distance = np.minimum.reduce((xx, yy, width - 1 - xx, height - 1 - yy))
    fade = np.clip(edge_distance / float(a.edge_fade), 0.0, 1.0)
    mask_big = np.asarray(Image.fromarray(mask.astype(np.uint8) * 255).resize(
        (width, height), Image.Resampling.NEAREST)) > 0
    delta = np.rint(high.astype(np.float32) * a.strength * fade[:, :, None]).astype(np.int16)
    result = np.clip(base + delta * mask_big[:, :, None], 0, 255).astype(np.uint8)
    outside_changed = int(np.count_nonzero(np.any(result != base.astype(np.uint8), axis=2) & ~mask_big))
    difference = np.abs(result.astype(np.int16) - base.astype(np.uint8).astype(np.int16))
    changed_mask_pixels = int(np.count_nonzero(np.any(difference > 0, axis=2) & mask_big))
    mean_detail_delta = round(float(difference.mean()), 4)
    maximum_detail_delta = int(difference.max())
    baseline_path = project / "work" / "city2-texture139-bicubic-baseline-4096x2048.png"
    base_image.save(baseline_path)
    if outside_changed:
        raise SystemExit(f"paint/source preservation failed: {outside_changed} outside-mask pixels changed")
    output.parent.mkdir(parents=True, exist_ok=True)
    Image.fromarray(result, "RGB").save(output, optimize=True)
    report = {
        "status": "PASS_UNREVIEWED_WORK_ONLY_MASKED_DETAIL_PREVIEW",
        "review_status": "UNREVIEWED_NOT_FOR_RUNTIME",
        "source_rgba_sha256": pixel_hash,
        "detail_sha256": sha(a.detail),
        "output_sha256": sha(output),
        "dimensions": [width, height],
        "mask": "Source texture139 interior x20..95, low-chroma gray luma95..185; yellow/white road paint and outer shoulders excluded.",
        "source_mask_pixels": int(np.count_nonzero(mask)),
        "source_paint_pixels_selected": paint_masked,
        "outside_mask_changed_pixels": outside_changed,
        "changed_mask_pixels": changed_mask_pixels,
        "mean_absolute_rgb_delta": mean_detail_delta,
        "maximum_channel_delta": maximum_detail_delta,
        "baseline_path": str(baseline_path.relative_to(project)).replace("\\", "/"),
        "blend": {"high_pass_blur_radius": a.blur_radius, "strength": a.strength, "edge_fade_pixels": a.edge_fade},
        "text_and_markings_policy": "No text is present in the source road tile; lane paint is excluded by chroma/luma mask and pixels outside the mask match the bicubic source upscale exactly.",
        "scope": "Diagnostic preview only; source ROM/CTM and game runtime are unchanged. Human style/text review and runtime integration are pending."
    }
    report_path.parent.mkdir(parents=True, exist_ok=True)
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))

if __name__ == "__main__":
    main()