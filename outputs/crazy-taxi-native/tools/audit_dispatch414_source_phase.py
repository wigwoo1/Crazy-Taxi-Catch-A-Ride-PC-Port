#!/usr/bin/env python3
"""Validate clean and instrumented ROM evidence at dispatcher ticks 414-419.

The report deliberately separates source-observation validity from native
parity: the voice-store probe is checked against the uninstrumented trace, and
the continuous native mismatch is retained as an open dependency.
"""
from __future__ import annotations
import argparse, gzip, hashlib, json, struct
from pathlib import Path

ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
SOURCE_KEYS = (
    "tick", "input_keys", "keyinput", "heading", "speed", "counter", "previous",
    "frame_delta", "delta", "intro", "position", "angles", "orientation", "gravity",
    "airborne", "raw_input_history", "traffic_camera", "driver_owners",
    "traffic_actor_records", "traffic_pool_changed_from_previous_vblank", "traffic_globals",
    "traffic_frame_context", "trip_clock_state", "trip_start_inputs",
)

def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def load(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))

def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--project-root", type=Path, required=True)
    ap.add_argument("--output", type=Path, required=True)
    a = ap.parse_args()
    root = a.project_root / "outputs/crazy-taxi-native"
    reports = root / "reports/original-core-reference"
    clean_doc = load(reports / "dispatch414clean420.json")
    clean = load(reports / "dispatch414clean420.actor-trace.json")
    voice_doc = load(reports / "dispatch414voiceprobe420.json")
    voice = load(reports / "dispatch414voiceprobe420.actor-trace.json")
    voice_writes = load(reports / "dispatch414voiceprobe420.voice-reset-writes.json")
    dispatch_doc = load(reports / "dispatch414trafficentry420.json")
    dispatch_trace = load(reports / "dispatch414trafficentry420.traffic-dispatch-entry-trace.json")
    with gzip.open(reports / "dispatchclean1200.actor-trace.json.gz", "rt", encoding="utf-8") as f:
        baseline = json.load(f)
    native = load(reports / "dispatchclean-recheck-after-revert.json")
    rom = Path(r"Crazy Taxi - Catch a Ride (USA).gba")

    identity_ok = (
        clean_doc.get("status") == "PASS" and clean_doc.get("frames") == 420
        and clean_doc.get("rom_sha256") == ROM_SHA and clean_doc.get("instrumentation") is None
        and voice_doc.get("status") == "PASS" and voice_doc.get("frames") == 420
        and voice_doc.get("rom_sha256") == ROM_SHA
        and clean_doc.get("core_sha256") == voice_doc.get("core_sha256")
        and dispatch_doc.get("status") == "PASS" and dispatch_doc.get("frames") == 420
        and dispatch_doc.get("rom_sha256") == ROM_SHA
        and len(clean) == len(voice) == 420 and len(baseline) >= 420
    )
    clean_matches_baseline = identity_ok and all(
        all(clean[i].get(k) == baseline[i].get(k) for k in SOURCE_KEYS)
        for i in range(420)
    )
    voice_matches_clean = identity_ok and all(
        all(voice[i].get(k) == clean[i].get(k) for k in SOURCE_KEYS)
        for i in range(420)
    )
    write_events = voice_writes.get("call_events", [])
    reset = [e for e in write_events if e.get("target") == "0x20025d6"
             and e.get("source_counter") == 4610]
    reset_ok = (
        voice_writes.get("logged_count") == 1 and not voice_writes.get("log_saturated")
        and len(reset) == 1 and reset[0].get("write_value") == 255
        and reset[0].get("before_value") == 18
        and reset[0].get("observed_after_core_tick") == 414
        and voice_writes.get("instrumentation", {}).get("source_rom_sha256") == ROM_SHA
    )
    events = dispatch_trace.get("call_events", [])
    before = [e for e in events if e.get("source_counter") == 4610]
    after = [e for e in events if e.get("source_counter") == 4615]
    slot8_gate = bool(before and after and
        before[0]["slot8"] == {"flags": 0, "type": 0, "countdown": 3, "y": 0} and
        after[0]["slot8"] == {"flags": 1, "type": 1, "countdown": -2, "y": 19200})
    step85 = native["comparisons"][85]
    step86 = native["comparisons"][86]
    native_state = {
        "status": native.get("status"),
        "exact_boundaries": len(native.get("exact_all_state_match_updates", [])),
        "last_exact_step": native.get("exact_all_state_match_updates", [])[-1]
            if native.get("exact_all_state_match_updates") else None,
        "first_mismatch_tick": step85.get("source_tick"),
        "first_mismatch_counter": step85.get("source_counter"),
        "tick414_actor_mismatch_slots": [x["slot"] for x in step85.get("actor_mismatches", [])],
        "tick414_global_mismatches": step85.get("traffic_global_mismatches", {}),
        "tick419_actor_match_count": step86.get("actor_match_count"),
        "tick419_global_mismatches": step86.get("traffic_global_mismatches", {}),
    }
    checks = {
        "rom_identity_and_uninstrumented_clean_capture": identity_ok,
        "clean_420_matches_accepted_1200_trace": clean_matches_baseline,
        "voice_hook_preserves_all_compared_source_rows_through_419": voice_matches_clean,
        "voice_reset_store_at_counter4610_is_exact": reset_ok,
        "traffic_dispatch_entry_shows_slot8_transition_4610_to_4615": slot8_gate,
        "native_replay_remains_diagnostic_with_open_tick414_mismatch": (
            native_state["status"] == "PASS_DIAGNOSTIC_EXECUTION"
            and native_state["exact_boundaries"] == 85
            and native_state["first_mismatch_tick"] == 414
            and native_state["tick419_actor_match_count"] == 50
        ),
    }
    original_rom_hash = sha(rom.read_bytes()) if rom.exists() else None
    report = {
        "status": "PASS_SOURCE_EVIDENCE_NATIVE_GAP_REMAINS" if all(checks.values()) and original_rom_hash == ROM_SHA else "FAIL",
        "source_rom_sha256_expected": ROM_SHA,
        "source_rom_sha256_current": original_rom_hash,
        "source_capture_hashes": {
            name: sha((reports / name).read_bytes()) for name in (
                "dispatch414clean420.json", "dispatch414clean420.actor-trace.json",
                "dispatch414voiceprobe420.voice-reset-writes.json",
                "dispatch414trafficentry420.traffic-dispatch-entry-trace.json",
                "dispatchclean-recheck-after-revert.json",
            )
        },
        "source_boundary": {
            "tick414_counter": clean[414]["counter"],
            "tick414_voice": clean[414]["traffic_globals"]["voice_25d6"],
            "tick413_voice": clean[413]["traffic_globals"]["voice_25d6"],
            "voice_reset_event": reset[0] if reset else None,
            "slot8_entry_4610": before[0]["slot8"] if before else None,
            "slot8_entry_4615": after[0]["slot8"] if after else None,
        },
        "native_replay": native_state,
        "checks": checks,
        "interpretation": "Source voice reset and slot-8 dispatcher observations are verified against an uninstrumented 420-row replay. The probe changes only in-memory ROM and its compared input, player and actor records match the clean trace through tick419. Native remains exactly matched for 85 dispatcher boundaries; slot8 spawn selection/counter phase at tick414 and native voice cleanup remain unresolved. No blanket spawn-delay change is accepted.",
        "next_dependency": "Recover the original slot-8 spawn-selection gate that prevents the native counter/RNG/actor transition at tick414, while retaining earlier slot-6 spawn parity; separately connect source voice-sentinel cleanup to native audio-context completion.",
        "full_playable_port": False,
        "production_gameplay_changed": False,
    }
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "checks": checks, "native_replay": native_state}, indent=2))
    return 0 if report["status"] != "FAIL" else 2

if __name__ == "__main__":
    raise SystemExit(main())
