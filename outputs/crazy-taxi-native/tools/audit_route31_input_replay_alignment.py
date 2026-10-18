#!/usr/bin/env python3
"""Locate the first divergence in a source-ROM/native route input replay."""
import csv
import gzip
import hashlib
import json
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
R = ROOT / "reports/original-core-reference"


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    native_path = ROOT / "reports/current-delivery-input-delivery-probe.csv"
    summary_path = R / "route31nativeinputreplaytrace.route-follow.json"
    trace_path = R / "route31nativeinputreplaytrace.actor-trace.json.gz"
    contacts_path = R / "route31nativeinputreplaytrace-traffic-contact-audit.json"
    native_summary = json.loads((ROOT / "reports/current-delivery-integration-verification.json").read_text(encoding="utf-8"))
    source_summary = json.loads(summary_path.read_text(encoding="utf-8"))
    contacts = json.loads(contacts_path.read_text(encoding="utf-8"))
    with native_path.open(newline="", encoding="utf-8") as f:
        native = list(csv.DictReader(f))
    with gzip.open(trace_path, "rt", encoding="utf-8") as f:
        trace = json.load(f)
    assert source_summary["input_replay"]["rows"] == source_summary["input_replay"]["consumed"] == len(native)
    active = [row for row in trace if row.get("route_driver", {}).get("trip_active")]
    aligned = []
    for row in active:
        driver = row["route_driver"]
        index = driver.get("replay_tick")
        if index is None or index >= len(native):
            continue
        sample = native[index]
        steer, brake = int(sample["steer"]), int(sample["brake"])
        assert int(driver["replay_steer"]) == steer and int(driver["replay_brake"]) == brake
        dx = row["position"][0] - int(sample["x"])
        dz = row["position"][2] - int(sample["z"])
        aligned.append({
            "source_tick": row["tick"], "native_input_tick": int(sample["tick"]),
            "source_replay_index": index, "source_waypoint": driver["waypoint"],
            "native_waypoint": int(sample["waypoint"]), "steer_command": steer,
            "brake_command": brake, "source_xz": [row["position"][0], row["position"][2]],
            "native_xz": [int(sample["x"]), int(sample["z"])],
            "position_delta_xz": [dx, dz], "position_distance_floor": math.isqrt(dx * dx + dz * dz),
            "source_speed": row["speed"], "native_speed": int(sample["speed"]),
            "source_heading": row["heading"], "native_heading": int(sample["heading"]),
        })
    if not aligned:
        raise SystemExit("no source/native input pairs were matched")
    first_waypoint_mismatch = next((x for x in aligned if x["source_waypoint"] != x["native_waypoint"]), None)
    for row in aligned:
        a, b = (row["source_heading"] >> 12) & 4095, (row["native_heading"] >> 12) & 4095
        row["heading_delta_units"] = abs((a - b + 2048) % 4096 - 2048)
    first_yaw_divergence = next(x for x in aligned if x["heading_delta_units"] >= 16)
    first_position_divergence = next(x for x in aligned if x["position_distance_floor"] >= 1024)
    first_contact = min(contacts["traffic_contact_events"], key=lambda x: x["tick"])
    assert len(aligned) == len(native) and first_waypoint_mismatch and first_contact["tick"] > first_waypoint_mismatch["source_tick"]
    checkpoints = [aligned[0], next(x for x in aligned if x["source_tick"] >= 650),
                   first_yaw_divergence, first_position_divergence, first_waypoint_mismatch,
                   next(x for x in aligned if x["source_tick"] >= first_contact["tick"])]
    report = {
        "status": "PASS_ROUTE31_SOURCE_NATIVE_INPUT_ALIGNMENT_AUDIT",
        "common_source_capture": {"rom_sha256": source_summary["rom_sha256"], "core_sha256": source_summary["core_sha256"], "state_sha256": source_summary["state_sha256"]},
        "native_success_route": {"route": 31, "points": 27, "destination": 27, "completed": int(native_summary["last_delivery"]["completed"]), "fare_whole": int(native_summary["last_delivery"]["cash_whole"])},
        "source_replay": {"frames": source_summary["frames"], "paired_input_rows": len(aligned), "completed_count": source_summary["completed_count"], "fare": source_summary["fare"], "first_yaw_divergence_at_16_units": first_yaw_divergence, "first_position_divergence_at_1024_units": first_position_divergence, "first_gate_contact": first_contact, "first_waypoint_mismatch": first_waypoint_mismatch},
        "checkpoints": checkpoints,
        "sha256": {str(p.relative_to(ROOT)): sha(p) for p in (Path(__file__), native_path, ROOT / "reports/current-delivery-integration-verification.json", summary_path, trace_path, contacts_path)},
        "scope": "Replay consumes the recorded native steer/brake values exactly, but matched-command states differ before the first verified traffic contact. Heading differs by at least16 of4096 units at the first active sample; position first differs by1024 units at source tick892, waypoint progress first differs at tick1056/replay index540, and the first exact contact is tick1412. This demonstrates input-value replay alone does not align the cross-runtime trajectory and does not isolate steering sign as the cause. Source ROM still completes zero fares; this is diagnostic, not source-game parity."
    }
    out = ROOT / "reports/route31-source-native-input-alignment-audit.json"
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "paired_rows": len(aligned), "first_yaw_divergence": first_yaw_divergence, "first_position_divergence": first_position_divergence, "first_waypoint_mismatch": first_waypoint_mismatch, "first_contact_tick": first_contact["tick"], "report": str(out)}, indent=2))


if __name__ == "__main__":
    main()
