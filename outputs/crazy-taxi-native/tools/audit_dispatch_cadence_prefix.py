#!/usr/bin/env python3
"""Audit captured dispatcher-call cadence against the untouched actor trace.

This reports relationships in the baseline-verified prefix only. It deliberately
does not extrapolate source call sites past the caller-supplied cutoff.
"""
import argparse
import json
from pathlib import Path


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("baseline_actor_trace", type=Path)
    parser.add_argument("dispatcher_entry_trace", type=Path)
    parser.add_argument("--verified-through", type=int, default=956)
    args = parser.parse_args()

    baseline = json.loads(args.baseline_actor_trace.read_text())
    capture = json.loads(args.dispatcher_entry_trace.read_text())
    rows = {row["tick"]: row for row in baseline}
    calls = capture["call_events"]
    prefix = [event for event in calls
              if event["observed_after_core_tick"] <= args.verified_through]
    if not prefix:
        raise SystemExit("No dispatcher calls fall within the verified prefix.")

    missing = [event["observed_after_core_tick"] for event in prefix
               if event["observed_after_core_tick"] not in rows]
    if missing:
        raise SystemExit(f"Call ticks missing from baseline trace: {missing[:8]}")
    mismatched = [event for event in prefix
                  if rows[event["observed_after_core_tick"]]["frame_delta"]
                  != event["frame_delta"]]
    if mismatched:
        raise SystemExit(f"Captured input delta differs from baseline at "
                         f"{[e['observed_after_core_tick'] for e in mismatched[:8]]}")

    gaps = []
    for previous, current in zip(prefix, prefix[1:]):
        gap = (current["observed_after_core_tick"]
               - previous["observed_after_core_tick"])
        gaps.append({"from_tick": previous["observed_after_core_tick"],
                     "to_tick": current["observed_after_core_tick"],
                     "gap": gap,
                     "previous_delta": previous["frame_delta"],
                     "next_delta": current["frame_delta"],
                     "matches_previous": gap == previous["frame_delta"],
                     "matches_next": gap == current["frame_delta"]})

    exceptions = [gap for gap in gaps if not gap["matches_next"]]
    summary = {
        "scope": "observed dispatcher calls inside verified source prefix only",
        "verified_through_tick": args.verified_through,
        "call_count": len(prefix),
        "last_call_tick": prefix[-1]["observed_after_core_tick"],
        "all_call_deltas_match_untouched_trace": True,
        "interval_count": len(gaps),
        "intervals_matching_next_call_delta": sum(g["matches_next"] for g in gaps),
        "intervals_matching_previous_call_delta": sum(g["matches_previous"] for g in gaps),
        "next_delta_exceptions": exceptions,
        "extrapolated_calls": False,
    }
    print(json.dumps(summary, indent=2))


if __name__ == "__main__":
    main()
