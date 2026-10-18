#!/usr/bin/env python3
"""Join an earned native route31 fare to the source passenger and destination."""
import csv
import hashlib
import json
import math
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports"
REF = REPORTS / "original-core-reference"


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    source = json.loads((REF / "route31-source-identity-timeout-audit.json").read_text(encoding="utf-8"))
    native = json.loads((REPORTS / "current-traffic-delivery-integration-verification.json").read_text(encoding="utf-8"))
    route_path = REPORTS / "current-traffic-delivery-delivery-source-route.csv"
    probe_path = REPORTS / "current-traffic-delivery-input-delivery-probe.csv"
    with route_path.open(newline="", encoding="utf-8") as f:
        route_rows = list(csv.DictReader(f))
    with probe_path.open(newline="", encoding="utf-8") as f:
        rows = list(csv.DictReader(f))
    if source["source_route_index"] != 31 or source["passenger_index"] != 98 or source["destination_index"] != 27:
        raise SystemExit("source identity audit is not the expected passenger/route/destination")
    if len(route_rows) != source["waypoint_count"]:
        raise SystemExit("native source-route projection point count differs from ROM-derived identity")
    route_end = [int(route_rows[-1]["x"]), int(route_rows[-1]["z"])]
    if route_end != source["route_end_xz"]:
        raise SystemExit("native route projection endpoint differs from ROM-derived endpoint")
    if not rows:
        raise SystemExit("native delivery trace is empty")
    accepted = [i for i, row in enumerate(rows) if int(row["accepted_material"]) == 1]
    final = rows[-1]
    expected = native["last_delivery"]
    for key in ("x", "y", "z", "speed", "material", "accepted_material", "trip_remaining", "completed", "cash_whole", "waypoint"):
        if int(final[key]) != int(expected[key]):
            raise SystemExit(f"native CSV/report terminal mismatch in {key}")
    if not accepted or final["accepted_material"] != "1" or final["completed"] != "1":
        raise SystemExit("native source material gate or completed fare is absent")
    material = int(final["material"])
    slots = source["destination_accepted_material_slots"]
    if material not in slots:
        raise SystemExit("native terminal material is not accepted by source destination data")
    target = source["destination_target_xz"]
    pos = [int(final["x"]), int(final["z"])]
    target_distance = math.isqrt((pos[0] - target[0]) ** 2 + (pos[1] - target[1]) ** 2)
    route_endpoint_distance = math.isqrt((route_end[0] - target[0]) ** 2 + (route_end[1] - target[1]) ** 2)
    assert int(final["speed"]) == 0 and int(final["trip_remaining"]) == 0
    assert int(final["completed"]) == 1 and int(final["cash_whole"]) > 0
    report = {
        "status": "PASS_NATIVE_ROUTE31_FARE_JOINED_TO_SOURCE_IDENTITY",
        "source_identity": {k: source[k] for k in ("source_route_index", "source_route_name", "passenger_index", "passenger_kind", "destination_index", "destination_target_xz", "destination_accepted_material_slots", "waypoint_count")},
        "source_route_endpoint_xz": route_end,
        "source_route_endpoint_to_destination_floor": route_endpoint_distance,
        "native_input_only_trace": {"rows": len(rows), "material_accepted_rows": len(accepted), "first_accepted_tick": int(rows[accepted[0]]["tick"]), "terminal_tick": int(final["tick"]), "terminal_position_xz": pos, "terminal_to_destination_floor": target_distance, "material": material, "speed": int(final["speed"]), "trip_remaining": int(final["trip_remaining"]), "completed": int(final["completed"]), "fare_whole": int(final["cash_whole"]), "waypoint": int(final["waypoint"])},
        "source_rom_route31_capture": {"completed_count": source["completed_count_at_capture_end"], "fare": source["fare_at_capture_end"], "timeout_waypoint": source["first_active_sample_at_clock_zero"]["waypoint_target_index"], "timeout_distance_to_destination_floor": source["last_active_sample"]["distance_to_destination_floor"]},
        "sha256": {str(p.relative_to(ROOT)): sha(p) for p in (Path(__file__), REF / "route31-source-identity-timeout-audit.json", REPORTS / "current-traffic-delivery-integration-verification.json", route_path, probe_path)},
        "scope": "Cross-runtime evidence join: the native input-only run completes this ROM-derived route/passenger/destination and earns fare1491 at accepted material70, while the scripted original-ROM route capture times out at waypoint21 with no fare. This identifies the remaining gap as source-ROM driving/input/traffic progression; it does not prove source-ROM delivery parity, general physics equivalence, human-input parity, or completed-port fidelity."
    }
    out = REF / "native-route31-source-delivery-join-audit.json"
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "native": report["native_input_only_trace"], "source_rom": report["source_rom_route31_capture"], "report": str(out)}, indent=2))


if __name__ == "__main__":
    main()
