#!/usr/bin/env python3
"""Replay one clean counter5294 source window through the native frame probe.

The native actor type models selected fields of each 92-byte source record;
this probe compares those fields for the 27-record source prefix and verifies
the 23-record tail stays byte-identical. It does not claim full raw-record or
full player-state parity.
"""
from __future__ import annotations

import hashlib
import json
import struct
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
ROM_PATH = Path(r"Crazy Taxi - Catch a Ride (USA).gba")
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
PRE_EWRAM = REPORTS / "counter5294-source-pre.ewram"
BASE_TRACE = REPORTS / "r5294plain3.actor-trace.json"
ROADS = ROOT / "assets/traffic/city1-roads.json"
BIN = ROOT / "bin/route31-source-traffic-frame-probe.exe"
ACTOR_FILE = REPORTS / "counter5294-native-frame-input.actors.bin"
CFG_FILE = REPORTS / "counter5294-native-frame-input.cfg"
OUTPUT = REPORTS / "counter5294-native-traffic-frame-audit.json"
STRIDE = 0x5C
KEY_BIT = {0: 1, 6: 5, 7: 4, 8: 0}


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def s32(b: bytes, o: int) -> int:
    return struct.unpack_from("<i", b, o)[0]


def u32(b: bytes, o: int) -> int:
    return struct.unpack_from("<I", b, o)[0]


def actor_rows(raw: bytes, descriptor_to_route: dict[int, int]) -> list[tuple[int, ...]]:
    if len(raw) != 50 * STRIDE:
        raise ValueError(f"expected 50 source actors, got {len(raw)} bytes")
    out = []
    for i in range(50):
        p = i * STRIDE
        desc = u32(raw, p + 0x24)
        route = descriptor_to_route.get(desc, 0)
        if desc and not route:
            raise ValueError(f"unknown route descriptor at actor{i}: {desc:#x}")
        out.append((u32(raw, p), u32(raw, p + 4), s32(raw, p + 0x10),
                    s32(raw, p + 0x14), s32(raw, p + 0x18), s32(raw, p + 0x1C),
                    s32(raw, p + 0x20), route, s32(raw, p + 0x28),
                    u32(raw, p + 0x2C), s32(raw, p + 0x34), s32(raw, p + 0x38),
                    struct.unpack_from("<H", raw, p + 0x3C)[0], s32(raw, p + 0x40),
                    s32(raw, p + 0x48), s32(raw, p + 0x4C), s32(raw, p + 0x54),
                    s32(raw, p + 0x58)))
    return out


def main() -> int:
    rom = ROM_PATH.read_bytes()
    pre = PRE_EWRAM.read_bytes()
    rows = json.loads(BASE_TRACE.read_text(encoding="utf-8"))
    if sha(rom) != ROM_SHA or len(pre) != 0x40000 or len(rows) != 3:
        raise SystemExit("source ROM, EWRAM, or clean trace identity mismatch")
    if [int(r["counter"]) for r in rows] != [5294, 5295, 5296]:
        raise SystemExit("clean trace is not aligned to counter5294–5296")
    if [r["input_keys"] for r in rows] != [[6, 8]] * 3:
        raise SystemExit("clean source input differs from held keys 6+8")
    before, partial, after = rows
    if (before["delta"], before["frame_delta"]) != (5, 5):
        raise SystemExit("counter5294 source frame timing changed")
    pre_pool = pre[0xF4:0xF4 + 50 * STRIDE]
    iwram_path = REPORTS / "r5294base1100.03000000.bin"
    iwram = iwram_path.read_bytes()
    if len(iwram) != 0x8000:
        raise SystemExit("matching checkpoint IWRAM file is missing or malformed")
    source_after = bytes.fromhex(after["traffic_actor_records"]["raw"])
    if len(source_after) != 50 * STRIDE:
        raise SystemExit("source output does not contain all 50 actor records")
    prefix_count = 27
    descriptor_to_route = {int(p["descriptor_address"], 16): int(p["index"])
                           for p in json.loads(ROADS.read_text(encoding="utf-8"))["paths"]}
    pre_actors = actor_rows(pre_pool, descriptor_to_route)
    expected_actors = actor_rows(source_after, descriptor_to_route)
    ACTOR_FILE.write_bytes(b"".join(struct.pack("<18i", *a) for a in pre_actors))

    camera = before["traffic_camera"]["position"]
    g = {
        "setting": pre[0x1EB3],
        "count": prefix_count,
        "timer": pre[0x25D5],
        "counter": u32(iwram, 0x4C98),
        "cooldown": struct.unpack_from("<h", pre, 0xE4)[0],
        "near": pre[0x25D3],
        "voice": pre[0x25D6],
        "combo_flag": pre[0x9AA6],
        "combo_whole": s32(pre, 0x9AA8),
        "combo_fraction": s32(pre, 0x9AAC),
        "rng": u32(pre, 0x3B04),
        "sound": int(before["traffic_frame_context"]["sound_7602"]),
        "active": int(before["traffic_frame_context"]["trip_active_1694"]),
        "camera_x": int(camera[0]),
        "camera_z": int(camera[2]),
    }
    CFG_FILE.write_text(" ".join(str(g[k]) for k in (
        "setting", "count", "timer", "counter", "cooldown", "near", "voice",
        "combo_flag", "combo_whole", "combo_fraction", "rng", "sound", "active",
        "camera_x", "camera_z")) + "\n", encoding="ascii")

    # The source trace records the GBA key names; the existing probe consumes
    # active-low key input and advances the native vehicle once at this boundary.
    bits = sum(1 << KEY_BIT[k] for k in before["input_keys"])
    raw_keys = 0x3FF ^ bits
    input_line = f"{raw_keys} {before['delta']} {before['frame_delta']} 1 5294 1\n"
    result = subprocess.run([str(BIN), str(PRE_EWRAM), str(iwram_path),
                             str(ACTOR_FILE), str(CFG_FILE)], input=input_line,
                           text=True, capture_output=True, cwd=ROOT.parent.parent,
                           timeout=90, check=True)
    source_fields = ("flags", "type", "countdown", "x", "y", "z", "heading", "route",
                     "collision_timer", "impulse_x", "impulse_z", "offset_x", "offset_z",
                     "path_distance", "path_segment", "path_x", "path_z", "path_heading")
    native_actors = []
    native_globals = native_vehicle = None
    for line in result.stdout.splitlines():
        p = line.split()
        if p[0] == "A":
            vals = list(map(int, p[1:]))
            native_actors.append(dict(zip(("index", *source_fields), vals)))
        elif p[0] == "S":
            vals = list(map(int, p[1:]))
            native_globals = dict(zip(("tick", "spawned", "count", "timer", "counter",
                                       "cooldown", "near", "voice", "combo_flag",
                                       "combo_whole", "combo_fraction", "rng"), vals))
        elif p[0] == "V":
            vals = list(map(int, p[1:]))
            native_vehicle = dict(zip(("tick", "x", "y", "z", "heading", "speed"), vals))
    if len(native_actors) != prefix_count or native_globals is None or native_vehicle is None:
        raise SystemExit(f"native frame probe output incomplete: {result.stdout[-1200:]}")

    actor_mismatches = []
    expected_names = ("flags", "type", "countdown", "x", "y", "z", "heading", "route",
                      "path_distance", "path_segment", "path_x", "path_z", "path_heading",
                      "impulse_x", "impulse_z", "offset_x", "offset_z", "collision_timer")
    for got in native_actors:
        expected = dict(zip(expected_names, expected_actors[got["index"]]))
        bad = {k: {"native": got[k], "source": v} for k, v in expected.items() if got[k] != v}
        if bad:
            actor_mismatches.append({"index": got["index"], "fields": bad})

    # Rebuild the complete source-format records by writing the native-owned
    # fields back into a copy of each raw pre-event record. This keeps genuinely
    # unmodeled bytes intact and makes any unexpected source-side changes visible.
    rebuilt = bytearray(pre_pool)
    route_to_descriptor = {int(p["index"]): int(p["descriptor_address"], 16)
                           for p in json.loads(ROADS.read_text(encoding="utf-8"))["paths"]}
    word_fields = (("flags", 0x00), ("type", 0x04), ("countdown", 0x10),
                   ("x", 0x14), ("y", 0x18), ("z", 0x1C), ("heading", 0x20),
                   ("collision_timer", 0x58), ("path_distance", 0x28),
                   ("path_segment", 0x2C), ("path_x", 0x34), ("path_z", 0x38),
                   ("impulse_x", 0x40), ("impulse_z", 0x48),
                   ("offset_x", 0x4C), ("offset_z", 0x54))
    for actor in native_actors:
        off = actor["index"] * STRIDE
        for field, field_off in word_fields:
            struct.pack_into("<I", rebuilt, off + field_off, actor[field] & 0xFFFFFFFF)
        struct.pack_into("<I", rebuilt, off + 0x24,
                         route_to_descriptor.get(actor["route"], 0))
        struct.pack_into("<H", rebuilt, off + 0x3C, actor["path_heading"] & 0xFFFF)
    raw_mismatches = []
    for slot in range(50):
        n, s = slot * STRIDE, (slot + 1) * STRIDE
        if rebuilt[n:s] != source_after[n:s]:
            offsets = [i for i, (a, b) in enumerate(zip(rebuilt[n:s], source_after[n:s])) if a != b]
            raw_mismatches.append({"slot": slot, "byte_offsets": offsets[:24],
                                   "mismatch_count": len(offsets)})

    source_g = after["traffic_globals"]
    global_expected = {
        "count": prefix_count, "timer": source_g["timer_25d5"],
        "counter": source_g["selection_counter_4c98"], "cooldown": source_g["cooldown_e4"],
        "near": source_g["near_miss_25d3"], "voice": source_g["voice_25d6"],
        "combo_flag": source_g["combo_flag_9aa6"], "combo_whole": source_g["combo_9aa8"],
        "combo_fraction": source_g["combo_9aac"], "rng": source_g["rng_3b04"],
    }
    global_mismatches = {k: {"native": native_globals[k], "source": v}
                         for k, v in global_expected.items() if native_globals[k] != v}
    source_vehicle = {"x": after["position"][0], "y": after["position"][1],
                      "z": after["position"][2], "heading": after["heading"],
                      "speed": after["speed"]}
    vehicle_mismatches = {k: {"native": native_vehicle[k], "source": v}
                          for k, v in source_vehicle.items() if native_vehicle[k] != v}
    tail_equal = pre_pool[prefix_count * STRIDE:] == source_after[prefix_count * STRIDE:]
    report = {
        "status": "PASS_COUNTER5294_NATIVE_TRAFFIC_PREFIX_FRAME" if not actor_mismatches and not raw_mismatches and not global_mismatches and not vehicle_mismatches and tail_equal else "MISMATCH_COUNTER5294_NATIVE_TRAFFIC_PREFIX_FRAME",
        "rom_sha256": ROM_SHA,
        "pre_ewram_sha256": sha(pre),
        "pre_iwram_sha256": sha(iwram),
        "source_trace_sha256": sha(BASE_TRACE.read_bytes()),
        "source_window_counters": [int(before["counter"]), int(partial["counter"]), int(after["counter"])],
        "native_frame_input": {"input_keys": before["input_keys"], "active_low_keyinput": raw_keys,
                                "delta": before["delta"], "frame_delta": before["frame_delta"],
                                "selected_prefix_count": prefix_count},
        "native_actor_fields_matched": len(native_actors) - len(actor_mismatches),
        "active_prefix_count": prefix_count,
        "actor_field_mismatches": actor_mismatches,
        "full_50_actor_table_byte_mismatches_after_preserving_unmodeled_native_bytes": raw_mismatches,
        "full_50_actor_table_byte_equal": not raw_mismatches,
        "inactive_tail_27_to_49_byte_equal": tail_equal,
        "traffic_global_mismatches": global_mismatches,
        "vehicle_mismatches": vehicle_mismatches,
        "native_stdout": result.stdout,
        "scope": "One native 27-slot taxi_traffic_frame replay seeded from the counter5294 pre-event EWRAM/IWRAM snapshot and held input. Compares modeled fields for slots0–26, then reconstructs complete 92-byte records by preserving unmodeled pre-event bytes and writing native-owned fields; compares all50 records byte-for-byte to the clean source snapshot after dispatcher completion at counter5295. Also verifies shared traffic globals/RNG and vehicle position/heading/speed. Does not compare full player actor state, camera bytes, or prove sustained gameplay/trip completion.",
    }
    OUTPUT.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "actor_matches": report["native_actor_fields_matched"],
                      "actor_mismatches": len(actor_mismatches), "tail_equal": tail_equal,
                      "full_table_byte_equal": not raw_mismatches,
                      "raw_record_mismatches": raw_mismatches,
                      "global_mismatches": global_mismatches, "vehicle_mismatches": vehicle_mismatches,
                      "native_vehicle": native_vehicle, "source_vehicle": source_vehicle}, indent=2))
    return 0 if report["status"].startswith("PASS") else 2


if __name__ == "__main__":
    raise SystemExit(main())
