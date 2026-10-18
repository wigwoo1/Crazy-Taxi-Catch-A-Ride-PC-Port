#!/usr/bin/env python3
"""Audit CTM6 scene texture usage and inherit only exact reviewed manifests."""
import argparse
from collections import Counter
import hashlib
import json
import re
import struct
from pathlib import Path

EXPECTED_ROM = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"

def u32(data, offset):
    return struct.unpack_from("<I", data, offset)[0]

def audit(asset, rom, scene, project, output):
    rom_hash = hashlib.sha256(rom.read_bytes()).hexdigest()
    if rom_hash != EXPECTED_ROM:
        raise SystemExit("refusing audit: ROM revision hash does not match the supplied USA image")
    data = asset.read_bytes()
    if data[:4] != b"CTM6":
        raise SystemExit("refusing audit: expected the extracted CTM6 scene asset")
    asset_hash = hashlib.sha256(data).hexdigest()
    vertices, faces, texture_count, material_count = struct.unpack_from("<4I", data, 4)
    offset = 32 + vertices * 12 + faces * 44
    if offset + material_count * 12 > len(data):
        raise SystemExit("truncated CTM6 materials")
    materials = []
    for index in range(material_count):
        row = offset + index * 12
        materials.append({"texture": u32(data, row), "flags": u32(data, row + 8)})
    offset += material_count * 12
    textures = []
    for index in range(texture_count):
        if offset + 8 > len(data):
            raise SystemExit("truncated CTM6 texture header")
        width, height = struct.unpack_from("<II", data, offset)
        offset += 8
        size = width * height * 4
        pixels = data[offset:offset + size]
        if not width or not height or width > 256 or height > 256 or len(pixels) != size:
            raise SystemExit(f"invalid CTM6 texture payload {index}")
        textures.append({"index": index, "width": width, "height": height,
                         "rgba_sha256": hashlib.sha256(pixels).hexdigest()})
        offset += size
    if offset != len(data):
        raise SystemExit("unexpected trailing CTM6 bytes")
    visible, hidden = Counter(), Counter()
    face_data = data[32 + vertices * 12 : 32 + vertices * 12 + faces * 44]
    for index in range(faces):
        pos = index * 44
        vertex_count, packed = struct.unpack_from("<HH", face_data, pos)
        material_index = packed & 0xff
        if vertex_count < 3 or vertex_count > 4 or material_index >= material_count:
            raise SystemExit(f"invalid CTM6 face {index}")
        material = materials[material_index]
        texture = material["texture"]
        if texture == 255:
            continue
        if texture >= texture_count:
            raise SystemExit(f"invalid texture index on face {index}")
        (hidden if material["flags"] & 9 else visible)[texture] += 1
    approved = {}
    experiment_root = project / "assets" / "texture-ai-experiments"
    for manifest_path in experiment_root.glob(f"{scene}-tile-*-text-free-approved/text-free-approval.json"):
        match = re.search(r"-tile-(\d+)-text-free-approved$", manifest_path.parent.name)
        if not match:
            continue
        index = int(match.group(1))
        if index >= texture_count:
            raise SystemExit(f"approved manifest index outside {scene} CTM6: {index}")
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
        policy = manifest.get("text_policy", {})
        if (manifest.get("status") != "APPROVED_TEXT_FREE_DIAGNOSTIC" or
                manifest.get("rom_sha256") != rom_hash or
                manifest.get("ctm6_sha256") != asset_hash or
                policy.get("source_contains_text") is not False or
                policy.get("generated_detail_contains_text") is not False or
                policy.get("exclude_text_fonts_logos_signage") is not True or
                policy.get("reviewed_by_human") is not True):
            raise SystemExit(f"approved text-free manifest failed policy/source checks: {manifest_path}")
        rgba_hash = manifest.get("raw_texture_rgba_sha256")
        if rgba_hash and rgba_hash != textures[index]["rgba_sha256"]:
            raise SystemExit(f"approved RGBA source hash mismatch for texture{index}")
        png_hash = manifest.get("raw_texture_png_sha256")
        if png_hash:
            source_png = manifest_path.parent / "source-original-32.png"
            if not source_png.is_file() or hashlib.sha256(source_png.read_bytes()).hexdigest() != png_hash:
                raise SystemExit(f"approved PNG source hash mismatch for texture{index}")
        if not rgba_hash and not png_hash:
            raise SystemExit(f"approved manifest lacks an exact source image hash: {manifest_path}")
        approved[index] = {"manifest": str(manifest_path.relative_to(project)).replace("\\", "/"),
                           "rgba_sha256": textures[index]["rgba_sha256"],
                           "visible_face_count": visible[index]}
    rows = []
    for row in textures:
        index = row["index"]
        rows.append({**row, "visible_face_count": visible[index],
                     "hidden_or_special_face_count": hidden[index],
                     "total_face_count": visible[index] + hidden[index],
                     "text_status": "APPROVED_TEXT_FREE_DIAGNOSTIC" if index in approved else "UNREVIEWED_KEEP_OUT_OF_UPSCALE"})
    rows.sort(key=lambda row: (-row["visible_face_count"], row["index"]))
    result = {
        "status": "PASS_SCENE_TEXTURE_USAGE_INDEX",
        "scene": scene,
        "source_rom_sha256": rom_hash,
        "ctm6_sha256": asset_hash,
        "geometry": {"vertices": vertices, "faces": faces,
                     "textures": texture_count, "materials": material_count},
        "count_rule": "Each CTM6 face contributes once to its referenced texture. Material flags&9 are separated as hidden/special; face counts are not screen-pixel coverage.",
        "review_policy": "This audit reads indices, dimensions, hashes, face references, and pre-existing approval manifests. It inspects no new pixels and makes no new text-free claim. Unapproved textures stay excluded from generated/upscaled assets.",
        "approved_text_free_textures": [{"index": index, **info} for index, info in sorted(approved.items())],
        "active_textured_face_total": sum(visible.values()),
        "textures_ranked_by_visible_face_use": rows,
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": result["status"], "scene": scene,
                      "rom_sha256": rom_hash, "ctm6_sha256": asset_hash,
                      "geometry": result["geometry"],
                      "approved_text_free_textures": result["approved_text_free_textures"],
                      "top_textures": rows[:12],
                      "active_textured_face_total": result["active_textured_face_total"],
                      "output": str(output)}, indent=2))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("scene", choices=("city1", "city2"))
    parser.add_argument("--project", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--output", type=Path)
    args = parser.parse_args()
    asset = args.project / "assets" / f"{args.scene}.ctm"
    output = args.output or (args.project / "reports" / f"{args.scene}-texture-usage-audit.json")
    audit(asset, args.rom, args.scene, args.project, output)

if __name__ == "__main__":
    main()