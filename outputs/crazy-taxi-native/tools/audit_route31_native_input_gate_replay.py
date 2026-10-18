#!/usr/bin/env python3
"""Compare open-loop native inputs with source-gate steering on one ROM state."""
import gzip
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
R = ROOT / "reports/original-core-reference"


def load(label):
    summary_path = R / f"{label}.route-follow.json"
    contact_path = R / f"{label}-traffic-contact-audit.json"
    trace_path = R / f"{label}.actor-trace.json.gz"
    summary = json.loads(summary_path.read_text(encoding="utf-8"))
    contacts = json.loads(contact_path.read_text(encoding="utf-8"))
    with gzip.open(trace_path, "rt", encoding="utf-8") as f:
        trace = json.load(f)
    gate_events = max((row.get("route_driver", {}).get("traffic_gate_events", 0) for row in trace), default=0)
    return summary, contacts, gate_events, summary_path, contact_path, trace_path


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    baseline = load("route31nativeinputreplaytrace")
    candidate = load("route31nativeinputgateavoid")
    b, bc, be, bp, bcp, bt = baseline
    c, cc, ce, cp, ccp, ct = candidate
    assert (b["rom_sha256"], b["core_sha256"], b["state_sha256"]) == (c["rom_sha256"], c["core_sha256"], c["state_sha256"])
    assert b["input_replay"]["sha256"] == c["input_replay"]["sha256"]
    assert b["route_avoid_collision_gate"] is False and not b.get("route_gate_avoid_on_replay", False)
    assert c["route_avoid_collision_gate"] is True and c["route_gate_avoid_on_replay"] is True
    assert c["route_gate_margin"] == 3.0 and c["route_gate_hold"] == 18
    assert bc["path_advance_match_count"] == bc["collision_gate_match_count_at_preimpact_speed"] == 4
    assert cc["path_advance_match_count"] == cc["collision_gate_match_count_at_preimpact_speed"] == 1
    assert b["completed_count"] == c["completed_count"] == 0 and b["fare"] == c["fare"] == [0, 0]
    assert c["final_waypoint"] < b["final_waypoint"] and ce == 6 and be == 0
    report = {
        "status": "PASS_NATIVE_INPUT_GATE_REPLAY_SWEEP_REJECTS_CANDIDATE",
        "common_rom_core_state": {"rom_sha256": b["rom_sha256"], "core_sha256": b["core_sha256"], "state_sha256": b["state_sha256"]},
        "common_native_input_schedule_sha256": b["input_replay"]["sha256"],
        "baseline_open_loop": {"waypoint": b["final_waypoint"], "contacts": bc["path_advance_match_count"], "gate_avoid_triggers": be, "completed_count": b["completed_count"], "fare": b["fare"]},
        "gate_override_margin3_hold18": {"waypoint": c["final_waypoint"], "contacts": cc["path_advance_match_count"], "gate_avoid_triggers": ce, "completed_count": c["completed_count"], "fare": c["fare"]},
        "sha256": {str(p.relative_to(ROOT)): sha(p) for p in (Path(__file__), bp, bcp, bt, cp, ccp, ct)},
        "scope": "Same supplied ROM, mGBA core, saved state and native input schedule. Exact path/gate audits confirm four collision transitions in open loop and one in the gate override; six avoidance pulses reduce final waypoint from3 to2 and neither reaches a fare. Reject gate override for this schedule. This is a bounded driver experiment, not native source-game parity or original human input behavior."
    }
    out = R / "route31-native-input-gate-replay-audit.json"
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "baseline": report["baseline_open_loop"], "gate_override": report["gate_override_margin3_hold18"], "report": str(out)}, indent=2))


if __name__ == "__main__":
    main()
