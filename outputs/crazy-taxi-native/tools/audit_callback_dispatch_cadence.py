#!/usr/bin/env python3
"""Join callback-cycle and dispatcher traces from the same ROM checkpoint.

This audit checks observed counter/tick alignment and baseline equality. The
temporary callback probe, post-retro_run logging, and separate dispatcher run
cannot prove instruction order or explain total loop time.
"""
import argparse
import json
import statistics
from pathlib import Path

ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
SHARED_FIELDS = (
    "tick", "input_keys", "keyinput", "heading", "speed", "counter", "previous",
    "frame_delta", "delta", "intro", "position", "angles", "orientation", "gravity",
    "airborne", "raw_input_history", "driver_owners", "traffic_frame_context",
    "route_driver", "completed_count", "fare_whole", "fare_fraction", "remaining_clock",
)


def read_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--report-dir", type=Path, required=True)
    parser.add_argument("--baseline", default="dispatchbaseline700")
    parser.add_argument("--dispatcher", default="dispatchcadence700")
    parser.add_argument("--callback", default="callbacktimingphase522")
    parser.add_argument("--service-summary", type=Path, required=True)
    parser.add_argument("--static-report", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = args.report_dir

    baseline = read_json(root / f"{args.baseline}.actor-trace.json")
    dispatcher_trace = read_json(root / f"{args.dispatcher}.actor-trace.json")
    callback_trace = read_json(root / f"{args.callback}.actor-trace.json")
    baseline_doc = read_json(root / f"{args.baseline}.json")
    dispatcher_doc = read_json(root / f"{args.dispatcher}.json")
    callback_doc = read_json(root / f"{args.callback}.json")
    baseline_route = read_json(root / f"{args.baseline}.route-follow.json")
    dispatcher_route = read_json(root / f"{args.dispatcher}.route-follow.json")
    callback_route = read_json(root / f"{args.callback}.route-follow.json")
    dispatch_doc = read_json(root / f"{args.dispatcher}.traffic-dispatch-entry-trace.json")
    callback_doc_trace = read_json(root / f"{args.callback}.callback-cycle-trace.json")
    cpu_doc = read_json(root / f"{args.callback}.cpu-timing-trace.json")
    service_doc = read_json(args.service_summary)
    static_doc = read_json(args.static_report)

    for doc in (baseline_doc, dispatcher_doc, callback_doc):
        if doc.get("rom_sha256") != ROM_SHA or doc.get("status") != "PASS":
            raise SystemExit("capture status or supplied-ROM hash mismatch")
    if not (baseline_doc["core_sha256"] == dispatcher_doc["core_sha256"] == callback_doc["core_sha256"]):
        raise SystemExit("captures were not run with the same libretro core")
    if not (baseline_route["state_sha256"] == dispatcher_route["state_sha256"] == callback_route["state_sha256"]):
        raise SystemExit("captures were not run from the same serialized source state")
    if not (baseline_route["input_schedule_scope"] == dispatcher_route["input_schedule_scope"] == callback_route["input_schedule_scope"]):
        raise SystemExit("route-follow input driver descriptions differ")

    row_count = len(callback_trace)
    if row_count != 522 or len(baseline) < row_count or len(dispatcher_trace) < row_count:
        raise SystemExit("expected a 522-row callback trace and longer paired references")
    for index, row in enumerate(callback_trace):
        base = baseline[index]
        dispatch = dispatcher_trace[index]
        for field in SHARED_FIELDS:
            if field not in row or field not in base or field not in dispatch:
                raise SystemExit(f"missing shared field {field} at tick {index}")
            if row[field] != base[field] or row[field] != dispatch[field]:
                raise SystemExit(f"baseline mismatch at tick {index}, field {field}")
        for trace in (base, dispatch):
            for nested in ("pointer", "position"):
                if trace["traffic_camera"][nested] != row["traffic_camera"][nested]:
                    raise SystemExit(f"baseline mismatch at tick {index}, camera {nested}")

    dispatch_events = [
        event for event in dispatch_doc["call_events"]
        if event["observed_after_core_tick"] < row_count
    ]
    callback_events = callback_doc_trace["call_events"]
    if len(dispatch_events) != 106 or len(callback_events) != 107:
        raise SystemExit(f"unexpected event counts: dispatcher={len(dispatch_events)} callback={len(callback_events)}")
    if callback_doc_trace["log_saturated"] or not callback_doc_trace["instrumentation"]["timer_counter_controls_preserved"]:
        raise SystemExit("callback timing trace saturated or timer controls were not preserved")
    if callback_doc_trace["instrumentation"]["source_rom_sha256"] != ROM_SHA:
        raise SystemExit("callback probe source ROM hash mismatch")

    dispatch_by_key = {
        ((event["source_counter"] + 1) & 0xFFFFFFFF,
         event["observed_after_core_tick"] + 1): event
        for event in dispatch_events
    }
    matches = []
    unmatched_callbacks = []
    for event in callback_events:
        previous = dispatch_by_key.get((event["source_counter"], event["observed_after_core_tick"]))
        if previous:
            matches.append({
                "callback_index": event["call_index"],
                "dispatcher_index": previous["call_index"],
                "source_counter": event["source_counter"],
                "observed_after_core_tick": event["observed_after_core_tick"],
            })
        else:
            unmatched_callbacks.append(event)
    if len(matches) != 106 or len(unmatched_callbacks) != 1:
        raise SystemExit(f"unexpected source-counter alignment: matches={len(matches)} unmatched={len(unmatched_callbacks)}")
    extra = unmatched_callbacks[0]
    if (extra["source_counter"], extra["observed_after_core_tick"]) != (4707, 511):
        raise SystemExit("the unmatched callback event moved from the reviewed phase boundary")

    service_events = {item["tick"]: item for item in service_doc["events_495_to_521"]}
    if service_doc.get("baseline_prefix_rows") != 522 or service_doc.get("dispatcher_actor_prefix_differences") != 0:
        raise SystemExit("frame-service report does not establish the same baseline-equal prefix")
    if service_doc.get("frame_service_store_events_in_prefix") != 106:
        raise SystemExit("frame-service store count differs from reviewed capture")
    if service_events.get(511, {}).get("frame_service", {}).get("stored_counter") != 4707:
        raise SystemExit("expected frame-service reset at tick511/counter4707")
    if service_events.get(515, {}).get("dispatcher", {}).get("delta") != 6:
        raise SystemExit("expected dispatcher delta6 at tick515")
    if service_events.get(520, {}).get("dispatcher", {}).get("delta") != 9:
        raise SystemExit("expected dispatcher delta9 at tick520")
    if not static_doc.get("dispatcher_dominates_render_callback"):
        raise SystemExit("static CFG did not prove dispatcher callsite dominance over render callback")

    cpu_records = cpu_doc["records"]
    periods = [
        right["master_cycles"] - left["master_cycles"]
        for left, right in zip(cpu_records, cpu_records[1:])
        if right["tick"] >= 1 and left["tick"] >= 0
    ]
    frame_period = int(statistics.median(periods))
    callback_windows = [
        (event["timer3_timer2_overflows"] * 65536 + event["timer2_signed_elapsed"] - 65536,
         event["timer3_timer2_overflows"] * 65536 + event["timer2_signed_elapsed"] + 65536)
        for event in callback_events
    ]
    anomaly_windows = [
        {"source_counter": event["source_counter"], "tick": event["observed_after_core_tick"],
         "lower_cycles": lo, "upper_cycles": hi}
        for event, (lo, hi) in zip(callback_events, callback_windows)
        if 4690 <= event["source_counter"] <= 4717
    ]
    if max(hi for _, hi in callback_windows) > 2 * frame_period:
        raise SystemExit("callback measurement bounds exceed two VBlank periods; audit assumptions need review")

    report = {
        "status": "PASS_BASELINE_EQUAL_CALLBACK_DISPATCH_CADENCE_JOIN",
        "rom_sha256": ROM_SHA,
        "source_state_sha256": callback_route["state_sha256"],
        "baseline_equal_shared_fields": list(SHARED_FIELDS) + ["traffic_camera.pointer", "traffic_camera.position"],
        "baseline_equal_rows": row_count,
        "dispatcher_events_in_prefix": len(dispatch_events),
        "callback_events_in_prefix": len(callback_events),
        "counter_aligned_callback_dispatch_pairs": len(matches),
        "counter_unmatched_callback_events": [
            {"source_counter": extra["source_counter"], "tick": extra["observed_after_core_tick"]}
        ],
        "frame_service_boundary": {
            "tick511_counter_reset": 4707,
            "tick515_dispatch_counter": 4711,
            "tick515_frame_delta": 6,
            "tick515_counter_minus_last_reset": 4,
            "tick520_dispatch_counter": 4716,
            "tick520_frame_delta": 9,
            "tick520_counter_minus_last_reset": 9,
        },
        "static_call_order": {
            "dispatcher_callsite": "0x0800B378",
            "render_callback_callsite": "0x0800B474",
            "dispatcher_dominates_callback": True,
            "cfg_scope": "ROM-static Thumb flow; BL callees modeled as returning; no instruction timing or dynamic branch ordering within a VBlank",
        },
        "callback_window_near_boundary": anomaly_windows,
        "callback_window_all_events_cycles": {
            "lower_min": min(lo for lo, _ in callback_windows),
            "upper_max": max(hi for _, hi in callback_windows),
            "median_frame_period": frame_period,
            "approx_upper_max_ms": round(max(hi for _, hi in callback_windows) / 16777216 * 1000, 3),
        },
        "finding": "Across 522 shared baseline rows, the timer-instrumented callback trace and separately instrumented dispatcher trace preserve the observed gameplay fields. 106 callback observations align with dispatcher entries at source_counter+1/tick+1. One callback is observed at counter4707/tick511 between dispatcher entries counter4701/tick505 and counter4711/tick515; the independent frame-service trace records its previous-counter reset to4707 at tick511. At tick515, the sampled counter difference from that reset is4 but the dispatcher receives delta6; at tick520, the counter difference is9 and the dispatcher receives delta9 before the same-tick frame-service observation stores4716. The static CFG establishes that the dispatcher callsite dominates the render callback callsite on reachable main-loop paths. This is phase/cadence correlation only: post-retro_run logs do not establish exact within-frame call order, and the callback timer window includes trampoline overhead and does not explain total main-loop execution time.",
        "limitations": [
            "Callback timing and dispatcher entry traces are separate instrumented runs from the same saved state, cross-checked against an untouched capture on shared fields.",
            "The route-follow capture reaches waypoint1 and completes zero fares; this is not gameplay parity.",
            "No callback or frame-service timing result is extrapolated beyond captured events.",
            "Text, fonts, HUD, menus, logos, signs, and lettering remain excluded from all upscale paths."
        ],
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({key: report[key] for key in (
        "status", "baseline_equal_rows", "dispatcher_events_in_prefix", "callback_events_in_prefix",
        "counter_aligned_callback_dispatch_pairs", "counter_unmatched_callback_events", "frame_service_boundary",
        "callback_window_all_events_cycles", "limitations")}, indent=2))


if __name__ == "__main__":
    main()
