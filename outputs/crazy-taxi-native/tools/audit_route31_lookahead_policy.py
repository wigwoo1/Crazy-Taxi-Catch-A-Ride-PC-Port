#!/usr/bin/env python3
"""Audit matched route31 lookahead-policy probes; none imply a completed fare."""
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
LABELS = {
    "divisor_8_steer_brake": "route31lookdiv8",
    "divisor_16_steer_brake": "route31baselineexact",
    "divisor_32_steer_brake": "route31div32b",
    "divisor_16_no_steer_brake": "route31baselineNSB",
}

def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()

runs = {}
for name, label in LABELS.items():
    path = REPORTS / f"{label}.route-follow.json"
    data = json.loads(path.read_text())
    runs[name] = {
        "label": label,
        "report_sha256": sha(path),
        "actor_trace_sha256": data["actor_trace_sha256"],
        "rom_sha256": data["rom_sha256"],
        "core_sha256": data["core_sha256"],
        "state_sha256": data["state_sha256"],
        "frames": data["frames"],
        "route_source": data["route_source"],
        "route_start_waypoint": data["route_start_waypoint"],
        "route_waypoint_radius": data["route_waypoint_radius"],
        "route_lookahead": data["route_lookahead"],
        "route_lookahead_speed_divisor": data["route_lookahead_speed_divisor"],
        "route_brake_speed": data["route_brake_speed"],
        "route_steer_during_brake": data["route_steer_during_brake"],
        "first_trip_tick": data["first_trip_tick"],
        "final_waypoint": data["final_waypoint"],
        "trip_active_at_end": data["trip_active_at_end"],
        "completed_count": data["completed_count"],
        "fare": data["fare"],
        "remaining_clock": data["remaining_clock"],
        "capture_status": data["status"],
    }
    assert data["frames"] == 16000
    assert data["route_source"] == 31 and data["first_trip_tick"] == 516
    assert data["completed_count"] == 0 and data["fare"] == [0, 0]
    assert data["state_sha256"] == runs["divisor_8_steer_brake"]["state_sha256"]
    assert data["core_sha256"] == runs["divisor_8_steer_brake"]["core_sha256"]
    assert data["rom_sha256"] == runs["divisor_8_steer_brake"]["rom_sha256"]

for name, run in runs.items():
    assert run["route_waypoint_radius"] == 4096
    assert run["route_lookahead"] == 8192
    assert run["route_brake_speed"] == 200000
    assert run["route_lookahead_speed_divisor"] == (8 if "divisor_8" in name else 16 if "divisor_16" in name else 32)
assert runs["divisor_8_steer_brake"]["route_steer_during_brake"] is True
assert runs["divisor_16_steer_brake"]["route_steer_during_brake"] is True
assert runs["divisor_32_steer_brake"]["route_steer_during_brake"] is True
assert runs["divisor_16_no_steer_brake"]["route_steer_during_brake"] is False

best = max(runs.values(), key=lambda run: run["final_waypoint"])
report = {
    "status": "PASS_MATCHED_ROUTE_POLICY_COMPARISON_NO_DELIVERY",
    "test": "route31 source-ROM controller lookahead sweep from one serialized boot state",
    "common_rom_core_state": True,
    "common_rom_sha256": best["rom_sha256"],
    "common_core_sha256": best["core_sha256"],
    "common_state_sha256": best["state_sha256"],
    "shared_parameters": {"frames": 16000, "route_source": 31, "route_start_waypoint": 0, "route_waypoint_radius": 4096, "route_lookahead": 8192, "route_brake_speed": 200000},
    "results": runs,
    "finding": "With the same ROM/core/saved state and remaining settings held fixed, divisor16 reached waypoint21, divisor8 waypoint20, and divisor32 waypoint19. Disabling steering while braking at divisor16 also reached waypoint21, so that flag produced no improvement at this endpoint. Every run expired with completed_count 0 and fare 0; this is diagnostic controller evidence, not delivery parity or playable-port verification.",
    "scope": "Only adaptive test-driver inputs changed. The source ROM supplied vehicle movement, collisions, passenger state and clocks; no ROM RAM or game physics was modified. This is not recorded human input, and the route remains incomplete.",
}
out = REPORTS / "route31-lookahead-policy-audit.json"
out.write_text(json.dumps(report, indent=2) + "\n")
print(report["status"])
print("divisor outcomes:", [(name, run["final_waypoint"], run["fare"]) for name, run in runs.items()])
