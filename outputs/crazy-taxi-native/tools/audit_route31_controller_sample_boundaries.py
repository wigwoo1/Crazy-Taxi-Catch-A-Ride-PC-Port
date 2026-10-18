#!/usr/bin/env python3
"""Measure command transitions against source update-counter boundaries."""
import gzip
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TRACE = ROOT / "reports/original-core-reference/route31nativeinputreplaytrace.actor-trace.json.gz"
CAPTURE = ROOT / "reports/original-core-reference/route31nativeinputreplaytrace.route-follow.json"
OUTPUT = ROOT / "reports/original-core-reference/route31-controller-sample-boundaries.json"

raw = TRACE.read_bytes()
rows = json.loads(gzip.decompress(raw))
capture = json.loads(CAPTURE.read_text(encoding="utf-8"))
assert capture["rom_sha256"] == "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
assert capture["route_source"] == 31 and capture["input_replay"]["rows"] == 7687
assert len(rows) == capture["frames"] == 12000
assert all(row["tick"] == i for i, row in enumerate(rows))
transitions = []
update_boundaries = []
controller_samples = []
intervals = {}
for i, row in enumerate(rows):
    previous = row["previous"]
    intervals.setdefault(previous, []).append(i)
    if i == 0:
        continue
    prior = rows[i - 1]
    boundary = previous != prior["previous"]
    if row["keyinput"] != prior["keyinput"]:
        controller_samples.append({"tick": i, "at_update_boundary": boundary,
                                   "input_keys": row["input_keys"],
                                   "keyinput_before": prior["keyinput"], "keyinput_after": row["keyinput"],
                                   "raw_input_history_before": prior["raw_input_history"],
                                   "raw_input_history_after": row["raw_input_history"],
                                   "previous_counter": previous, "frame_delta": row["frame_delta"]})
    if boundary:
        update_boundaries.append({"tick": i, "from": prior["previous"], "to": previous,
                                  "counter_increment": (previous - prior["previous"]) & 0xffffffff,
                                  "frame_delta": row["frame_delta"], "input_keys": row["input_keys"]})
    if row["input_keys"] != prior["input_keys"]:
        active = bool(row.get("trip_clock_state", {}).get("trip_active"))
        transitions.append({"tick": i, "classification": "same_vblank_as_update_boundary" if boundary else "between_update_boundaries",
                            "trip_active": active, "from": prior["input_keys"], "to": row["input_keys"],
                            "previous_counter": previous, "frame_delta": row["frame_delta"], "delta": row["delta"]})

counts = {"same_vblank_as_update_boundary": 0, "between_update_boundaries": 0}
active_counts = {"same_vblank_as_update_boundary": 0, "between_update_boundaries": 0}
for event in transitions:
    counts[event["classification"]] += 1
    if event["trip_active"]:
        active_counts[event["classification"]] += 1
assert len(controller_samples) == 298
assert all(event["at_update_boundary"] and
           event["raw_input_history_before"] != event["raw_input_history_after"]
           for event in controller_samples)
assert [event["tick"] for event in controller_samples] == [
    i for i in range(1, len(rows)) if rows[i]["raw_input_history"] != rows[i - 1]["raw_input_history"]
]
pickup_command = next(event for event in transitions if event["tick"] == 516)
pickup_sample = next(event for event in controller_samples if event["tick"] == 520)
assert pickup_command["from"] == [0] and pickup_command["to"] == [6, 8]
assert pickup_sample["input_keys"] == [6, 8] and pickup_sample["keyinput_before"] == 1021 and pickup_sample["keyinput_after"] == 990
multiple_command_intervals = []
for counter, ticks in intervals.items():
    changes = [event for event in transitions if event["previous_counter"] == counter]
    if len(changes) > 1:
        multiple_command_intervals.append({"previous_counter": counter, "first_tick": ticks[0], "last_tick": ticks[-1],
                                           "vblank_count": len(ticks), "command_transition_count": len(changes)})

report = {
    "status": "PASS_SOURCE_TRACE_BOUNDARY_AUDIT",
    "rom_sha256": capture["rom_sha256"],
    "core_sha256": capture["core_sha256"],
    "trace_file": TRACE.name,
    "trace_sha256": hashlib.sha256(raw).hexdigest(),
    "rows": len(rows),
    "source_update_boundaries": len(update_boundaries),
    "update_counter_increment_histogram": capture["source_update_interval_histogram"],
    "per_vblank_key_transitions": len(transitions),
    "transition_classification": counts,
    "trip_active_transition_classification": active_counts,
    "observed_controller_samples": len(controller_samples),
    "controller_samples_at_update_boundaries": sum(event["at_update_boundary"] for event in controller_samples),
    "controller_samples_matching_raw_input_history_changes": len(controller_samples),
    "pickup_input_timing": {"command_changed_tick": pickup_command["tick"],
                            "command": pickup_command["to"],
                            "observed_keyinput_sample_tick": pickup_sample["tick"],
                            "vblanks_until_sample": pickup_sample["tick"] - pickup_command["tick"],
                            "keyinput_before": pickup_sample["keyinput_before"],
                            "keyinput_after": pickup_sample["keyinput_after"],
                            "source_counter": pickup_sample["previous_counter"],
                            "frame_delta": pickup_sample["frame_delta"]},
    "source_update_intervals_with_multiple_key_changes": len(multiple_command_intervals),
    "sample_transitions": transitions[:24],
    "sample_multiple_change_intervals": multiple_command_intervals[:24],
    "scope": "Read-only analysis of the captured adaptive route-follow test driver in the original ROM. input_keys are commands injected per VBlank; previous/frame_delta are observed after that core tick. Same-tick counter and command transitions are correlated snapshots, not proof of internal read order. This does not record human controller input, instrument the dispatcher read, establish native controller-sampling policy, or demonstrate route/fare parity."
}
OUTPUT.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({k: report[k] for k in ("status", "rows", "source_update_boundaries", "update_counter_increment_histogram", "per_vblank_key_transitions", "transition_classification", "trip_active_transition_classification", "observed_controller_samples", "controller_samples_at_update_boundaries", "pickup_input_timing", "source_update_intervals_with_multiple_key_changes", "trace_sha256")}, indent=2))




