#!/usr/bin/env python3
"""Count CTM6 City1 textured-face references for safe upscale prioritization."""
from collections import Counter
from pathlib import Path
import hashlib
import json
import struct

ROOT = Path(__file__).resolve().parents[1]
ASSET = ROOT / "assets/city1.ctm"
ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
CTM_SHA256 = "fbcafe5e6861feff4e58f039a03c5461913989c869d895ff3dc04bbb819c3ac3"


def u32(data, offset):
    return struct.unpack_from("<I", data, offset)[0]


def main():
    data = ASSET.read_bytes()
    assert hashlib.sha256(data).hexdigest() == CTM_SHA256
    assert data[:4] == b"CTM6"
    vertices, faces, textures, materials = struct.unpack_from("<4I", data, 4)
    assert (vertices, faces, textures, materials) == (14250, 9880, 149, 240)
    offset = 32 + vertices * 12
    face_data = data[offset : offset + faces * 44]
    assert len(face_data) == faces * 44
    offset += faces * 44

    material_rows = []
    for index in range(materials):
        base = offset + index * 12
        material_rows.append({"texture": u32(data, base), "flags": u32(data, base + 8)})
    offset += materials * 12

    texture_rows = []
    for index in range(textures):
        width, height = struct.unpack_from("<II", data, offset)
        offset += 8
        size = width * height * 4
        assert width and height and width <= 256 and height <= 256 and offset + size <= len(data)
        texture_rows.append({"texture": index, "width": width, "height": height, "rgba_sha256": hashlib.sha256(data[offset : offset + size]).hexdigest()})
        offset += size
    assert offset == len(data)

    approved = {}
    approval_specs = {
        33: ("city1-tile-33-text-free-approved", "raw_texture_png_sha256", "source-original-32.png"),
        141: ("city1-tile-141-text-free-approved", "raw_texture_rgba_sha256", None),
    }
    for texture, (folder, source_hash_key, source_png) in approval_specs.items():
        approval_path = ROOT / "assets/texture-ai-experiments" / folder / "text-free-approval.json"
        approval = json.loads(approval_path.read_text(encoding="utf-8"))
        assert approval["status"] == "APPROVED_TEXT_FREE_DIAGNOSTIC"
        assert approval["ctm6_sha256"] == CTM_SHA256
        assert approval["text_policy"]["source_contains_text"] is False
        assert approval["text_policy"]["generated_detail_contains_text"] is False
        assert approval["text_policy"]["exclude_text_fonts_logos_signage"] is True
        assert approval["text_policy"]["reviewed_by_human"] is True
        if source_png:
            source_hash = hashlib.sha256((approval_path.parent / source_png).read_bytes()).hexdigest()
        else:
            source_hash = texture_rows[texture]["rgba_sha256"]
        assert source_hash == approval[source_hash_key]
        approved[texture] = {"manifest": str(approval_path.relative_to(ROOT)), "source_sha256": source_hash}

    all_face_uses = Counter()
    visible_face_uses = Counter()
    hidden_face_uses = Counter()
    for index in range(faces):
        face_offset = index * 44
        vertex_count, packed_flags = struct.unpack_from("<HH", face_data, face_offset)
        material_index = packed_flags & 0xFF
        assert 3 <= vertex_count <= 4 and material_index < materials
        material = material_rows[material_index]
        texture = material["texture"]
        if texture == 255:
            continue
        assert texture < textures
        all_face_uses[texture] += 1
        (hidden_face_uses if material["flags"] & 9 else visible_face_uses)[texture] += 1

    rows = []
    for texture in range(textures):
        rows.append({
            **texture_rows[texture],
            "visible_face_count": visible_face_uses[texture],
            "hidden_or_special_face_count": hidden_face_uses[texture],
            "total_face_count": all_face_uses[texture],
            "text_status": "APPROVED_TEXT_FREE_DIAGNOSTIC" if texture in approved else "UNREVIEWED_KEEP_OUT_OF_UPSCALE",
        })
    rows.sort(key=lambda row: (-row["visible_face_count"], row["texture"]))
    report = {
        "status": "PASS_CITY1_TEXTURE_USAGE_INDEX",
        "source_rom_sha256": ROM_SHA256,
        "ctm6_sha256": CTM_SHA256,
        "geometry": {"vertices": vertices, "faces": faces, "textures": textures, "materials": materials},
        "count_rule": "Each CTM6 face contributes once to its referenced texture when its low-byte material index resolves to a texture. Material flags&9 are separated as hidden/special exactly as detail_view filters them; this is geometry reference count, not screen-pixel coverage.",
        "review_policy": "Only the explicitly listed texture33 and texture141 source pixels reference exact-hash text-free approvals; every other texture remains unreviewed and must stay out of any generated/upscaled pack until its pixels are reviewed and exact source/detail hashes are approved. This audit performs no image processing and makes no new text-free claim.",
        "approved_text_free_textures": [{"index": texture, **info, "visible_face_count": visible_face_uses[texture]} for texture, info in sorted(approved.items())],
        "visual_contact_sheet": "work/city1-texture-use-contact-sheet.png (diagnostic only; labels are not game assets)",
        "active_textured_face_total": sum(visible_face_uses.values()),
        "textures_ranked_by_visible_face_use": rows,
    }
    output = ROOT / "reports/city1-texture-usage-audit.json"
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "output": str(output), "top_textures": rows[:12], "active_textured_face_total": report["active_textured_face_total"]}, indent=2))


if __name__ == "__main__":
    main()
