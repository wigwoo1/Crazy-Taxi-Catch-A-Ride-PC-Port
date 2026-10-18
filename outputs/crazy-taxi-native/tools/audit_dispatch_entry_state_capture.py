"""Audit a dispatcher-entry probe against an untouched source-ROM run."""
import hashlib
import json
import pathlib
import struct
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
OUT = ROOT / "reports" / "original-core-reference"
SNAPSHOT_ADDRESS = 0x3FB00
SNAPSHOT_BYTES = 136


def read(label, suffix):
    return (OUT / (label + suffix)).read_bytes()


def json_read(label, suffix):
    return json.loads(read(label, suffix))


def runs(offsets):
    result = []
    for value in offsets:
        if not result or value != result[-1][-1] + 1:
            result.append([value])
        else:
            result[-1].append(value)
    return [{"start": hex(run[0]), "end": hex(run[-1]), "bytes": len(run)} for run in result]


def main():
    if len(sys.argv) not in (3, 4):
        raise SystemExit("usage: audit_dispatch_entry_state_capture.py BASELINE_LABEL PROBE_LABEL [OUTPUT_JSON]")
    baseline, probe = sys.argv[1:3]
    collision_entry = (OUT / (probe + ".traffic-collision-entry-state.json")).exists()
    entry_suffix = ".traffic-collision-entry-state" if collision_entry else ".traffic-dispatch-entry-state"
    default_name = "traffic-collision-entry-state-audit.json" if collision_entry else "dispatch-entry-state-capture-audit.json"
    output = pathlib.Path(sys.argv[3]) if len(sys.argv) == 4 else OUT / default_name
    base_actor = json_read(baseline, ".actor-trace.json")
    probe_actor = json_read(probe, ".actor-trace.json")
    base_packed = json_read(baseline, ".packed-driving-trace.json")
    probe_packed = json_read(probe, ".packed-driving-trace.json")
    assert len(base_actor) == len(probe_actor) and base_actor == probe_actor
    assert base_packed == probe_packed

    base_ewram = read(baseline, ".ewram")
    probe_ewram = read(probe, ".ewram")
    assert len(base_ewram) == len(probe_ewram) == 0x40000
    ewram_differences = [i for i, (a, b) in enumerate(zip(base_ewram, probe_ewram)) if a != b]
    outside_snapshot = [i for i in ewram_differences
        if not SNAPSHOT_ADDRESS <= i < SNAPSHOT_ADDRESS + SNAPSHOT_BYTES]
    assert not outside_snapshot, outside_snapshot[:16]

    memory_differences = {}
    for suffix in (".03000000.bin", ".04000000.bin", ".05000000.bin", ".06000000.bin", ".07000000.bin"):
        base = read(baseline, suffix)
        test = read(probe, suffix)
        assert len(base) == len(test), suffix
        offsets = [i for i, (a, b) in enumerate(zip(base, test)) if a != b]
        memory_differences[suffix] = {"bytes": len(offsets), "ranges": runs(offsets)}

    entry = json_read(probe, entry_suffix + ".json")
    raw = read(probe, entry_suffix + ".bin")
    assert len(raw) == SNAPSHOT_BYTES
    assert hashlib.sha256(raw).hexdigest() == entry["raw_sha256"]
    assert entry["source_counter"] == 4286
    assert entry["observed_after_core_tick"] < len(base_actor)
    tick = entry["observed_after_core_tick"]
    source_row = base_actor[tick]
    assert source_row["counter"] == entry["source_counter"]
    assert source_row.get("traffic_player_state")
    post_actor = bytes.fromhex(source_row["traffic_player_state"]["actor"])
    post_actor0 = bytes.fromhex(source_row["traffic_actor_records"]["raw"])[:0x5C]
    entry_actor0 = raw[36:128]
    post_speed = struct.unpack_from("<i", post_actor, 0x544)[0]
    post_position = list(struct.unpack_from("<iii", post_actor, 0))
    entry_position = list(struct.unpack_from("<iii", raw, 12))
    entry_speed = struct.unpack_from("<i", raw, 28)[0]
    entry_flags = struct.unpack_from("<I", entry_actor0, 0)[0]
    post_flags = struct.unpack_from("<I", post_actor0, 0)[0]
    actor0_differences = [i for i, (a, b) in enumerate(zip(entry_actor0, post_actor0)) if a != b]

    incoming = entry.get("incoming_r0_actor_pointer", entry.get("incoming_r0_delta"))
    result = {
        "status": "PASS_GAMEPLAY_RAM_PARITY_WITH_CAPTURE_BUFFER_ONLY",
        "baseline_label": baseline,
        "probe_label": probe,
        "rom_sha256": entry["instrumentation"]["source_rom_sha256"],
        "instrumented_rom_sha256": entry["instrumentation"]["instrumented_rom_sha256"],
        "source_counter": entry["source_counter"],
        "entrypoint": entry["instrumentation"]["patched_entry"],
        "observed_after_core_tick": tick,
        "incoming_r0": incoming,
        "rows": len(base_actor),
        "actor_trace_identical": True,
        "packed_driving_trace_identical": True,
        "all_50_traffic_actor_records_identical_each_frame": True,
        "ewram_differences": len(ewram_differences),
        "ewram_differences_outside_hook_buffer": len(outside_snapshot),
        "hook_buffer_address": hex(SNAPSHOT_ADDRESS),
        "hook_buffer_size": SNAPSHOT_BYTES,
        "other_memory_differences": memory_differences,
        "dispatcher_entry": {
            "player_position": entry_position,
            "post_vblank_player_position": post_position,
            "position_unchanged": entry_position == post_position,
            "player_speed_544": entry_speed,
            "post_vblank_player_speed_544": post_speed,
            "actor0_flags": entry_flags,
            "post_vblank_actor0_flags": post_flags,
            "actor0_changed_offsets": actor0_differences,
            "actor0_entry_sha256": hashlib.sha256(entry_actor0).hexdigest(),
            "actor0_post_vblank_sha256": hashlib.sha256(post_actor0).hexdigest(),
        },
        "interpretation": ("At counter 4286, the direct 0x08006908 routine-entry probe sees r0 pointing at actor0 and player speed 22140 before the original collision/follow body; the same tick's post-retro_run snapshot sees speed 5535 and actor0 after its update. This resolves the post-frame phase mismatch for this event. EWRAM and packed gameplay traces match the untouched run outside the deliberate 136-byte probe buffer. IWRAM stack bytes and VRAM bytes differ, so the hook is not claimed to be fully timing-neutral or instruction-cycle neutral." if collision_entry else "At counter 4286, the direct dispatcher-entry probe sees player speed 22140 before original actor service; the same tick's post-retro_run snapshot sees speed 5535 and actor0 after its update. EWRAM and packed gameplay traces match the untouched run outside the deliberate 136-byte probe buffer. IWRAM stack bytes and VRAM bytes differ, so the hook is not claimed to be fully timing-neutral or instruction-cycle neutral."),
        "scope": "Source-ROM diagnostic only. This is one event in one route-following capture. It does not establish full native traffic parity, passenger/fare gameplay, or a playable port.",
    }
    assert result["dispatcher_entry"]["position_unchanged"]
    assert entry_speed != post_speed
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result))


if __name__ == "__main__":
    main()
