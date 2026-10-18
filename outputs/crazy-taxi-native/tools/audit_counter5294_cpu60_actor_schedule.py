#!/usr/bin/env python3
"""Audit 60 uninstrumented source frames against actor-pool and CPU snapshots."""
from __future__ import annotations
import hashlib, json, pathlib, statistics, struct

ROOT = pathlib.Path(__file__).resolve().parents[1]
REF = ROOT / "reports/original-core-reference"
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
CORE_SHA = "73411273b6b9bb99e22f2278f8da79f92feb3b80a007a21e18262a915a0f84d4"
STATE_SHA = "a20a48db7c4514d28c072de7ad80ed1576d77076a06d5e7b6c038ce8b0404a01"
STRIDE, COUNT = 0x5C, 50

def read(name): return json.loads((REF / name).read_text(encoding="utf-8"))
def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def comparable(value):
    if isinstance(value, dict):
        return {k: comparable(v) for k, v in value.items() if k != "raw_40"}
    if isinstance(value, list): return [comparable(v) for v in value]
    return value

def pool(row):
    raw = bytes.fromhex(row["traffic_actor_records"]["raw"])
    assert len(raw) == STRIDE * COUNT
    return raw

def main():
    name = "r5294cpu60pool"
    baseline = read("r5294plain3.actor-trace.json")
    rows = read(name + ".actor-trace.json")
    run = read(name + ".json")
    route = read(name + ".route-follow.json")
    cpu_path = REF / (name + ".cpu-timing-trace.json")
    cpu = json.loads(cpu_path.read_text(encoding="utf-8"))
    assert run["status"] == "PASS" and run["frames"] == len(rows) == 60
    assert run["rom_sha256"] == route["rom_sha256"] == ROM_SHA
    assert run["core_sha256"] == route["core_sha256"] == CORE_SHA
    assert run["instrumentation"] is None
    assert run["traffic_dispatch_instrumentation"] is None
    assert run["traffic_follow_entry_instrumentation"] is None
    assert route["state_sha256"] == STATE_SHA
    assert len(baseline) == 3 and rows[0]["counter"] == 5294
    assert comparable({k:v for k,v in rows[0].items() if k in baseline[0] and k != "route_driver"}) == comparable({k:v for k,v in baseline[0].items() if k != "route_driver"})
    records = cpu["records"]
    assert [r["tick"] for r in records] == list(range(-1, 60))
    assert len(records) == 61
    by_tick = {r["tick"]: r for r in records}

    per_tick, prior_pool = [], None
    changed_ticks, updated_pcs, idle_pcs, cycle_gaps = [], set(), set(), []
    for i, row in enumerate(rows):
        current = pool(row)
        reported = row.get("traffic_pool_changed_from_previous_vblank") or []
        assert all(0 <= slot < COUNT for slot in reported)
        if prior_pool is None:
            actual = reported
            delta_verified = False
        else:
            actual = [slot for slot in range(COUNT)
                      if prior_pool[slot*STRIDE:(slot+1)*STRIDE] != current[slot*STRIDE:(slot+1)*STRIDE]]
            assert actual == reported, (row["tick"], actual, reported)
            delta_verified = True
        timer_only = []
        non_timer = []
        for slot in actual:
            if prior_pool is None:
                continue
            old = prior_pool[slot*STRIDE:(slot+1)*STRIDE]
            new = current[slot*STRIDE:(slot+1)*STRIDE]
            diffs = [j for j,(a,b) in enumerate(zip(old,new)) if a != b]
            if diffs and all(0x10 <= j < 0x14 for j in diffs):
                before = struct.unpack_from("<i", old, 0x10)[0]
                after = struct.unpack_from("<i", new, 0x10)[0]
                if before > 0 and after == before - row["frame_delta"]:
                    timer_only.append(slot); continue
            non_timer.append(slot)
        snap = by_tick[row["tick"]]
        pc = snap["gprs"][15]
        (updated_pcs if actual else idle_pcs).add(pc)
        if actual: changed_ticks.append(row["tick"])
        if i:
            cycle_gaps.append(snap["master_cycles"] - by_tick[row["tick"]-1]["master_cycles"])
        per_tick.append({"tick": row["tick"], "source_counter": row["counter"],
                         "frame_delta": row["frame_delta"], "changed_slots": actual,
                         "delta_verified_from_adjacent_raw_snapshot": delta_verified,
                         "positive_timer_only_slots": timer_only, "other_changed_slots": non_timer,
                         "vblank_return_pc": hex(pc)})
        prior_pool = current

    assert route["completed_count"] == 0 and route["fare"] == [0,0]
    overlaps = sorted(updated_pcs & idle_pcs)
    report = {
        "status": "PASS_UNINSTRUMENTED_60_FRAME_ACTOR_POOL_CPU_CORRELATION",
        "source_rom_sha256": ROM_SHA, "core_sha256": CORE_SHA, "input_state_sha256": STATE_SHA,
        "run_report": name + ".json", "actor_trace": name + ".actor-trace.json",
        "cpu_trace": cpu_path.name, "actor_trace_sha256": sha(REF/(name+".actor-trace.json")),
        "cpu_trace_sha256": sha(cpu_path), "frames": 60, "rom_instrumentation": None,
        "start_row_matches_existing_plain_capture_excluding_route_driver_and_optional_raw_camera_bytes": True,
        "cpu_snapshots": len(records), "actor_pool_record_bytes": STRIDE * COUNT,
        "changed_ticks": changed_ticks, "vblank_return_pc_unique_count": len({by_tick[t]["gprs"][15] for t in range(60)}),
        "return_pc_values_seen_on_both_update_and_no_update_ticks": [hex(x) for x in overlaps],
        "cpu_master_cycle_delta_min_median_max": [min(cycle_gaps), int(statistics.median(cycle_gaps)), max(cycle_gaps)],
        "route_result": {"final_waypoint": route["final_waypoint"], "completed_count": route["completed_count"],
                         "fare": route["fare"], "remaining_clock": route["remaining_clock"]},
        "per_tick": per_tick,
        "interpretation": "This no-hook 60-frame replay validates all 59 post-initial 50-actor-pool deltas against adjacent raw snapshots; the first row's changed-slot list is retained from the capture but has no pre-run raw actor pool in this trace. It supplies a VBlank-return CPU checkpoint for every frame. Actor updates recur in paired VBlanks with several different slot split points, followed by inactive intervals; the returned PCs vary and at least one PC is shared by update and no-update ticks. These frame-boundary snapshots do not expose the instruction owner or exact sub-VBlank phase, so they narrow cadence but do not close dispatcher ownership or establish native frame-service parity.",
        "scope": "Source-core diagnostic from the saved counter-5294 pickup state, with the fixed existing scripted route driver. Fare remains zero; not a playable game or human-control result. CPU cycle deltas are emulator VBlank-return measurements, not native presentation FPS."
    }
    output = REF / "counter5294-uninstrumented-60frame-actor-cpu-audit.json"
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "frames": 60, "changed_ticks": changed_ticks,
                      "unique_return_pcs": report["vblank_return_pc_unique_count"],
                      "pc_overlap": report["return_pc_values_seen_on_both_update_and_no_update_ticks"],
                      "report": str(output)}, indent=2))

if __name__ == "__main__": main()

