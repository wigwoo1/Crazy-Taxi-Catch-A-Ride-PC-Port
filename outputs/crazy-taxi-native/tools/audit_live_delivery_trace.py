"""Summarize an input-only native delivery smoke's movement/contact traces.

This is an integration diagnostic, not an original-ROM gameplay differential.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
from collections import Counter, defaultdict
from pathlib import Path


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def read_csv(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8-sig") as stream:
        return list(csv.DictReader(stream))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("movement_csv", type=Path)
    parser.add_argument("query_csv", type=Path)
    parser.add_argument("run_log", type=Path)
    parser.add_argument("output_json", type=Path)
    args = parser.parse_args()

    movement = read_csv(args.movement_csv)
    queries = read_csv(args.query_csv)
    log = args.run_log.read_text(encoding="utf-8", errors="replace")
    terminal = re.search(
        r"LIVE DELIVERY TERMINAL: presentations(\d+) sourcecounter(\d+) "
        r"updates(\d+) completed(\d+) cash(\d+)\.(\d+) active(\d+) "
        r"pending(-?\d+) waypoint(\d+) clock(-?\d+) rejected(\d+) unavailable(\d+)",
        log,
    )
    route = re.search(r"LIVE DELIVERY ROUTE: source(\d+) points(\d+) destination(-?\d+)", log)
    if not movement or not queries or not terminal or not route:
        raise SystemExit("Required route terminal and nonempty movement/query traces were not found.")

    moves_by_waypoint: dict[int, list[dict[str, str]]] = defaultdict(list)
    queries_by_waypoint: Counter[int] = Counter()
    for row in movement:
        waypoint = int(row["waypoint"])
        moves_by_waypoint[waypoint].append(row)
    for row in queries:
        queries_by_waypoint[int(row["waypoint"])] += 1

    movement_query_calls = sum(int(row["queries"]) for row in movement)
    if movement_query_calls != len(queries):
        raise SystemExit(f"Movement reports {movement_query_calls} queries but trace contains {len(queries)}.")
    for waypoint, rows in moves_by_waypoint.items():
        expected = sum(int(row["queries"]) for row in rows)
        if expected != queries_by_waypoint[waypoint]:
            raise SystemExit(f"Query rows do not reconcile at waypoint {waypoint}: {expected} != {queries_by_waypoint[waypoint]}.")

    collision_rows = [row for row in movement if int(row["collided"]) != 0]
    contact_kinds = Counter(int(row["kind"]) for row in queries)
    unchanged_moves = [
        row for row in movement
        if any(int(row[key]) != 0 for key in ("dx", "dy", "dz"))
        and all(int(row[start]) == int(row[end]) for start, end in (("start_x", "result_x"), ("start_y", "result_y"), ("start_z", "result_z")))
    ]
    source, point_count, destination = map(int, route.groups())
    terminal_values = [int(value) for value in terminal.groups()]
    (presentations, source_counter, updates, completed, cash_whole, cash_fraction,
     active, pending, final_waypoint, clock, rejected, unavailable) = terminal_values
    waypoint_ids = sorted(moves_by_waypoint)
    if waypoint_ids != list(range(point_count)):
        raise SystemExit(f"Observed movement waypoint coverage is not complete: {waypoint_ids}")

    report = {
        "status": "PASS_NATIVE_DELIVERY_TRACE_RECONCILED",
        "route": {"source": source, "points": point_count, "destination": destination},
        "run": {
            "presentations": presentations,
            "source_counter": source_counter,
            "updates": updates,
            "completed": completed,
            "fare": {"whole": cash_whole, "fraction": cash_fraction},
            "active": active,
            "pending": pending,
            "waypoint": final_waypoint,
            "clock": clock,
            "audio_rejected": rejected,
            "audio_unavailable": unavailable,
        },
        "trace": {
            "movement_attempts": len(movement),
            "movement_query_calls": movement_query_calls,
            "query_trace_rows": len(queries),
            "contact_kind_counts": {str(key): value for key, value in sorted(contact_kinds.items())},
            "movement_collision_rows": len(collision_rows),
            "nonzero_displacements_with_no_position_change": len(unchanged_moves),
            "waypoint_movement_attempts": {str(key): len(moves_by_waypoint[key]) for key in waypoint_ids},
            "waypoint_query_rows": {str(key): queries_by_waypoint[key] for key in waypoint_ids},
            "collision_rows": [
                {key: int(row[key]) for key in (
                    "source_counter", "waypoint", "start_x", "start_y", "start_z",
                    "dx", "dy", "dz", "result_x", "result_y", "result_z",
                    "normal_x", "normal_y", "normal_z", "queries",
                )}
                for row in collision_rows
            ],
        },
        "scope": (
            "One input-only native route smoke using the experimental minimum-five-tick host-update policy. "
            "Trace counts reconcile per waypoint; they do not establish original-ROM collision ordering, "
            "a source-timed complete route, player-control equivalence, physical Xbox behavior, or 60Hz scanout."
        ),
        "sha256": {
            str(path): sha256(path)
            for path in (args.movement_csv, args.query_csv, args.run_log)
        },
    }
    if completed != 1 or active != 0 or pending != 0 or final_waypoint != point_count - 1:
        raise SystemExit("Trace does not end at a completed, settled delivery.")
    if len(collision_rows) != contact_kinds.get(2, 0):
        raise SystemExit("Movement collision rows and obstacle-contact classifications do not reconcile.")

    args.output_json.parent.mkdir(parents=True, exist_ok=True)
    args.output_json.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({
        "status": report["status"],
        "route": report["route"],
        "movement_attempts": len(movement),
        "query_calls": movement_query_calls,
        "contact_kind_counts": report["trace"]["contact_kind_counts"],
        "completed_fare": report["run"]["fare"],
        "report": str(args.output_json),
    }, indent=2))


if __name__ == "__main__":
    main()
