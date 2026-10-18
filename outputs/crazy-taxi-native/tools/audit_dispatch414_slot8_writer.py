#!/usr/bin/env python3
"""Validate the tick414 slot8 store hook against a clean source replay."""
from __future__ import annotations
import hashlib, json, struct
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
REF = ROOT / "outputs/crazy-taxi-native/reports/original-core-reference"
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
SOURCE_KEYS = ("tick", "input_keys", "keyinput", "heading", "speed", "counter",
    "previous", "frame_delta", "delta", "intro", "position", "angles",
    "orientation", "gravity", "airborne", "raw_input_history", "driver_owners",
    "traffic_actor_records", "traffic_pool_changed_from_previous_vblank",
    "traffic_player_state", "traffic_globals", "traffic_frame_context",
    "trip_clock_state", "trip_start_inputs")

def load(name: str):
    return json.loads((REF / name).read_text(encoding="utf-8"))

def slot8(row: dict) -> dict:
    raw = bytes.fromhex(row["traffic_actor_records"]["raw"])
    off = 8 * 0x5c
    return {"flags": struct.unpack_from("<I", raw, off)[0],
            "type": struct.unpack_from("<I", raw, off + 4)[0],
            "countdown": struct.unpack_from("<i", raw, off + 16)[0],
            "y": struct.unpack_from("<i", raw, off + 24)[0]}

def main() -> None:
    clean_doc = load("dispatch414clean420.json")
    probe_doc = load("dispatch414slot8full.json")
    clean = load("dispatch414clean420.actor-trace.json")
    probe = load("dispatch414slot8full.actor-trace.json")
    hook = load("dispatch414slot8full.traffic-slot-y-write-trace.json")
    spawn_hook = load("dispatch414slot8spawn.traffic-spawn-return-trace.json")
    slot6_probe = load("dispatch414slot6spawn.actor-trace.json")
    slot6_hook = load("dispatch414slot6spawn.traffic-spawn-return-trace.json")
    assert clean_doc["rom_sha256"] == probe_doc["rom_sha256"] == ROM_SHA
    assert clean_doc["core_sha256"] == probe_doc["core_sha256"]
    assert clean_doc["instrumentation"] is None and len(clean) == len(probe) == 420
    assert hook["rom_sha256"] == ROM_SHA and hook["core_sha256"] == clean_doc["core_sha256"]
    assert hook["instrumentation"]["actor_slot"] == 8
    assert hook["instrumentation"]["actor_pointer"] == "0x20003d4"
    def compare(trace):
        mismatches = []
        for tick, (expected, actual) in enumerate(zip(clean, trace)):
            differing = [key for key in SOURCE_KEYS if expected.get(key) != actual.get(key)]
            if expected.get("traffic_camera", {}).get("position") != actual.get("traffic_camera", {}).get("position"):
                differing.append("traffic_camera.position")
            if differing:
                mismatches.append({"tick": tick, "fields": differing})
        return mismatches
    mismatches = compare(probe)
    slot6_mismatches = compare(slot6_probe)
    events = hook["events"]
    spawn_events = spawn_hook["events"]
    slot6_events = slot6_hook["events"]
    target = [e for e in events if e["source_counter"] == 4611]
    clean414, clean415 = slot8(clean[414]), slot8(clean[415])
    assert hook["record_count"] == len(events) == 2
    assert len(target) == 1 and target[0]["matching_vblank_ticks"] == [415]
    assert (target[0]["written_y"], target[0]["flags"], target[0]["route_pointer"]) == (19200, 1, "0x85bc3a4")
    assert clean414 == {"flags": 0, "type": 0, "countdown": -2, "y": 0}
    assert clean415 == {"flags": 1, "type": 1, "countdown": -2, "y": 19200}
    assert clean[414]["traffic_globals"]["selection_counter_4c98"] == 12
    assert clean[415]["traffic_globals"]["selection_counter_4c98"] == 13
    assert clean[414]["traffic_globals"]["rng_3b04"] == 1899946468
    assert clean[415]["traffic_globals"]["rng_3b04"] == 2046486665
    assert spawn_hook["rom_sha256"] == slot6_hook["rom_sha256"] == ROM_SHA
    assert spawn_hook["instrumentation"]["actor_slot"] == 8
    assert slot6_hook["instrumentation"]["actor_slot"] == 6
    slot8_spawn = [e for e in spawn_events if e["source_counter"] == 4611]
    slot6_spawn = [e for e in slot6_events if e["source_counter"] == 4251]
    assert spawn_hook["record_count"] == len(spawn_events) == 1
    assert slot6_hook["record_count"] == len(slot6_events) == 1
    assert slot8_spawn[0]["matching_vblank_ticks"] == [415]
    assert (slot8_spawn[0]["y"], slot8_spawn[0]["flags"], slot8_spawn[0]["countdown"]) == (0, 1, -2)
    assert slot6_spawn[0]["matching_vblank_ticks"] == [55]
    assert (slot6_spawn[0]["y"], slot6_spawn[0]["flags"], slot6_spawn[0]["countdown"]) == (0, 1, -3)
    report = {
        "status": "PASS_SLOT8_STORE_CAPTURE_CLEAN_PARITY",
        "source_rom_sha256": ROM_SHA,
        "source_core_sha256": clean_doc["core_sha256"],
        "clean_rows_compared": len(clean),
        "instrumented_rows_compared": len(probe),
        "source_field_mismatch_count": len(mismatches),
        "source_field_mismatches": mismatches,
        "slot6_source_field_mismatch_count": len(slot6_mismatches),
        "slot6_source_field_mismatches": slot6_mismatches,
        "instrumentation": hook["instrumentation"],
        "slot8_y_store_at_counter4611": target[0],
        "slot8_original_spawn_return": slot8_spawn[0],
        "slot6_original_spawn_return_control": slot6_spawn[0],
        "source_slot8_tick414": clean414,
        "source_slot8_tick415": clean415,
        "selection_counter_12_to_13": [clean[414]["traffic_globals"]["selection_counter_4c98"], clean[415]["traffic_globals"]["selection_counter_4c98"]],
        "rng_4610_to_4611": [clean[414]["traffic_globals"]["rng_3b04"], clean[415]["traffic_globals"]["rng_3b04"]],
        "finding": "The original 0x08006BC0 spawn-helper return for slot8 is captured at counter4611/VBlank tick415 (Y=0, flags=1, countdown=-2); its 0x08006820 height store follows at the same counter/tick (Y=19200). An independent earlier slot6 control returns at counter4251/VBlank tick55 (Y=0, flags=1, countdown=-3). Both test hooks preserve every compared clean source row (420/420), including the full actor pool, globals, input, driver and camera position. This rules out a blanket one-frame spawn delay: source spawn return timing varies across these two boundaries. It narrows the native mismatch to within-dispatcher event phase, but does not yet establish the scheduling rule or writer order for the selector/RNG.",
        "scope": "Timing-checked source-ROM diagnostics only. The supplied ROM file is read-only; no native production behavior was changed and this is not a playable-port milestone.",
        "next_dependency": "Trace the source selector and RNG writes at counters4251 and4611, then derive a repeatable source event-phase rule that reproduces both slot6 and slot8 without hardcoded slot exceptions.",
    }
    assert not mismatches and not slot6_mismatches
    (REF / "dispatch414-slot8-writer-audit.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("status", "clean_rows_compared", "source_field_mismatch_count", "slot8_y_store_at_counter4611", "source_slot8_tick414", "source_slot8_tick415", "next_dependency")}, indent=2))

if __name__ == "__main__":
    main()
