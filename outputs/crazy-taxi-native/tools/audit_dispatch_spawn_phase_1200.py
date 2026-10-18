#!/usr/bin/env python3
"""Pair original spawn RNG-call timing to a clean 1,200-frame actor replay."""
from __future__ import annotations
import gzip, hashlib, json, struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
REF = ROOT / "outputs/crazy-taxi-native/reports/original-core-reference"
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
FIELDS = ("tick", "input_keys", "keyinput", "heading", "speed", "counter", "previous",
    "frame_delta", "delta", "intro", "position", "angles", "orientation", "gravity",
    "airborne", "raw_input_history", "driver_owners", "traffic_actor_records",
    "traffic_pool_changed_from_previous_vblank", "traffic_player_state", "traffic_globals",
    "traffic_frame_context", "trip_clock_state", "trip_start_inputs")

def load(name: str):
    return json.loads((REF / name).read_text(encoding="utf-8"))

def raw_pool(row: dict) -> bytes:
    return bytes.fromhex(row["traffic_actor_records"]["raw"])

def main() -> None:
    clean_doc = load("dispatchclean1200.json")
    probe_doc = load("dispatchcleanrngspawn1200.json")
    with gzip.open(REF / "dispatchclean1200.actor-trace.json.gz", "rt", encoding="utf-8") as f:
        clean = json.load(f)
    probe = load("dispatchcleanrngspawn1200.actor-trace.json")
    rng = load("dispatchcleanrngspawn1200.rng-callers.json")
    dispatch = load("callbackdispatchminimal1200.traffic-dispatch-entry-trace.json")
    assert clean_doc["status"] == probe_doc["status"] == "PASS"
    assert clean_doc["frames"] == probe_doc["frames"] == len(clean) == len(probe) == 1200
    assert clean_doc["rom_sha256"] == probe_doc["rom_sha256"] == ROM_SHA
    assert clean_doc["core_sha256"] == probe_doc["core_sha256"]
    assert clean_doc["direct_input_schedule"]["sha256"] == probe_doc["direct_input_schedule"]["sha256"]
    assert rng["logged_count"] == len(rng["call_events"]) == 10 and not rng["log_saturated"]
    assert rng["instrumentation"]["call_sites"] == ["0x8006cb0"]
    assert dispatch["logged_count"] == len(dispatch["call_events"]) and not dispatch["log_saturated"]

    mismatches = []
    for tick, (expected, actual) in enumerate(zip(clean, probe)):
        differing = [key for key in FIELDS if expected.get(key) != actual.get(key)]
        if expected.get("traffic_camera", {}).get("position") != actual.get("traffic_camera", {}).get("position"):
            differing.append("traffic_camera.position")
        if differing:
            detail = {"tick": tick, "fields": differing}
            if "traffic_actor_records" in differing:
                a, b = raw_pool(expected), raw_pool(actual)
                slots = []
                for slot in range(50):
                    old, new = a[slot * 92:(slot + 1) * 92], b[slot * 92:(slot + 1) * 92]
                    if old != new:
                        fields = [off for off in range(0, 92, 4)
                                  if old[off:off + 4] != new[off:off + 4]]
                        slots.append({"slot": slot, "changed_offsets": fields})
                detail["actor_differences"] = slots
            mismatches.append(detail)

    events = []
    for event in rng["call_events"]:
        tick = int(event["tick"])
        row = clean[tick]
        previous = clean[tick - 1] if tick else None
        born = []
        if previous:
            before, after = raw_pool(previous), raw_pool(row)
            born = [slot for slot in range(50)
                    if not struct.unpack_from("<I", before, slot * 92)[0] & 1
                    and struct.unpack_from("<I", after, slot * 92)[0] & 1]
        entries = [e for e in dispatch["call_events"] if e["observed_after_core_tick"] <= tick]
        last = entries[-1] if entries else None
        events.append({"tick": tick, "source_counter": row["counter"],
            "call_site": event["call_site"], "rng_before": event["rng_before"],
            "newly_active_actor_slots_at_vblank": born,
            "latest_dispatch_entry": None if last is None else {
                "source_counter": last["source_counter"],
                "observed_after_core_tick": last["observed_after_core_tick"]},
            "counter_offset_after_entry": None if last is None else row["counter"] - last["source_counter"]})

    assert [x["tick"] for x in events] == [55, 210, 235, 415, 772, 772, 956, 961, 1026, 1101]
    assert [(e["source_counter"], e["counter_offset_after_entry"]) for e in events[:4]] == [
        (4251, 0), (4406, 0), (4431, 0), (4611, 1)]
    assert len(mismatches) == 2 and mismatches[0]["tick"] == 961 and mismatches[1]["tick"] == 962
    assert mismatches[0]["actor_differences"] == [{"slot": 19, "changed_offsets": [16]}]
    assert mismatches[1]["fields"] == ["traffic_pool_changed_from_previous_vblank"]
    assert raw_pool(clean[962]) == raw_pool(probe[962])

    report = {
        "status": "PASS_SPAWN_PHASE_EVENTS_WITH_QUANTIFIED_HOOK_LAG",
        "source_rom_sha256": ROM_SHA,
        "source_core_sha256": clean_doc["core_sha256"],
        "input_schedule_sha256": clean_doc["direct_input_schedule"]["sha256"],
        "source_rows": len(clean),
        "rng_hook_callsite": "0x08006CB0",
        "rng_call_event_count": len(events),
        "spawn_rng_events": events,
        "probe_vs_clean_mismatch_count": len(mismatches),
        "probe_vs_clean_mismatches": mismatches,
        "finding": "The 10 source RNG calls at 0x08006CB0 are traffic-spawn type selections. At slot6's tick55 event, the counter matches dispatcher entry4251; slot7's tick235 event matches entry4431; slot8's tick415 event is one counter after entry4610. The slot7 and slot8 helper returns are separately captured by the slot-filtered 0x080067B8 probes. This pattern is consistent with the dispatcher reaching later actor work across a frame boundary, but three slots alone do not establish a general cycle-cost rule. The 1,200-frame RNG hook preserves all compared source fields except a one-frame slot19 countdown lag at tick961; full actor bytes match again at tick962, so events at/after this perturbation are retained as diagnostic observations, not clean-state parity proof.",
        "scope": "Original-ROM source diagnostics paired to a clean fixed-input trace; not a native gameplay fix, complete dispatcher parity, or playable-port claim.",
        "next_dependency": "Measure dispatcher-to-spawn-helper elapsed cycles at multiple actor slots and actor-state combinations with a timing-checked probe; derive the source phase rule and validate a native implementation against the clean 1,200-frame trace, preserving the slot19 hook-lag caveat.",
    }
    (REF / "dispatch-spawn-phase-1200-audit.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("status", "rng_call_event_count", "spawn_rng_events", "probe_vs_clean_mismatch_count", "probe_vs_clean_mismatches", "next_dependency")}, indent=2))

if __name__ == "__main__":
    main()
