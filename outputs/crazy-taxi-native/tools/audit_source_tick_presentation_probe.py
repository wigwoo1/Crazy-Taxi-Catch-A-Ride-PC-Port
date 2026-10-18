"""Audit bounded timestamped native-render cadence experiments.

This is diagnostic accounting only: it does not prove source-ROM parity,
monitor scanout, or a release-quality 60 Hz presentation.
"""
from __future__ import annotations
import argparse
import csv
import json
from pathlib import Path


def read_run(root: Path, stem: str) -> dict:
    pose_path = root / f"{stem}-presentation.txt"
    perf_path = root / f"{stem}-performance.csv"
    poses = []
    for number, line in enumerate(pose_path.read_text(encoding="utf-8").splitlines(), 1):
        parts = line.split()
        if len(parts) != 14:
            raise ValueError(f"{pose_path}:{number}: expected 14 fields, got {len(parts)}")
        values = [int(parts[i]) for i in (0, 1, 2, 3)]
        alpha = float(parts[4])
        held = int(parts[5])
        taxi = [int(x) for x in parts[6:9]]
        camera = [int(x) for x in parts[9:12]]
        mode, updates = int(parts[12]), int(parts[13])
        if not 0.0 <= alpha <= 1.0 or held not in (0, 1):
            raise ValueError(f"{pose_path}:{number}: invalid alpha/held value")
        poses.append({"render_ns": values[0], "target_ns": values[1], "before_ns": values[2],
                      "after_ns": values[3], "alpha": alpha, "held": held,
                      "taxi": taxi, "camera": camera, "camera_mode": mode, "updates": updates})
    with perf_path.open(encoding="utf-8", newline="") as stream:
        perf = list(csv.DictReader(stream))
    intervals = [float(row["interval_ms"]) for row in perf if float(row["interval_ms"]) > 0]
    if not poses or not perf or not intervals:
        raise ValueError(f"{stem}: missing pose or performance samples")
    interior = [row for row in poses if row["before_ns"] != row["after_ns"] and 0 < row["alpha"] < 1]
    return {
        "presentation_trace": pose_path.name,
        "performance_trace": perf_path.name,
        "pose_samples": len(poses),
        "updates_min_max": [min(row["updates"] for row in poses), max(row["updates"] for row in poses)],
        "interpolated_between_distinct_poses": len(interior),
        "held_samples": sum(row["held"] for row in poses),
        "unique_taxi_positions": len({tuple(row["taxi"]) for row in poses}),
        "taxi_position_min": [min(row["taxi"][i] for row in poses) for i in range(3)],
        "taxi_position_max": [max(row["taxi"][i] for row in poses) for i in range(3)],
        "completed_swaps": len(perf),
        "measured_intervals": len(intervals),
        "mean_interval_ms": sum(intervals) / len(intervals),
        "p95_interval_ms": sorted(intervals)[int((len(intervals) - 1) * 0.95)],
        "max_interval_ms": max(intervals),
        "intervals_over_16_667ms": sum(value > 16.667 for value in intervals),
        "scope": "Native 4K render/history diagnostic. No source-ROM cadence equivalence, display scanout, or sustained 60 Hz claim."
    }


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("stems", nargs="+", help="Trace filename stems without -presentation.txt")
    args = parser.parse_args()
    report = {"status": "PASS_TRACE_ACCOUNTING", "runs": [read_run(args.root, stem) for stem in args.stems]}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"PASS: audited {len(report['runs'])} timestamped presentation runs -> {args.output}")


if __name__ == "__main__":
    main()
