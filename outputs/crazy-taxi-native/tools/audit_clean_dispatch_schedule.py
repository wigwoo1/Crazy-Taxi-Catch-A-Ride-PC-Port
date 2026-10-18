#!/usr/bin/env python3
"""Cross-check hooked dispatcher timing against a clean fixed-input replay.

This audit validates the observed dispatcher counter/delta schedule only.
It deliberately does not use hook-mutated actor state as source truth.
"""
from __future__ import annotations

import argparse
import gzip
import hashlib
import json
from pathlib import Path

ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"


def read_json(path: Path) -> dict | list:
    return json.loads(path.read_text(encoding="utf-8"))


def sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--report-dir", type=Path, required=True)
    ap.add_argument("--clean", default="dispatchclean1200")
    ap.add_argument("--hook", default="callbackdispatchminimal1200")
    ap.add_argument("--output", type=Path, required=True)
    args = ap.parse_args()
    root = args.report_dir

    clean_capture = read_json(root / f"{args.clean}.json")
    hook_capture = read_json(root / f"{args.hook}.json")
    hook_route = read_json(root / f"{args.hook}.route-follow.json")
    clean_trace_path = root / f"{args.clean}.actor-trace.json.gz"
    event_path = root / f"{args.hook}.traffic-dispatch-entry-trace.json"
    input_schedule_path = root.parents[1] / "work/dispatcher-1200-inputs.json"
    start_state_path = root / "boot4200.state"
    clean_trace_sha = sha256(clean_trace_path)
    event_sha = sha256(event_path)

    if any(doc.get("status") != "PASS" or doc.get("rom_sha256") != ROM_SHA
           for doc in (clean_capture, hook_capture)):
        raise SystemExit("capture status or supplied-ROM hash is invalid")
    if clean_capture.get("core_sha256") != hook_capture.get("core_sha256"):
        raise SystemExit("clean and hooked captures use different core builds")
    if sha256(start_state_path) != hook_route.get("state_sha256"):
        raise SystemExit("clean and hooked captures use different starting states")
    clean_schedule = clean_capture.get("direct_input_schedule", {})
    if (clean_schedule.get("sha256") != sha256(input_schedule_path)
            or clean_schedule.get("rows") != 1200):
        raise SystemExit("clean capture is not bound to the reviewed fixed-input schedule")

    with gzip.open(clean_trace_path, "rt", encoding="utf-8") as f:
        clean_rows = json.load(f)
    hook_rows = read_json(root / f"{args.hook}.actor-trace.json")
    event_doc = read_json(event_path)
    events = event_doc.get("call_events")
    if not isinstance(events, list) or not clean_rows or len(clean_rows) != len(hook_rows):
        raise SystemExit("trace rows or dispatcher events are missing/malformed")
    if len(clean_rows) != 1200:
        raise SystemExit(f"expected the reviewed 1200-row replay, got {len(clean_rows)}")
    if event_doc.get("instrumentation", {}).get("source_rom_sha256") != ROM_SHA:
        raise SystemExit("dispatcher event log has the wrong source-ROM identity")
    if event_doc.get("log_saturated") is not False:
        raise SystemExit("dispatcher event log saturated or saturation status is absent")

    input_differences = [i for i, (a, b) in enumerate(zip(clean_rows, hook_rows))
                         if a.get("input_keys") != b.get("input_keys")]
    if input_differences:
        raise SystemExit(f"clean and hooked input schedules differ at {input_differences[:5]}")

    mismatches = []
    matched = []
    seen_ticks: set[int] = set()
    for event in events:
        tick = int(event["observed_after_core_tick"])
        counter = int(event["source_counter"])
        delta = int(event["frame_delta"])
        if tick in seen_ticks or not 0 <= tick < len(clean_rows):
            mismatches.append({"event": event, "reason": "duplicate_or_out_of_range_tick"})
            continue
        seen_ticks.add(tick)
        row = clean_rows[tick]
        # The hook samples the current source counter and incoming frame delta
        # at dispatcher entry; compare both to the untouched VBlank row.
        differences = {}
        if int(row["counter"]) != counter:
            differences["counter"] = {"hook": counter, "clean": row["counter"]}
        if int(row["delta"]) != delta:
            differences["delta"] = {"hook": delta, "clean": row["delta"]}
        if differences:
            mismatches.append({"tick": tick, "differences": differences})
        else:
            matched.append({"tick": tick, "source_counter": counter, "delta": delta})

    if len(events) != 236 or len(matched) != len(events) or mismatches:
        raise SystemExit(f"schedule mismatch: events={len(events)} matched={len(matched)} bad={len(mismatches)}")

    actor_rows_equal = sum(
        a["traffic_actor_records"]["raw"] == b["traffic_actor_records"]["raw"]
        for a, b in zip(clean_rows, hook_rows)
    )
    report = {
        "status": "PASS_CLEAN_REPLAY_DISPATCH_COUNTER_DELTA_SCHEDULE",
        "rom_sha256": ROM_SHA,
        "frames": len(clean_rows),
        "input_schedule_equal": True,
        "dispatcher_events": len(events),
        "events_matched_to_clean_counter_and_delta": len(matched),
        "event_ticks": [x["tick"] for x in matched],
        "actor_pool_equal_rows": actor_rows_equal,
        "actor_pool_divergence_ticks": [i for i, (a, b) in enumerate(zip(clean_rows, hook_rows))
                                         if a["traffic_actor_records"]["raw"] != b["traffic_actor_records"]["raw"]],
        "clean_trace_sha256": clean_trace_sha,
        "hook_event_trace_sha256": event_sha,
        "finding": "The clean, unmodified fixed-input replay independently reproduces the instrumented dispatcher's observed source-counter and delta at every logged event. This establishes a non-invasive timing cross-check only; the hooked run's traffic actor records are not accepted past its first parity divergence.",
        "scope": "Cross-check of 236 instrumented dispatcher entry counter/delta observations against a separate clean replay with the same starting state and exact input sequence. Does not prove that instrumentation preserves actor evolution, reconstruct internal helper ordering, or establish full gameplay parity.",
        "next_dependency": "Use the clean-derived boundaries to advance a native driver/input state continuously between traffic updates, then compare each untouched source checkpoint including player, all 50 traffic actors and shared globals.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in (
        "status", "frames", "input_schedule_equal", "dispatcher_events",
        "events_matched_to_clean_counter_and_delta", "actor_pool_equal_rows",
        "actor_pool_divergence_ticks", "scope")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
