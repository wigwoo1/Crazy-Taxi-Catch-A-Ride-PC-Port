"""Pack captured source traffic state into a fixed binary test input.

The packed input is consumed by an offline native differential harness. It is
not a runtime asset and does not imply dispatch parity.
"""
import hashlib
import json
import pathlib
import struct
import sys

ACTOR = struct.Struct("<18i")
GLOBALS = struct.Struct("<hBBBBiiII")
SCHEDULE = struct.Struct("<14i")
HEADER = struct.Struct("<8sIIB3x")
MAGIC = b"CTTFRP02"


def u16(data, off): return struct.unpack_from("<H", data, off)[0]
def s16(data, off): return struct.unpack_from("<h", data, off)[0]
def u32(data, off): return struct.unpack_from("<I", data, off)[0]
def s32(data, off): return struct.unpack_from("<i", data, off)[0]


def actor_table(raw, descriptors):
    assert len(raw) == 50 * 0x5C
    result = bytearray()
    for i in range(50):
        p = i * 0x5C
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
    if len(sys.argv) != 5:
        raise SystemExit("usage: pack_native_traffic_dispatch_fixture.py FIXTURE ROADS INITIAL_EWRAM INITIAL_IWRAM")
    fixture_path, roads_path, ewram_path, iwram_path = map(pathlib.Path, sys.argv[1:])
    roads = json.loads(roads_path.read_text(encoding="utf-8"))
    descriptors = {int(p["descriptor_address"], 16): int(p["index"]) for p in roads["paths"]}
    lines = fixture_path.read_text(encoding="utf-8").splitlines()
    header = json.loads(lines[0])["header"]
    assert header["format"] == "SOURCE_TRAFFIC_REPLAY_FIXTURE_V2"
    records = [json.loads(line) for line in lines[1:]]
    assert len(records) == header["schedule_rows"] == 1755
    ewram = ewram_path.read_bytes(); iwram = iwram_path.read_bytes()
    assert len(ewram) == 0x40000 and len(iwram) >= 0x8000
    initial_actors = actor_table(ewram[0xF4:0xF4+50*0x5C], descriptors)
    initial_globals = {
        "cooldown_e4": s16(ewram, 0xE4), "near_miss_25d3": ewram[0x25D3],
        "voice_25d6": ewram[0x25D6], "combo_flag_9aa6": ewram[0x9AA6],
        "timer_25d5": ewram[0x25D5], "combo_9aa8": s32(ewram, 0x9AA8),
        "combo_9aac": s32(ewram, 0x9AAC), "rng_3b04": u32(ewram, 0x3B04),
        "selection_counter_4c98": u32(iwram, 0x4C98),
    }
    out = bytearray(HEADER.pack(MAGIC, len(records), 50, ewram[0x1EB3]))
    out.extend(bytes((ewram[0x5F20], ewram[0x7602], ewram[0x1694], 0)))
    out.extend(globals_pack(initial_globals))
    out.extend(initial_actors)
    for record in records:
        source = record["source"]
        row = record["schedule"]
        assert len(row) == 14
        out.extend(SCHEDULE.pack(*row))
        context = source["frame_context"]
        out.extend(bytes((context["freeze_5f20"], context["sound_7602"], context["trip_active_1694"], 0)))
        out.extend(struct.pack("<3i", *source["camera"]["position"]))
        out.extend(globals_pack(source["globals"]))
        out.extend(actor_table(bytes.fromhex(source["actors_raw_hex"]), descriptors))
        player = source["player_state"]
        out.extend(bytes.fromhex(player["actor"]))
        out.extend(bytes.fromhex(player["input_history"]))
        out.extend(bytes.fromhex(player["time_commands"]))
        shared = player["shared_owners"]
        for address in ("02001534", "0200164C", "02001770", "02001EA0", "02001EB0", "020025D0", "02007600"):
            out.extend(bytes.fromhex(shared[address]))
        out.extend(bytes.fromhex(source["camera"]["raw_40"]))
    output = fixture_path.with_name("route31poolcontext4-native-traffic-dispatch.bin")
    output.write_bytes(out)
    print(json.dumps({"status":"PASS","records":len(records),"route_ids":sorted({x["index"] for x in roads["paths"] if x["index"] in {a[7] for a in [ACTOR.unpack_from(initial_actors,i*ACTOR.size) for i in range(50)]}}),"binary_bytes":len(out),"binary_sha256":hashlib.sha256(out).hexdigest(),"output":str(output),"setting_1eb3":ewram[0x1EB3],"initial_globals":initial_globals}))


if __name__ == "__main__": main()
