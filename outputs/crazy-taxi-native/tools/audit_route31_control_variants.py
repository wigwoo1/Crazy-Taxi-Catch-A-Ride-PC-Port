#!/usr/bin/env python3
"""Compare source-ROM route-control probes from the identical saved state."""
import gzip
import hashlib
import json
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
TARGET = (-257694, 274082)


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def read_json(path):
    return json.loads(path.read_text())


def read_trace(label):
    raw = REPORTS / f"{label}.actor-trace.json"
    zipped = REPORTS / f"{label}.actor-trace.json.gz"
    if raw.exists():
        return json.loads(raw.read_text()), raw
    with gzip.open(zipped, "rt", encoding="utf-8") as f:
        return json.load(f), zipped


def main():
    baseline_timeout = read_json(REPORTS / "route31-source-identity-timeout-audit.json")
    variants = {
        "route31radius4k": "source path, no shoulder input",
        "route31boost": "one shoulder pulse (legacy experiment label)",
        "route31cycle80": "repeated shoulder pulses every 80 ticks",
        "route31direct": "direct steering at the extracted destination coordinates",
        "route31shift2": "two shoulder shift pulses 80 ticks apart",
        "route31nativeinputreplay": "native no-traffic route steering/brake replay, normal direction",
        "route31trafficinput": "native traffic-enabled route steering/brake replay, normal direction",
        "route31trafficinvert": "same traffic-enabled schedule with steering inverted",
    }
    results = {}
    hashes = set()
    for label, description in variants.items():
        summary = read_json(REPORTS / f"{label}.route-follow.json")
        trace, trace_path = read_trace(label)
        hashes.add((summary["rom_sha256"], summary["core_sha256"], summary["state_sha256"]))
        timeout = next((row for row in trace if row.get("remaining_clock") == 0 and row.get("route_driver", {}).get("trip_active")), None)
        if timeout is None:
            timeout = next((row for row in reversed(trace) if row.get("remaining_clock") == 0), trace[-1])
        driver = timeout.get("route_driver", {})
        position = driver.get("position", [0, 0, 0])
        pulses = [row for row in trace if row.get("route_driver", {}).get("shoulder_pulse") or row.get("route_driver", {}).get("boost_pulse")]
        distance = math.floor(math.hypot(position[0] - TARGET[0], position[2] - TARGET[1]))
        results[label] = {
            "description": description,
            "status": summary["status"],
            "timeout_tick": timeout["tick"],
            "waypoint": driver.get("waypoint"),
            "position_xz": [position[0], position[2]],
            "floor_distance_to_destination": distance,
            "shoulder_pulse_ticks": [row["tick"] for row in pulses],
            "completed_count": summary["completed_count"],
            "fare": summary["fare"],
            "input_replay": summary.get("input_replay"),
            "actor_trace_sha256": summary["actor_trace_sha256"],
            "actor_trace_file_sha256": sha(trace_path),
            "route_summary_sha256": sha(REPORTS / f"{label}.route-follow.json"),
        }
    report = {
        "status": "PASS_CONTROL_VARIANTS_AUDITED",
        "common_rom_core_state_hashes": len(hashes) == 1,
        "shared_source_time_out": baseline_timeout["first_active_sample_at_clock_zero"],
        "destination_target_xz": list(TARGET),
        "variants": results,
        "scope": "Same supplied ROM, mGBA core and saved state across scripted input probes. All remain incomplete with zero fares. The one-press shoulder result is not labeled a boost; repeated taps switch the source vehicle direction state. Direct-destination steering is a diagnostic comparison, not a recovered source route planner or proof of human-input parity.",
    }
    if not report["common_rom_core_state_hashes"]:
        raise SystemExit("control variants do not share ROM/core/state inputs")
    output = REPORTS / "route31-control-variant-audit.json"
    output.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
