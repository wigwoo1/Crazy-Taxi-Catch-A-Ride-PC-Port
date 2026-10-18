#!/usr/bin/env python3
"""Compare a bounded source pickup window against the native driver.

Each captured VBlank updates the native TaxiInput state once. Native physics runs
only when the source trace advances its `previous` counter, using that exact
source delta/frame_delta. This diagnoses cadence alignment; it is not full-game parity.
"""
import gzip
import hashlib
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
TRACE = REPORTS / "route31nativeinputreplaytrace.actor-trace.json.gz"
CAPTURE = REPORTS / "route31nativeinputreplaytrace.route-follow.json"
BOUNDARY_AUDIT = REPORTS / "route31-controller-sample-boundaries.json"
FIXTURE = REPORTS / "route31pickupstate516.ewram-vblank-00516.bin"
IWRAM = REPORTS / "route31pickupstate516.03000000.bin"
BINARY = ROOT / "bin/route31-source-vblank-native-probe.exe"
OUTPUT = REPORTS / "route31-source-vblank-native-pickup-differential.json"
CONTACT_AUDIT = REPORTS / "route31nativeinputreplaytrace-traffic-contact-audit.json"
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
KEY_BITS = {0: 1, 6: 5, 7: 4, 8: 0}

trace_bytes = TRACE.read_bytes()
trace_sha = hashlib.sha256(trace_bytes).hexdigest()
audit = json.loads(BOUNDARY_AUDIT.read_text(encoding="utf-8"))
capture = json.loads(CAPTURE.read_text(encoding="utf-8"))
assert capture["rom_sha256"] == ROM_SHA
assert capture["route_source"] == 31 and capture["frames"] == 12000
assert audit["trace_sha256"] == trace_sha
assert FIXTURE.stat().st_size == 0x40000 and IWRAM.stat().st_size == 0x8000
rows = json.loads(gzip.decompress(trace_bytes))
assert len(rows) == 12000 and all(row["tick"] == i for i, row in enumerate(rows))
assert rows[516]["position"] == [34927, 19200, -27676]
assert rows[516]["heading"] == 13848576 and rows[516]["speed"] == 0

start, end = 516, 11999
stdin_rows = []
expected = []
last_previous = rows[start]["previous"]
for tick in range(start + 1, end + 1):
    row = rows[tick]
    active_bits = 0
    for key in row["input_keys"]:
        assert key in KEY_BITS, (tick, key)
        active_bits |= 1 << KEY_BITS[key]
    raw = 0x03FF ^ active_bits
    boundary = int(row["previous"] != last_previous)
    stdin_rows.append(f"{raw} {row['delta']} {row['frame_delta']} {boundary} {tick}\n")
    if boundary:
        expected.append({"tick": tick, "position": row["position"], "heading": row["heading"],
                         "speed": row["speed"], "delta": row["delta"],
                         "frame_delta": row["frame_delta"], "input_keys": row["input_keys"]})
    last_previous = row["previous"]

proc = subprocess.run([str(BINARY), str(FIXTURE), str(IWRAM)], input="".join(stdin_rows),
                      text=True, capture_output=True, cwd=ROOT.parent.parent, timeout=90, check=True)
actual = []
for line in proc.stdout.splitlines():
    tick, result, x, y, z, heading, speed = map(int, line.split())
    actual.append({"tick": tick, "result": result, "position": [x, y, z],
                   "heading": heading, "speed": speed})
assert len(actual) == len(expected), (len(actual), len(expected), proc.stdout[-1000:])
contact_audit = json.loads(CONTACT_AUDIT.read_text(encoding="utf-8"))
assert contact_audit["actor_trace_sha256"] == trace_sha
contact_by_tick = {event["tick"]: event for event in contact_audit["traffic_contact_events"]}
comparisons = []
for a, e in zip(actual, expected):
    assert a["tick"] == e["tick"]
    comparisons.append({"tick": a["tick"], "native_result": a["result"],
                        "position_match": a["position"] == e["position"],
                        "heading_match": a["heading"] == e["heading"],
                        "speed_match": a["speed"] == e["speed"],
                        "native_position": a["position"], "source_position": e["position"],
                        "native_heading": a["heading"], "source_heading": e["heading"],
                        "native_speed": a["speed"], "source_speed": e["speed"],
                        "source_delta": e["delta"], "source_frame_delta": e["frame_delta"],
                        "sampled_keys": e["input_keys"]})
first_mismatch = next((x for x in comparisons if not (x["position_match"] and x["heading_match"] and x["speed_match"] and x["native_result"] == 0)), None)
first_contact = contact_by_tick.get(first_mismatch["tick"]) if first_mismatch else None
match = first_mismatch is None
matching_prefix = 0
for item in comparisons:
    if not (item["position_match"] and item["heading_match"] and item["speed_match"] and item["native_result"] == 0):
        break
    matching_prefix += 1
status = ("PASS_BOUNDED_VEHICLE_CADENCE_PARITY" if match else
          "MISMATCH_AT_SOURCE_TRAFFIC_CONTACT" if first_contact else "MISMATCH_BOUNDED_VEHICLE_CADENCE")
report = {"status": status,
          "rom_sha256": ROM_SHA, "trace_sha256": trace_sha,
          "source_trace": TRACE.name, "fixture": FIXTURE.name,
          "window_ticks": [start + 1, end], "vblank_input_updates": end - start,
          "source_physics_commits": len(expected), "exact_vehicle_checkpoints": sum(x["position_match"] and x["heading_match"] and x["speed_match"] for x in comparisons),
          "matching_prefix_checkpoints": matching_prefix,
          "first_mismatch": first_mismatch,
          "source_traffic_contact_at_first_mismatch": first_contact,
          "comparisons": comparisons,
          "scope": "Diagnostic native driving routine only: one native TaxiInput update per captured VBlank and one taxi_drive_world_step at each source previous-counter boundary using source delta/frame_delta. It compares vehicle XYZ/heading/speed in a bounded pickup window. Traffic actor pool, passenger/route/fare gameplay integration, later trip behavior, and live host clock/input cadence are not tested."}
OUTPUT.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({k: report[k] for k in ("status", "window_ticks", "vblank_input_updates", "source_physics_commits", "exact_vehicle_checkpoints", "matching_prefix_checkpoints", "first_mismatch", "source_traffic_contact_at_first_mismatch", "scope")}, indent=2))





