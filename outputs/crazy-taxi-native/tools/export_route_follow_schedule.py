"""Export variable-tick, per-update inputs from the scripted original-ROM capture."""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from collections import Counter
from pathlib import Path

root = Path("outputs/crazy-taxi-native")
parser = argparse.ArgumentParser()
parser.add_argument("actor_trace", type=Path)
parser.add_argument("baseline_ewram", type=Path)
parser.add_argument("schedule_txt", type=Path)
parser.add_argument("report_json", type=Path)
parser.add_argument("--pose-heading-only", action="store_true", help="write V4 rows that verify position and heading while excluding the known shared-speed owner discrepancy")
args = parser.parse_args()

rows = json.loads(args.actor_trace.read_text(encoding="utf-8"))
baseline = args.baseline_ewram.read_bytes()
if len(baseline) != 0x40000:
    raise SystemExit("Unexpected source baseline EWRAM size.")
active_rows = [index for index, row in enumerate(rows) if row.get("route_driver", {}).get("trip_active")]
if not active_rows:
    raise SystemExit("The source capture never boarded a passenger.")
# Stop at the timeout boundary. The game returns to a different cadence after
# the passenger timer expires; that ending state is outside this route replay.
rows = rows[:active_rows[-1] + 1]
baseline_counter = struct.unpack_from("<I", baseline, 0x3520)[0]
baseline_previous = struct.unpack_from("<I", baseline, 0x3524)[0]
baseline_delta = struct.unpack_from("<i", baseline, 0x3B00)[0]
steps: list[tuple[int, ...]] = []
def owner_fields(row: dict) -> tuple[int, ...]:
    owners = row.get("driver_owners")
    if not isinstance(owners, dict):
        raise SystemExit("Source trace has no per-update driving owner fields.")
    values = tuple(int(owners[name]) for name in ("flag_1697", "flag_153c", "freeze_2620", "freeze_177b"))
    if any(value not in (0, 1) for value in values):
        raise SystemExit(f"Invalid source driver owner flags: {values!r}.")
    return values
intervals: Counter[int] = Counter()
first = rows[0]
first_elapsed = (first["previous"] - baseline_previous) & 0xFFFFFFFF
if first["frame_delta"] != max(1, min(60, first_elapsed)):
    raise SystemExit("Initial captured update does not match the source frame clamp.")
first_stamp = (first["counter"] - baseline_counter) & 0xFFFFFFFF
steps.append((first["raw_input_history"], baseline_delta, first["delta"], first["frame_delta"], *first["position"], first["heading"], first["speed"], first_stamp, *owner_fields(first)))
intervals[first_stamp] += 1
previous_stamp = first_stamp
for index, (a, b) in enumerate(zip(rows, rows[1:])):
    if a["previous"] == b["previous"]:
        continue
    elapsed = (b["previous"] - a["previous"]) & 0xFFFFFFFF
    if elapsed >= 0x80000000:
        elapsed -= 0x100000000
    expected_frame = max(1, min(60, elapsed))
    if b["frame_delta"] != expected_frame:
        raise SystemExit(f"Frame clamp mismatch at trace row {index + 1}: {b['frame_delta']} != {expected_frame}.")
    physics_delta = b["delta"]
    if not 1 <= physics_delta <= 60 or not 1 <= b["frame_delta"] <= 60:
        raise SystemExit(f"Unsupported source time pair at trace row {index + 1}: {physics_delta}/{b['frame_delta']}.")
    raw = b["raw_input_history"]
    if not 0 <= raw <= 1023:
        raise SystemExit(f"Invalid active-low GBA key sample at trace row {index + 1}: {raw}.")
    stamp = (b["counter"] - baseline_counter) & 0xFFFFFFFF
    if stamp <= previous_stamp or stamp - previous_stamp > 60:
        raise SystemExit(f"Invalid VBlank event stamp at trace row {index + 1}: {stamp}.")
    steps.append((raw, a["frame_delta"], physics_delta, b["frame_delta"], *b["position"], b["heading"], b["speed"], stamp, *owner_fields(b)))
    intervals[stamp - previous_stamp] += 1
    previous_stamp = stamp

if not steps:
    raise SystemExit("No original update boundaries found in the source actor trace.")
args.schedule_txt.parent.mkdir(parents=True, exist_ok=True)
schedule_rows = [step + (1,) if args.pose_heading_only else step for step in steps]
magic = "SOURCE_DRIVING_REFERENCE_V4" if args.pose_heading_only else "SOURCE_DRIVING_REFERENCE_V3"
args.schedule_txt.write_text(
    f"{magic} {len(schedule_rows)}\n" + "".join(" ".join(map(str, step)) + "\n" for step in schedule_rows),
    encoding="ascii",
)
report = {
    "status": "PASS_SOURCE_ROUTE31_SCHEDULE_EXPORTED_POSE_HEADING_ONLY" if args.pose_heading_only else "PASS_SOURCE_ROUTE31_SCHEDULE_EXPORTED",
    "updates": len(steps),
    "source_vblank_intervals": dict(sorted(intervals.items())),
    "time_delta_histogram": dict(sorted(Counter(step[2] for step in steps).items())),
    "frame_delta_histogram": dict(sorted(Counter(step[3] for step in steps).items())),
    "physics_frame_delta_mismatches": sum(step[2] != step[3] for step in steps),
    "first_update": list(steps[0]),
    "last_update": list(steps[-1]),
    "capture_scope": (
        "Each exported update uses raw active-low keys, per-update flag_1697/flag_153c/freeze_2620/freeze_177b, recorded prior/frame/physics deltas, captured expected "
        "vehicle pose/heading/speed, and VBlank stamp from the original-ROM adaptive test-driver run. "
        "The run timed out at waypoint19 without a settled fare; it is a native routine reference schedule, "
        "not a successful delivery or a human input recording."
        + (" This V4 schedule checks only position and heading; speed is deliberately excluded because a separate within-dispatch owner changes the shared speed field." if args.pose_heading_only else "")
    ),
    "sha256": {
        str(path): hashlib.sha256(path.read_bytes()).hexdigest()
        for path in (args.actor_trace, args.baseline_ewram, args.schedule_txt)
    },
}
args.report_json.parent.mkdir(parents=True, exist_ok=True)
args.report_json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({key: value for key, value in report.items() if key not in ("sha256",)}, indent=2))
