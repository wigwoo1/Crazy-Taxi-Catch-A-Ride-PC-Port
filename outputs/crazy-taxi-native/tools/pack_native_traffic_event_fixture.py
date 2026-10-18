"""Pack per-VBlank traffic-pool change checkpoints for offline native replay.

Changed actor bytes nominate candidate traffic update windows; this is not a
ROM instruction trace and must not be described as proof of call entry.
"""
import collections
import hashlib
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
from extract_traffic_actor_replay_fixture import array_items, sha

ACTOR = struct.Struct("<18i")
GLOBALS = struct.Struct("<h4B2i2I")
SCHEDULE = struct.Struct("<14i")
HEADER = struct.Struct("<8sIIB3x")
MAGIC = b"CTTFRP03"
ACTORS = 50
STRIDE = 0x5C
PLAYER_BYTES = 0xEA0


def u16(data, off): return struct.unpack_from("<H", data, off)[0]
def s16(data, off): return struct.unpack_from("<h", data, off)[0]
def u32(data, off): return struct.unpack_from("<I", data, off)[0]
def s32(data, off): return struct.unpack_from("<i", data, off)[0]


def actor_table(raw, descriptors):
    assert len(raw) == ACTORS * STRIDE
    result = bytearray()
    for i in range(ACTORS):
        p = i * STRIDE
        descriptor = u32(raw, p + 0x24)
        route = descriptors.get(descriptor, 0)
        assert not descriptor or route, (i, hex(descriptor))
        values = (
            u32(raw, p), u32(raw, p + 4), s32(raw, p + 0x10),
            s32(raw, p + 0x14), s32(raw, p + 0x18), s32(raw, p + 0x1C),
            s32(raw, p + 0x20), route, s32(raw, p + 0x28),
            u32(raw, p + 0x2C), s32(raw, p + 0x34), s32(raw, p + 0x38),
            u16(raw, p + 0x3C), s32(raw, p + 0x40), s32(raw, p + 0x48),
            s32(raw, p + 0x4C), s32(raw, p + 0x54), s32(raw, p + 0x58),
        )
        result.extend(ACTOR.pack(*values))
    return result


def globals_pack(g):
    return GLOBALS.pack(g["cooldown_e4"], g["near_miss_25d3"], g["voice_25d6"],
        g["combo_flag_9aa6"], g["timer_25d5"], g["combo_9aa8"], g["combo_9aac"],
        g["rng_3b04"], g["selection_counter_4c98"])


def main():
    if len(sys.argv) != 6:
        raise SystemExit("usage: pack_native_traffic_event_fixture.py TRACE ROADS INITIAL_EWRAM INITIAL_IWRAM OUTPUT_STEM")
    trace_path, roads_path, ewram_path, iwram_path, stem = map(pathlib.Path, sys.argv[1:])
    roads = json.loads(roads_path.read_text(encoding="utf-8"))
    descriptors = {int(p["descriptor_address"], 16): int(p["index"]) for p in roads["paths"]}
    ewram = ewram_path.read_bytes(); iwram = iwram_path.read_bytes()
    assert len(ewram) == 0x40000 and len(iwram) >= 0x8000
    initial_globals = {
        "cooldown_e4": s16(ewram, 0xE4), "near_miss_25d3": ewram[0x25D3],
        "voice_25d6": ewram[0x25D6], "combo_flag_9aa6": ewram[0x9AA6],
        "timer_25d5": ewram[0x25D5], "combo_9aa8": s32(ewram, 0x9AA8),
        "combo_9aac": s32(ewram, 0x9AAC), "rng_3b04": u32(ewram, 0x3B04),
        "selection_counter_4c98": u32(iwram, 0x4C98),
    }
    output = trace_path.with_name(stem.name + "-native-traffic-events.bin")
    schedule_path = trace_path.with_name(stem.name + "-native-traffic-events.schedule.txt")
    events = []; previous_raw = ewram[0xF4:0xF4+ACTORS*STRIDE]; previous_tick = None; changed_hist = collections.Counter()
    for tick, frame in array_items(trace_path):
        raw = bytes.fromhex(frame["traffic_actor_records"]["raw"])
        if previous_raw is not None and raw != previous_raw:
            changed = [i for i in range(ACTORS) if raw[i*STRIDE:(i+1)*STRIDE] != previous_raw[i*STRIDE:(i+1)*STRIDE]]
            assert frame.get("traffic_player_state") and frame.get("traffic_camera", {}).get("raw_40")
            events.append((tick, frame, raw, changed))
            changed_hist[len(changed)] += 1
        previous_raw = raw
    assert events
    with output.open("wb") as out, schedule_path.open("w", encoding="utf-8", newline="\n") as sched:
        out.write(HEADER.pack(MAGIC, len(events), ACTORS, ewram[0x1EB3]))
        out.write(bytes((ewram[0x5F20], ewram[0x7602], ewram[0x1694], 0)))
        out.write(globals_pack(initial_globals))
        out.write(actor_table(ewram[0xF4:0xF4+ACTORS*STRIDE], descriptors))
        sched.write(f"SOURCE_DRIVING_REFERENCE_V3 {len(events)}\n")
        for tick, frame, raw, changed in events:
            ps = frame["traffic_player_state"]
            actor = bytes.fromhex(ps["actor"])
            hist = bytes.fromhex(ps["input_history"])
            commands = bytes.fromhex(ps["time_commands"])
            shared = ps["shared_owners"]
            camera = frame["traffic_camera"]
            position = struct.unpack_from("<3i", actor, 0)
            heading = s32(actor, 0x1C); speed = s32(actor, 0x544)
            delta = s32(commands, 0)
            frame_delta = ps["frame_delta"]
            prior = max(1, min(60, tick - previous_tick)) if previous_tick is not None else 1
            previous_tick = tick
            owners = frame["driver_owners"]
            schedule = (frame["keyinput"], prior, delta, frame_delta, *position,
                heading, speed, frame["counter"], owners["flag_1697"],
                owners["flag_153c"], owners["freeze_2620"], owners["freeze_177b"])
            assert 0 < delta <= 60 and 0 < frame_delta <= 60, (tick, delta, frame_delta)
            sched.write(" ".join(map(str, schedule[:9])) + f" {tick + 1} " + " ".join(map(str, schedule[10:14])) + "\n")
            out.write(SCHEDULE.pack(*schedule))
            context = frame["traffic_frame_context"]
            out.write(bytes((context["freeze_5f20"], context["sound_7602"], context["trip_active_1694"], 0)))
            out.write(struct.pack("<3i", *camera["position"]))
            out.write(globals_pack(frame["traffic_globals"]))
            out.write(actor_table(raw, descriptors))
            out.write(actor)
            out.write(hist)
            out.write(commands)
            for address in ("02001534", "0200164C", "02001770", "02001EA0", "02001EB0", "020025D0", "02007600"):
                out.write(bytes.fromhex(shared[address]))
            out.write(bytes.fromhex(camera["raw_40"]))
    report = {
        "status": "PASS_FIXTURE_SCHEMA", "rom_sha256": roads["rom_sha256"],
        "source_trace_sha256": sha(trace_path), "road_table_sha256": sha(roads_path),
        "initial_ewram_sha256": sha(ewram_path), "initial_iwram_sha256": sha(iwram_path),
        "candidate_vblanks": len(events), "changed_actor_slot_histogram": dict(sorted(changed_hist.items())),
        "full_player_shared_owner_and_camera_captured": True,
        "binary_bytes": output.stat().st_size, "binary_sha256": sha(output),
        "schedule_sha256": sha(schedule_path), "output": str(output),
        "scope": "Offline source checkpoint fixture; VBlank frames where the raw 50-slot actor pool changed nominate candidate traffic updates. This is not routine-entry instrumentation or proof that each candidate is one original traffic_frame call."
    }
    report_path = output.with_suffix(".json")
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report))


if __name__ == "__main__": main()
