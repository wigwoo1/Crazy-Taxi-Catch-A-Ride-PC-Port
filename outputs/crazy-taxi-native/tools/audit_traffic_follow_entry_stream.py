"""Audit per-actor 0x08006908 entry records against an untouched capture."""
import hashlib
import json
import pathlib
import struct
import sys
from collections import Counter, defaultdict

ROOT = pathlib.Path(__file__).resolve().parents[1]
OUT = ROOT / "reports" / "original-core-reference"
LOG_BEGIN, LOG_END = 0x3F000, 0x40000
STRIDE = 0x5C
ACTOR_BASE = 0x020000F4


def read(label, suffix):
    return (OUT / (label + suffix)).read_bytes()


def load(label, suffix):
    return json.loads(read(label, suffix))


def main():
    if len(sys.argv) not in (3, 4):
        raise SystemExit("usage: audit_traffic_follow_entry_stream.py BASELINE_LABEL PROBE_LABEL [OUTPUT_JSON]")
    baseline, probe = sys.argv[1:3]
    output = pathlib.Path(sys.argv[3]) if len(sys.argv) == 4 else OUT / "traffic-follow-entry-stream-audit.json"
    base_rows = load(baseline, ".actor-trace.json")
    probe_rows = load(probe, ".actor-trace.json")
    base_packed = load(baseline, ".packed-driving-trace.json")
    probe_packed = load(probe, ".packed-driving-trace.json")
    assert base_rows == probe_rows and base_packed == probe_packed
    assert base_rows and len(base_rows) == len(probe_rows)

    base_ram, probe_ram = read(baseline, ".ewram"), read(probe, ".ewram")
    assert len(base_ram) == len(probe_ram) == 0x40000
    ewram_diff = [i for i, (a, b) in enumerate(zip(base_ram, probe_ram)) if a != b]
    outside = [i for i in ewram_diff if not LOG_BEGIN <= i < LOG_END]
    assert not outside, outside[:16]

    stream_doc = load(probe, ".traffic-follow-entry-trace.json")
    events = stream_doc["call_events"]
    per_tick = defaultdict(list)
    entry_player_snapshot_relation = Counter()
    previous_actor_matches = 0
    for event in events:
        tick = event["observed_after_core_tick"]
        assert 0 <= tick < len(base_rows)
        row = base_rows[tick]
        assert event["source_counter"] == row["counter"]
        ptr = int(event["actor_pointer"], 16)
        slot = (ptr - ACTOR_BASE) // STRIDE
        assert 0 <= slot < 50 and (ptr - ACTOR_BASE) % STRIDE == 0, (tick, hex(ptr))
        raw_actor = bytes.fromhex(event["actor_raw"])
        assert len(raw_actor) == STRIDE
        if tick > 0:
            previous_pool = bytes.fromhex(base_rows[tick - 1]["traffic_actor_records"]["raw"])
            assert raw_actor == previous_pool[slot * STRIDE:(slot + 1) * STRIDE], (tick, slot)
            previous_actor_matches += 1
        if not per_tick[tick]:
            packed_by_tick = {item["tick"]: item for item in base_packed["records"]}
            entry = (event["player_position"], event["player_heading"], event["player_speed_544"])
            relations = []
            for offset, name in ((-1, "previous_vblank"), (0, "same_index_vblank"), (1, "next_index_vblank")):
                candidate = packed_by_tick.get(tick + offset)
                if candidate is None:
                    continue
                actor = bytes.fromhex(candidate["actor"])
                snapshot = (list(struct.unpack_from("<iii", actor, 0)),
                            struct.unpack_from("<i", actor, 0x1C)[0],
                            struct.unpack_from("<i", actor, 0x544)[0])
                if entry == snapshot:
                    relations.append(name)
            entry_player_snapshot_relation["+".join(relations) if relations else "no_adjacent_vblank_match"] += 1
        per_tick[tick].append((slot, event))

    target = [event for event in events if event["source_counter"] == 4286]
    assert len(target) >= 1 and target[0]["actor_pointer"] == "0x20000f4"
    result = {
        "status": "PASS_ENTRY_STREAM_MATCHES_PREVIOUS_VBLANK_ACTOR_POOL",
        "baseline_label": baseline,
        "probe_label": probe,
        "rom_sha256": stream_doc["instrumentation"]["source_rom_sha256"],
        "instrumented_rom_sha256": stream_doc["instrumentation"]["instrumented_rom_sha256"],
        "vblank_rows": len(base_rows),
        "actor_trace_identical": True,
        "packed_driving_trace_identical": True,
        "paired_50_actor_pool_rows_identical": True,
        "per_actor_entry_records": len(events),
        "source_ticks_with_calls": len(per_tick),
        "calls_per_tick_histogram": dict(sorted(Counter(map(len, per_tick.values())).items())),
        "entry_actor_matches_previous_vblank": previous_actor_matches,
        "first_call_player_entry_relation_to_neighboring_vblank_snapshots": dict(entry_player_snapshot_relation),
        "ewram_diff_bytes": len(ewram_diff),
        "ewram_diff_outside_diagnostic_buffer": len(outside),
        "diagnostic_buffer": {"address": "0x0203F000", "bytes": LOG_END - LOG_BEGIN},
        "counter_4286_call_sequence": [{"actor_pointer": e["actor_pointer"], "player_speed_544": e["player_speed_544"], "player_position": e["player_position"]} for e in target],
        "scope": "Source-ROM diagnostic. Each record is the exact per-actor 0x08006908 input before its original routine body. Actor records match the preceding VBlank pool, but player inputs do not reliably map to a neighboring VBlank snapshot; the captured per-call values must be retained as separate data. At counter4286 the player input matches neither neighboring snapshot. Paired gameplay traces match, while instrumentation still changes small IWRAM/VRAM regions and is not fully timing-neutral. This does not prove a complete native port.",
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result))


if __name__ == "__main__":
    main()
