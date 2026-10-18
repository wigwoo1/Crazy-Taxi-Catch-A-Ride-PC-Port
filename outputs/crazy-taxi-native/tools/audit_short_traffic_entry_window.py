"""Audit a short source-ROM traffic-callback trace against its untouched run."""
from __future__ import annotations

import hashlib
import json
import pathlib
import subprocess
import struct

ROOT = pathlib.Path("outputs/crazy-taxi-native/reports/original-core-reference")
BASE = "seed4200baseline"
PROBE = "seed4200followprobe"
OUT = ROOT / "seed4200-short-traffic-entry-audit.json"
PROJECT = ROOT.parents[1]


def load(name: str):
    return json.loads((ROOT / name).read_text())


def sha(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


baseline = load(BASE + ".actor-trace.json")
probe = load(PROBE + ".actor-trace.json")
trace = load(PROBE + ".traffic-follow-entry-trace.json")
assert len(baseline) == len(probe) == 96

fields = (
    "input_keys", "keyinput", "heading", "speed", "counter", "previous",
    "frame_delta", "delta", "intro", "position", "angles", "orientation",
    "gravity", "airborne", "raw_input_history", "traffic_camera",
    "driver_owners", "traffic_actor_records", "traffic_globals",
    "traffic_frame_context", "completed_count", "fare_whole", "fare_fraction",
    "remaining_clock",
)
for index, (before, after) in enumerate(zip(baseline, probe)):
    differences = [field for field in fields if before[field] != after[field]]
    assert not differences, ("instrumentation changed source state", index, differences)

event = next(
    item for item in trace["call_events"]
    if item["source_counter"] == 4286 and item["actor_pointer"] == "0x20000f4"
)
same_counter = [item for item in trace["call_events"] if item["source_counter"] == 4286]
assert len(same_counter) == 6
assert event["observed_entry_tick"] == event["observed_return_tick"] == 89
assert event["player_speed_544"] == 22140
assert event["player_after_speed_544"] == 5535
assert event["cooldown_e4"] == 0 and event["cooldown_after_e4"] == 60
assert event["rng_3b04"] == 1555287050 and event["rng_after_3b04"] == 1136748202

frame = probe[89]
assert frame["counter"] == 4286
assert frame["position"] == event["player_after_position"]
assert frame["heading"] == event["player_after_heading"]
assert frame["speed"] == event["player_after_speed_544"]
assert frame["traffic_globals"]["cooldown_e4"] == event["cooldown_after_e4"]
assert frame["traffic_globals"]["rng_3b04"] == event["rng_after_3b04"]
actor0_after = bytes.fromhex(event["actor_after_raw"])
source_pool = bytes.fromhex(frame["traffic_actor_records"]["raw"])
assert len(source_pool) == 50 * 92 and actor0_after == source_pool[:92]

before_checkpoint = ROOT / "route31fullram.ewram-vblank-00085.bin"
after_checkpoint = ROOT / "route31fullram.ewram-vblank-00090.bin"
assert bytes.fromhex(event["actor_raw"]) == before_checkpoint.read_bytes()[0xF4:0xF4 + 92]
assert actor0_after == after_checkpoint.read_bytes()[0xF4:0xF4 + 92]
native_exe = PROJECT / "bin/route31-traffic-event-probe.exe"
native_run = subprocess.run(
    [str(native_exe), str(before_checkpoint), str(after_checkpoint),
     str(PROJECT / "assets/traffic/city1-road-131.ctrp"),
     str(PROJECT / "assets/driving.ctr")],
    check=True, capture_output=True, text=True,
)
assert "exact_actor=yes event_match=yes" in native_run.stdout, native_run.stdout

files = [
    ROOT / (BASE + ".actor-trace.json"),
    ROOT / (PROBE + ".actor-trace.json"),
    ROOT / (PROBE + ".traffic-follow-entry-trace.json"),
    ROOT / (PROBE + ".state"),
    ROOT / (PROBE + ".ewram"),
    ROOT / "seedcheckb4200.state",
    before_checkpoint,
    after_checkpoint,
    PROJECT / "tools/audit_short_traffic_entry_window.py",
    native_exe,
]
report = {
    "status": "PASS_SHORT_WINDOW_EXACT_TRAFFIC_CALLBACK",
    "scope": "Independent mGBA original-ROM probe. It validates one bounded callback event and a 96-frame observationally exact window; it does not establish full traffic dispatch, native gameplay parity, or a completed port.",
    "rom_sha256": trace["instrumentation"]["source_rom_sha256"],
    "baseline": BASE,
    "instrumented_capture": PROBE,
    "matched_frames": len(baseline),
    "matched_fields_per_frame": list(fields),
    "actor_pool_slots": 50,
    "exact_actor0_entry_and_return_match_prior_vblank_checkpoints": True,
    "strict_native_callback_probe": {
        "status": "PASS",
        "stdout": native_run.stdout.strip(),
        "binary_sha256": sha(native_exe),
        "scope": "The existing strict standalone native callback probe reproduces the matching route131 actor record, path, collision, and taxi speed from the exact source boundary. It uses a minimal synthetic gear seed and does not establish general dispatch or full powertrain state parity.",
    },
    "follow_calls": trace["record_count"],
    "calls_at_source_counter_4286": [
        {
            "call_index": item["call_index"],
            "slot": (int(item["actor_pointer"], 16) - 0x020000F4) // 0x5C,
            "actor_pointer": item["actor_pointer"],
            "speed_before": item["player_speed_544"],
            "speed_after": item["player_after_speed_544"],
            "actor_changed": item["actor_raw"] != item["actor_after_raw"],
        }
        for item in same_counter
    ],
    "first_collision_event": {
        "call_index": event["call_index"],
        "entry_and_return_tick": event["observed_entry_tick"],
        "actor_slot": 0,
        "actor_pointer": event["actor_pointer"],
        "argument_r1": event["argument_r1"],
        "position": event["player_position"],
        "heading": event["player_heading"],
        "speed_544_before_after": [event["player_speed_544"], event["player_after_speed_544"]],
        "cooldown_before_after": [event["cooldown_e4"], event["cooldown_after_e4"]],
        "rng_before_after": [event["rng_3b04"], event["rng_after_3b04"]],
        "actor0_after_matches_full_pool_capture": True,
        "pool_actor_count": 50,
    },
    "artifact_sha256": {path.name: sha(path) for path in files},
    "project_progress": {
        "estimate_percent": 30,
        "confidence": "low",
        "rough_days_remaining": "180-365",
        "playable_native_port_complete": False,
    },
}
OUT.write_text(json.dumps(report, indent=2) + "\n")
print(json.dumps({"status": report["status"], "matched_frames": len(baseline),
                  "follow_calls": trace["record_count"],
                  "speed_change": [event["player_speed_544"], event["player_after_speed_544"]],
                  "output": str(OUT)}))
