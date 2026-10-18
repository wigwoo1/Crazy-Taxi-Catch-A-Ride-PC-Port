"""Audit paired per-actor routine-entry and return captures."""
import hashlib
import json
import pathlib
import struct
import sys
from collections import Counter, defaultdict

ROOT = pathlib.Path(__file__).resolve().parents[1]
OUT = ROOT / "reports" / "original-core-reference"
ACTOR_BASE, ACTOR_STRIDE = 0x020000F4, 0x5C


def load(label, suffix):
    return json.loads((OUT / (label + suffix)).read_text(encoding="utf-8"))


def raw(label, suffix):
    return (OUT / (label + suffix)).read_bytes()


def player_snapshot(packed_row):
    actor = bytes.fromhex(packed_row["actor"])
    return (list(struct.unpack_from("<iii", actor, 0)),
            struct.unpack_from("<i", actor, 0x1C)[0],
            struct.unpack_from("<i", actor, 0x544)[0])


def main():
    if len(sys.argv) < 3:
        raise SystemExit("usage: audit_traffic_follow_entry_return.py BASELINE_LABEL PROBE_LABEL [OUTPUT_JSON] [--counter=N]")
    baseline, probe = sys.argv[1:3]
    args = sys.argv[3:]
    output_arg = next((arg for arg in args if not arg.startswith("--")), None)
    counter_arg = next((arg for arg in args if arg.startswith("--counter=")), "--counter=4286")
    expected_counter = int(counter_arg.split("=", 1)[1], 0)
    output = pathlib.Path(output_arg) if output_arg else OUT / "traffic-follow-entry-return-audit.json"
    base_rows, probe_rows = load(baseline, ".actor-trace.json"), load(probe, ".actor-trace.json")
    base_packed = load(baseline, ".packed-driving-trace.json")
    probe_packed = load(probe, ".packed-driving-trace.json")
    assert base_rows == probe_rows and base_packed == probe_packed
    assert len(base_rows) == len(probe_rows) > 0

    trace = load(probe, ".traffic-follow-entry-trace.json")
    instrumentation = trace["instrumentation"]
    memory_region = instrumentation.get("memory_region", "ewram")
    memory_address = int(instrumentation.get("memory_log_address", instrumentation["ewram_log_address"]), 16)
    memory_bytes = instrumentation.get("memory_log_bytes", 0x1000)
    memory_base_address = 0x03000000 if memory_region == "iwram" else 0x02000000
    diagnostic_base = memory_address - memory_base_address
    diagnostic_end = diagnostic_base + memory_bytes
    base_ram, probe_ram = raw(baseline, ".ewram"), raw(probe, ".ewram")
    assert len(base_ram) == len(probe_ram) == 0x40000
    ewram_diff = [i for i, (a, b) in enumerate(zip(base_ram, probe_ram)) if a != b]
    base_iwram, probe_iwram = raw(baseline, ".03000000.bin"), raw(probe, ".03000000.bin")
    assert len(base_iwram) == len(probe_iwram) == 0x8000
    iwram_changed = [i for i, (a, b) in enumerate(zip(base_iwram, probe_iwram)) if a != b]
    iwram_diff = len(iwram_changed)
    if memory_region == "ewram":
        outside = [i for i in ewram_diff if not diagnostic_base <= i < diagnostic_end]
        assert not outside, outside[:16]
        iwram_outside = iwram_changed
    else:
        outside = ewram_diff
        iwram_outside = [i for i in iwram_changed if not diagnostic_base <= i < diagnostic_end]
    vram_diff = sum(a != b for a, b in zip(raw(baseline, ".06000000.bin"), raw(probe, ".06000000.bin")))

    assert trace["instrumentation"]["kind"] == "bounded_thumb_follow_entry_and_single_return_state_stream"
    events = trace["call_events"]
    assert len(events) == trace["record_count"] > 0
    packed_by_tick = {row["tick"]: row for row in base_packed["records"]}
    events_by_tick = defaultdict(list)
    actor_input_matches = 0
    actor_output_vblank_matches = 0
    next_call_player_matches = 0
    next_call_globals_matches = 0
    last_call_player_snapshot_relations = Counter()
    last_call_by_tick = {}

    for event in events:
        tick = event["observed_after_core_tick"]
        assert 0 <= tick < len(base_rows)
        assert event["source_counter"] == base_rows[tick]["counter"]
        ptr = int(event["actor_pointer"], 16)
        slot = (ptr - ACTOR_BASE) // ACTOR_STRIDE
        assert 0 <= slot < 50 and (ptr - ACTOR_BASE) % ACTOR_STRIDE == 0
        before, after = bytes.fromhex(event["actor_raw"]), bytes.fromhex(event["actor_after_raw"])
        assert len(before) == len(after) == ACTOR_STRIDE
        if tick > 0:
            previous = bytes.fromhex(base_rows[tick - 1]["traffic_actor_records"]["raw"])
            assert before == previous[slot * ACTOR_STRIDE:(slot + 1) * ACTOR_STRIDE], (tick, slot)
            actor_input_matches += 1
        output_match = False
        for output_tick in (tick, tick + 1):
            if output_tick < len(base_rows):
                pool = bytes.fromhex(base_rows[output_tick]["traffic_actor_records"]["raw"])
                if after == pool[slot * ACTOR_STRIDE:(slot + 1) * ACTOR_STRIDE]:
                    output_match = True
        actor_output_vblank_matches += output_match
        events_by_tick[tick].append(event)
        last_call_by_tick[tick] = event

    for tick, group in events_by_tick.items():
        for current, following in zip(group, group[1:]):
            if (current["player_after_position"], current["player_after_heading"], current["player_after_speed_544"]) == \
                    (following["player_position"], following["player_heading"], following["player_speed_544"]):
                next_call_player_matches += 1
            else:
                raise AssertionError(("player return state does not feed next actor call", tick, current["call_index"]))
            if (current["rng_after_3b04"], current["cooldown_after_e4"], current["near_miss_after_25d3"]) == \
                    (following["rng_3b04"], following["cooldown_e4"], following["near_miss_25d3"]):
                next_call_globals_matches += 1
            else:
                raise AssertionError(("global return state does not feed next actor call", tick, current["call_index"]))
        last = group[-1]
        after_player = (last["player_after_position"], last["player_after_heading"], last["player_after_speed_544"])
        relations = []
        for offset, name in ((0, "same_index_vblank"), (1, "next_index_vblank")):
            packed_row = packed_by_tick.get(tick + offset)
            if packed_row and after_player == player_snapshot(packed_row):
                relations.append(name)
        last_call_player_snapshot_relations["+".join(relations) if relations else "no_match"] += 1
        assert relations, ("last routine return not represented in neighboring VBlank snapshots", tick)

    target = [e for e in events if e["source_counter"] == expected_counter]
    assert target and target[0]["actor_pointer"] == "0x20000f4"
    if expected_counter == 4286:
        assert len(target) >= 6
        assert target[0]["player_speed_544"] == 22140 and target[0]["player_after_speed_544"] == 5535
        assert all(e["player_speed_544"] == 5535 for e in target[1:6])
        assert target[0]["actor_after_raw"][:8] == "00000000"

    result = {
        "status": "PASS_PAIRED_ENTRY_RETURN_GAMEPLAY_TRACE_WITH_AUXILIARY_RAM_DIFFS" if outside or iwram_outside else "PASS_PAIRED_PER_CALL_ENTRY_RETURN_CAPTURE",
        "baseline_label": baseline,
        "probe_label": probe,
        "audited_source_counter": expected_counter,
        "rom_sha256": trace["instrumentation"]["source_rom_sha256"],
        "instrumented_rom_sha256": trace["instrumentation"]["instrumented_rom_sha256"],
        "vblank_rows": len(base_rows),
        "actor_trace_identical": True,
        "packed_driving_trace_identical": True,
        "paired_50_actor_pool_rows_identical": True,
        "per_actor_entry_return_records": len(events),
        "source_ticks_with_calls": len(events_by_tick),
        "calls_per_tick_histogram": dict(sorted(Counter(map(len, events_by_tick.values())).items())),
        "entry_actor_matches_previous_vblank": actor_input_matches,
        "actor_return_matches_same_or_next_vblank": actor_output_vblank_matches,
        "adjacent_call_player_return_feeds_next_entry": next_call_player_matches,
        "adjacent_call_globals_return_feed_next_entry": next_call_globals_matches,
        "last_call_player_return_snapshot_relation": dict(last_call_player_snapshot_relations),
        "counter_4286_call_sequence": [
            {"actor_pointer": e["actor_pointer"], "player_speed_before": e["player_speed_544"],
             "player_speed_after": e["player_after_speed_544"], "actor_flags_before": e["actor_raw"][:8],
             "actor_flags_after": e["actor_after_raw"][:8]} for e in target
        ],
        "ewram_diff_bytes": len(ewram_diff),
        "ewram_diff_outside_diagnostic_buffer": len(outside),
        "iwram_diff_outside_diagnostic_buffer": len(iwram_outside),
        "diagnostic_buffer": {"address": hex(memory_address), "region": memory_region, "bytes": memory_bytes},
        "iwram_changed_bytes": iwram_diff,
        "vram_changed_bytes": vram_diff,
        "scope": "Original-ROM diagnostic. Exact per-call input and return snapshots are paired at the sole 0x08006908 exit. All actor/global return fields feed the next actor entry within a dispatcher pass; final player state maps to the same or next VBlank. Gameplay actor/driving traces match the baseline, but IWRAM/VRAM differences mean the probe is not cycle-neutral. This does not prove a completed native port.",
    }
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result))


if __name__ == "__main__":
    main()
