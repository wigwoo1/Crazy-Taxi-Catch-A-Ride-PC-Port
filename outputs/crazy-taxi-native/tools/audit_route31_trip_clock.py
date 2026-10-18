#!/usr/bin/env python3
"""Check captured route-31 trip-clock transitions against recovered C rules."""
import argparse
import hashlib
import json
from pathlib import Path


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("trace", type=Path)
    ap.add_argument("--output", type=Path, required=True)
    args = ap.parse_args()
    trace = json.loads(args.trace.read_text())
    active = [row for row in trace if row["trip_clock_state"]["trip_active"]]
    checked = 0
    mismatches = []
    for before, after in zip(active, active[1:]):
        a = before["trip_clock_state"]
        b = after["trip_clock_state"]
        if (a["trip_time"], a["trip_allowance"], before["remaining_clock"]) == (
            b["trip_time"], b["trip_allowance"], after["remaining_clock"]
        ):
            continue
        delta = after["delta"]
        # Source order: main-clock decrement, trip-clock decrement, then allowance transfer.
        remaining = max(0, before["remaining_clock"] - delta)
        trip_time = a["trip_time"] - delta
        allowance = a["trip_allowance"]
        if allowance:
            amount = delta << 4
            remaining += amount
            allowance -= amount
            if allowance <= 0:
                remaining += allowance
                allowance = 0
        expected = (trip_time, allowance, remaining)
        observed = (b["trip_time"], b["trip_allowance"], after["remaining_clock"])
        checked += 1
        if expected != observed:
            mismatches.append({
                "from_tick": before["tick"], "to_tick": after["tick"],
                "delta": delta, "expected": expected, "observed": observed,
            })
    report = {
        "pass": not mismatches and checked > 0,
        "trace": str(args.trace),
        "trace_sha256": sha256(args.trace),
        "active_snapshots": len(active),
        "advancing_transitions_checked": checked,
        "mismatch_count": len(mismatches),
        "mismatches_first_20": mismatches[:20],
        "scope": "Observed main clock, trip countdown and allowance-transfer arithmetic for this route trace. Does not establish trip-start input parity, trip expiry, successful delivery, or full game parity.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps(report, indent=2))
    if not report["pass"]:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
