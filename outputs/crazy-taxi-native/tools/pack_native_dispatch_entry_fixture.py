"""Pack original-ROM traffic snapshots only when callback-entry state exists.

Dispatcher events are logged during retro_run, while ordinary player snapshots
are read after it returns. The packer therefore refuses legacy traces unless
an explicit dispatcher-entry snapshot was captured at 0x080066B4. This remains
an offline diagnostic, not a playable-port test.
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

ACTORS = 50
STRIDE = 0x5C
PLAYER_BYTES = 0xEA0
ACTOR = struct.Struct("<18i")
GLOBALS = struct.Struct("<h4B2i2I")
SCHEDULE = struct.Struct("<14i")
HEADER = struct.Struct("<8sIIB3x")
MAGIC = b"CTTFRP04"
COMPARE = ("tick", "input_keys", "keyinput", "heading", "speed", "counter",
           "previous", "frame_delta", "delta", "position", "traffic_camera",
           "driver_owners", "traffic_actor_records", "traffic_globals",
           "traffic_frame_context", "route_driver", "completed_count",
           "fare_whole", "fare_fraction", "remaining_clock")


def require_dispatch_entry_snapshots(selected, trace_rows):
    """Reject VBlank-only captures masquerading as callback-entry state."""
    for event in selected:
        tick = event["observed_after_core_tick"]
        frame = trace_rows[tick]
        if "traffic_dispatch_entry_state" not in frame:
            raise SystemExit(
                "refusing to pack: trace has only post-retro_run player snapshots; "
                "capture traffic_dispatch_entry_state at 0x080066B4 first"
            )


def sha_file(path):
    h = hashlib.sha256()
    with pathlib.Path(path).open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


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
        result.extend(ACTOR.pack(
            u32(raw, p), u32(raw, p + 4), s32(raw, p + 0x10),
            s32(raw, p + 0x14), s32(raw, p + 0x18), s32(raw, p + 0x1C),
            s32(raw, p + 0x20), route, s32(raw, p + 0x28), u32(raw, p + 0x2C),
            s32(raw, p + 0x34), s32(raw, p + 0x38), u16(raw, p + 0x3C),
            s32(raw, p + 0x40), s32(raw, p + 0x48), s32(raw, p + 0x4C),
            s32(raw, p + 0x54), s32(raw, p + 0x58)))
    return result


def globals_pack(g):
    return GLOBALS.pack(g["cooldown_e4"], g["near_miss_25d3"], g["voice_25d6"],
        g["combo_flag_9aa6"], g["timer_25d5"], g["combo_9aa8"], g["combo_9aac"],
        g["rng_3b04"], g["selection_counter_4c98"])


def main():
    if len(sys.argv) != 8:
        raise SystemExit("usage: pack_native_dispatch_entry_fixture.py INSTRUMENTED_TRACE BASELINE_TRACE ENTRY_TRACE ROADS INITIAL_EWRAM INITIAL_IWRAM OUTPUT_STEM")
    traced, baseline, entries_path, roads_path, ewram_path, iwram_path, stem = map(pathlib.Path, sys.argv[1:])
    roads = json.loads(roads_path.read_text(encoding="utf-8"))
    descriptors = {int(p["descriptor_address"], 16): int(p["index"]) for p in roads["paths"]}
    ewram = ewram_path.read_bytes(); iwram = iwram_path.read_bytes()
    assert len(ewram) == 0x40000 and len(iwram) >= 0x8000
    entries_doc = json.loads(entries_path.read_text(encoding="utf-8"))
    entries = entries_doc["call_events"]
    wanted = {e["observed_after_core_tick"] for e in entries}
    needed_rows = wanted | {tick + 1 for tick in wanted}
    trace_rows = {}
    first_divergence = None
    baseline_iter = iter(array_items(baseline))
    baseline_index, base = next(baseline_iter)
    for tick, frame in array_items(traced):
        if tick != baseline_index:
            raise AssertionError(("trace row order", tick, baseline_index))
        for key in COMPARE:
            if frame.get(key) != base.get(key):
                left, right = frame.get(key), base.get(key)
                first_divergence = {"trace_index": tick, "field": key}
                if key == "traffic_actor_records":
                    lraw = bytes.fromhex(left["raw"]); rraw = bytes.fromhex(right["raw"])
                    first_divergence.update(changed_slots=[i for i in range(ACTORS)
                        if lraw[i * STRIDE:(i + 1) * STRIDE] != rraw[i * STRIDE:(i + 1) * STRIDE]],
                        instrumented_sha256=hashlib.sha256(lraw).hexdigest(),
                        baseline_sha256=hashlib.sha256(rraw).hexdigest())
                else:
                    first_divergence.update(instrumented=left, baseline=right)
                break
        if tick in needed_rows:
            trace_rows[tick] = frame
        if first_divergence:
            break
        try: baseline_index, base = next(baseline_iter)
        except StopIteration: break
    trustworthy_until = first_divergence["trace_index"] if first_divergence else len(trace_rows)
    selected = [e for e in entries if e["observed_after_core_tick"] < trustworthy_until]
    assert selected and len(trace_rows) >= len((wanted | {tick + 1 for tick in wanted}) & set(range(trustworthy_until)))
    assert selected[0]["observed_after_core_tick"] == 0
    initial_globals = {"cooldown_e4": s16(ewram, 0xE4), "near_miss_25d3": ewram[0x25D3],
        "voice_25d6": ewram[0x25D6], "combo_flag_9aa6": ewram[0x9AA6], "timer_25d5": ewram[0x25D5],
        "combo_9aa8": s32(ewram, 0x9AA8), "combo_9aac": s32(ewram, 0x9AAC),
        "rng_3b04": u32(ewram, 0x3B04), "selection_counter_4c98": u32(iwram, 0x4C98)}
    # A dispatcher event is recorded during retro_run, while the legacy
    # traffic_player_state field is sampled only after retro_run returns.
    # Require an explicit snapshot captured at the dispatcher hook.
    require_dispatch_entry_snapshots(selected, trace_rows)
    output = traced.with_name(stem.name + "-native-dispatch-entry.bin")
    schedule_path = traced.with_name(stem.name + "-native-dispatch-entry.schedule.txt")
    previous_tick = None; delta_matches = 0
    with output.open("wb") as out, schedule_path.open("w", encoding="utf-8", newline="\n") as sched:
        out.write(HEADER.pack(MAGIC, len(selected), ACTORS, ewram[0x1EB3]))
        out.write(bytes((ewram[0x5F20], ewram[0x7602], ewram[0x1694], 0)))
        out.write(globals_pack(initial_globals))
        out.write(actor_table(ewram[0xF4:0xF4 + ACTORS * STRIDE], descriptors))
        sched.write(f"SOURCE_DRIVING_REFERENCE_V3 {len(selected)}\n")
        for event in selected:
            tick = event["observed_after_core_tick"]
            frame = trace_rows[tick]; result_frame = trace_rows[tick + 1]; ps = frame["traffic_player_state"]
            actor = bytes.fromhex(ps["actor"]); hist = bytes.fromhex(ps["input_history"])
            commands = bytes.fromhex(ps["time_commands"]); camera = frame["traffic_camera"]
            raw = bytes.fromhex(result_frame["traffic_actor_records"]["raw"])
            assert len(actor) == PLAYER_BYTES and len(hist) == 68 and len(commands) == 48
            assert event["source_counter"] == frame["counter"], (tick, event["source_counter"], frame["counter"])
            assert event["frame_delta"] == ps["frame_delta"], (tick, event["frame_delta"], ps["frame_delta"])
            assert camera.get("raw_40") and len(camera["raw_40"]) == 0x80
            position = struct.unpack_from("<3i", actor, 0)
            delta = s32(commands, 0)
            prior = max(1, min(60, tick - previous_tick)) if previous_tick is not None else 1
            previous_tick = tick
            owners = frame["driver_owners"]
            schedule = (frame["keyinput"], prior, delta, event["frame_delta"], *position,
                s32(actor, 0x1C), s32(actor, 0x544), frame["counter"], owners["flag_1697"],
                owners["flag_153c"], owners["freeze_2620"], owners["freeze_177b"])
            assert 0 < delta <= 60 and 0 < event["frame_delta"] <= 60, (tick, delta, event["frame_delta"])
            delta_matches += int(event["frame_delta"] == ps["frame_delta"])
            sched.write(" ".join(map(str, schedule[:9])) + f" {tick + 1} " + " ".join(map(str, schedule[10:14])) + "\n")
            out.write(SCHEDULE.pack(*schedule))
            context = frame["traffic_frame_context"]
            out.write(bytes((context["freeze_5f20"], context["sound_7602"], context["trip_active_1694"], 0)))
            out.write(struct.pack("<3i", *camera["position"]))
            out.write(globals_pack(result_frame["traffic_globals"]))
            out.write(actor_table(raw, descriptors))
            out.write(actor + hist + commands)
            for address in ("02001534", "0200164C", "02001770", "02001EA0", "02001EB0", "020025D0", "02007600"):
                out.write(bytes.fromhex(ps["shared_owners"][address]))
            out.write(bytes.fromhex(camera["raw_40"]))
    report = {"status": "PASS_EXACT_DISPATCH_ENTRY_FIXTURE", "rom_sha256": roads["rom_sha256"],
        "instrumented_trace_sha256": sha_file(traced), "baseline_trace_sha256": sha_file(baseline),
        "entry_trace_sha256": sha_file(entries_path), "initial_ewram_sha256": sha_file(ewram_path),
        "initial_iwram_sha256": sha_file(iwram_path), "all_entry_ticks_have_player_camera_actor_snapshots": True,
        "entry_count_total": len(entries), "entry_count_in_verified_prefix": len(selected),
        "verified_prefix_last_tick": selected[-1]["observed_after_core_tick"],
        "frame_delta_matches_packed_source": delta_matches == len(selected),
        "first_instrumentation_divergence": first_divergence,
        "expected_snapshot_alignment": "Each entry uses player/camera/gates at the observed call frame and compares traffic actor/global output with the following VBlank snapshot; the direct trace shows pool changes become visible on that next frame.",
        "scope": "Offline native dispatcher diagnostic fixture. Original entry timestamps and input snapshots are aligned; output comparisons use the next VBlank and only the instrumented-vs-baseline equal prefix. Native actor/global parity remains to be tested; this is not a playable port.",
        "binary_bytes": output.stat().st_size, "binary_sha256": sha_file(output),
        "schedule_sha256": sha_file(schedule_path), "output": str(output)}
    report_path = output.with_suffix(".json")
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report))


if __name__ == "__main__": main()
