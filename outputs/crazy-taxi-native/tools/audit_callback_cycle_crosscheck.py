#!/usr/bin/env python3
"""Cross-check two temporary callback timers against untouched gameplay traces."""
import argparse
import hashlib
import json
from pathlib import Path

EXPECTED_ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"

def read_json(path):
    return json.loads(path.read_text(encoding="utf-8"))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--report-dir", type=Path, required=True)
    parser.add_argument("--baseline", default="callbackcyclebase700")
    parser.add_argument("--timer01", default="callbackcycleprobe700v7")
    parser.add_argument("--timer23", default="callbackcycleprobe700v9")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    root = args.report_dir
    baseline = read_json(root / f"{args.baseline}.actor-trace.json")
    t01_trace = read_json(root / f"{args.timer01}.actor-trace.json")
    t23_trace = read_json(root / f"{args.timer23}.actor-trace.json")
    t01_doc = read_json(root / f"{args.timer01}.callback-cycle-trace.json")
    t23_doc = read_json(root / f"{args.timer23}.callback-cycle-trace.json")
    t01 = t01_doc["call_events"]
    t23 = t23_doc["call_events"]
    t01_inst = t01_doc["instrumentation"]
    t23_inst = t23_doc["instrumentation"]

    if not (len(baseline) == len(t01_trace) == len(t23_trace) == 140):
        raise SystemExit("expected exactly 140 rows in baseline and both probes")
    if baseline != t01_trace or baseline != t23_trace:
        raise SystemExit("instrumented actor trace differs from untouched baseline")
    if len(t01) != len(t23) or len(t01) != 26:
        raise SystemExit("callback event counts differ or are outside expected fixture")
    if t01_doc["log_saturated"] or t23_doc["log_saturated"]:
        raise SystemExit("callback timing log saturated")
    if t01_inst["source_rom_sha256"] != EXPECTED_ROM_SHA256 or t23_inst["source_rom_sha256"] != EXPECTED_ROM_SHA256:
        raise SystemExit("source ROM hash differs from the supplied ROM")
    if t01_inst["timer0_reload_values"] != ["0xfce2"]:
        raise SystemExit("Timer0 reload is not stable at the expected value")
    if not t01_inst["timer1_cascade_control_preserved"]:
        raise SystemExit("Timer1 did not remain in cascade mode during every callback")
    if not t23_inst["timer_counter_controls_preserved"]:
        raise SystemExit("Timer2/Timer3 controls changed during a callback")

    comparisons = []
    for a, b in zip(t01, t23):
        if (a["source_counter"] != b["source_counter"]
                or a["callback_pointer"] != b["callback_pointer"]
                or a["observed_after_core_tick"] != b["observed_after_core_tick"]):
            raise SystemExit("timer probes did not observe the same ordered callback on the same frame")
        if a["callback_pointer"] != "0x3002620":
            raise SystemExit("unexpected callback pointer")
        elapsed_timer01 = (a["timer1_timer0_overflows"] * (65536 - 0xFCE2)
                           + a["timer0_return"] - a["timer0_entry"])
        elapsed_timer23 = (b["timer3_timer2_overflows"] * 65536
                           + b["timer2_return"] - b["timer2_entry"])
        if elapsed_timer01 <= 0 or elapsed_timer23 <= 0:
            raise SystemExit("timer produced a nonpositive elapsed window")
        comparisons.append({
            "source_counter": a["source_counter"],
            "observed_after_core_tick_timer0_timer1": a["observed_after_core_tick"],
            "observed_after_core_tick_timer2_timer3": b["observed_after_core_tick"],
            "timer0_timer1_window_cycles": elapsed_timer01,
            "timer2_timer3_window_cycles": elapsed_timer23,
            "difference_cycles": elapsed_timer23 - elapsed_timer01,
        })

    differences = [abs(row["difference_cycles"]) for row in comparisons]
    result = {
        "status": "PASS_CROSSCHECK_DIAGNOSTIC",
        "scope": "Two temporary in-memory GBA timer probes around one indirect callback; not exact callback-only cycles and not native-port runtime.",
        "source_rom_sha256": EXPECTED_ROM_SHA256,
        "source_rom_modified": False,
        "baseline_and_probe_actor_rows": len(baseline),
        "actor_trace_differences": 0,
        "callback_pointer": "0x03002620",
        "callback_event_count_per_probe": len(comparisons),
        "log_saturated": False,
        "timer0_reload": "0xfce2",
        "timer0_period_cycles": 65536 - 0xFCE2,
        "timer1_probe_control_preserved": True,
        "timer1_original_data_values_vary_but_are_saved_and_restored": not t01_inst.get("timer1_original_registers_stable", True),
        "timer2_timer3_probe_controls_preserved": True,
        "timer2_timer3_elapsed_bounds_uncertainty_cycles": 65536,
        "timer0_timer1_sampling_uncertainty_cycles": 65536 - 0xFCE2,
        "cross_probe_absolute_difference_cycles": {
            "maximum": max(differences),
            "within_one_timer0_period_plus_17_cycles": sum(d <= (65536 - 0xFCE2 + 17) for d in differences),
            "count": len(differences),
        },
        "combined_observed_callback_plus_probe_window_cycles": {
            "minimum_timer0_timer1": min(row["timer0_timer1_window_cycles"] for row in comparisons),
            "maximum_timer0_timer1": max(row["timer0_timer1_window_cycles"] for row in comparisons),
            "minimum_timer2_timer3": min(row["timer2_timer3_window_cycles"] for row in comparisons),
            "maximum_timer2_timer3": max(row["timer2_timer3_window_cycles"] for row in comparisons),
        },
        "route_result": "The 140-frame source replay remains an incomplete route-follow capture with zero completed fares.",
        "events": comparisons,
    }
    if result["cross_probe_absolute_difference_cycles"]["within_one_timer0_period_plus_17_cycles"] != len(comparisons):
        raise SystemExit("the independent timer windows disagree beyond the measured tolerance")
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: v for k, v in result.items() if k != "events"}, indent=2))

if __name__ == "__main__":
    main()
