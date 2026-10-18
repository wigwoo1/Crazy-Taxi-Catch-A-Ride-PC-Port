"""Compare traced native delivery query points against the original ROM routine.

Samples actual XYZs from a native input-only delivery trace. This checks the
source query routine at those points; it is not a full route/timing differential.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, "work/python-packages")
root = Path("outputs/crazy-taxi-native")
parser = argparse.ArgumentParser()
parser.add_argument("query_csv", type=Path)
parser.add_argument("output_json", type=Path)
args = parser.parse_args()

# Reuse the already-qualified Thumb interpreter, explicit division/containment
# contracts, and GroundProbe setup used by the existing ROM comparison.
exec(open(root / "tools/compare_bound_ground_contact_rom.py", encoding="utf-8").read().split("contactcfg=")[0])

cfg = json.loads((root / "reports/challenge-bound-obstacle-cfg.json").read_text())
allowed_thumb = {int(item["address"], 16): bytes.fromhex(item["bytes"]) for item in cfg["instructions"]}
source_query = 0x087556CC
polygon_table = int.from_bytes(r[source_query - 0x8000000 + 0x28:source_query - 0x8000000 + 0x2C], "little")
with args.query_csv.open(newline="", encoding="utf-8-sig") as stream:
    rows = list(csv.DictReader(stream))
if not rows:
    raise SystemExit("The native delivery query trace is empty.")

# Keep first/middle/last samples in each waypoint, and force every recorded
# obstacle-contact point into the check. Preserve query order for replay.
by_waypoint: dict[int, list[int]] = defaultdict(list)
for index, row in enumerate(rows):
    by_waypoint[int(row["waypoint"])].append(index)
selected_indices: set[int] = set()
for indices in by_waypoint.values():
    selected_indices.update((indices[0], indices[len(indices) // 2], indices[-1]))
selected_indices.update(i for i, row in enumerate(rows) if int(row["kind"]) == 2)
selected = [rows[index] for index in sorted(selected_indices)]
cases = [tuple(int(row[key]) for key in ("query_x", "query_y", "query_z")) for row in selected]

native_process = subprocess.run(
    [str(root / "bin/scene-obstacle-rom-compare.exe"), str(root / "assets/city1.ctw")],
    input="".join(" ".join(map(str, case)) + "\n" for case in cases),
    capture_output=True,
    text=True,
    check=True,
)
native = [list(map(int, line.split())) for line in native_process.stdout.splitlines()]
if len(native) != len(cases):
    raise SystemExit(f"Native comparison returned {len(native)} rows for {len(cases)} queries.")

steps = 0
kind_counts: Counter[int] = Counter()
polygon_contacts: Counter[int] = Counter()
for capture, case, native_row in zip(selected, cases, native):
    point = GroundProbe((0, 0, 0, 0, 0, 1), 0)
    query = point.base
    output = query + 0x100
    for offset, value in zip((0, 4, 8), case):
        point.write(query + offset, value)
    point.arm.mem[0x3006810] = source_query
    point.arm.mem[0x3006940] = 0x80029FC
    point.reg[:2] = [query, output]
    point.reg[14] = 0x10020001
    point.run(entry=0x8029E84, stop=0x10020000)
    height = signed(point.reg[0])
    polygon_pointer = point.read(output)
    kind = 2 if height == 2147483647 else 0 if height == -2147483648 else 1
    original = [kind, height, (polygon_pointer - polygon_table) // 24 if kind == 2 else 65535]
    expected_capture = [int(capture[key]) for key in ("kind", "height", "polygon")]
    if original != expected_capture or native_row[:3] != expected_capture:
        raise SystemExit({
            "counter": capture["source_counter"], "waypoint": capture["waypoint"],
            "query_xyz": case, "captured": expected_capture,
            "original_rom": original, "native_result": native_row[:3],
        })
    kind_counts[kind] += 1
    if kind == 2:
        polygon_contacts[original[2]] += 1
    steps += point.steps

report = {
    "status": "PASS_TRACED_DELIVERY_QUERY_SAMPLES_MATCH_ORIGINAL_ROM",
    "captured_query_rows": len(rows),
    "sampled_queries": len(selected),
    "sampled_waypoints": sorted(by_waypoint),
    "sampled_kind_counts": {str(key): value for key, value in sorted(kind_counts.items())},
    "sampled_obstacle_polygons": {str(key): value for key, value in sorted(polygon_contacts.items())},
    "rom_thumb_instructions": steps,
    "samples": [
        {
            "counter": int(row["source_counter"]),
            "waypoint": int(row["waypoint"]),
            "xyz": [int(row[key]) for key in ("query_x", "query_y", "query_z")],
            "kind": int(row["kind"]),
            "height": int(row["height"]),
            "polygon": int(row["polygon"]),
        }
        for row in selected
    ],
    "scope": (
        "The original ROM obstacle/ground-query routine and native city1 query implementation agree at a stratified "
        "sample of actual XYZ movement-query points from one experimental-minimum-five-tick input-only route, "
        "including all observed kind-2 contacts. This does not compare the original whole vehicle trajectory, "
        "query ordering during original gameplay, source cadence/input, or fare/results behavior."
    ),
    "sha256": {
        str(path): hashlib.sha256(path.read_bytes()).hexdigest()
        for path in (args.query_csv, root / "assets/city1.ctw", root / "bin/scene-obstacle-rom-compare.exe", root / "tools/compare_traced_delivery_queries_rom.py")
    },
}
args.output_json.parent.mkdir(parents=True, exist_ok=True)
args.output_json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({key: value for key, value in report.items() if key not in ("samples", "sha256")}, indent=2))
