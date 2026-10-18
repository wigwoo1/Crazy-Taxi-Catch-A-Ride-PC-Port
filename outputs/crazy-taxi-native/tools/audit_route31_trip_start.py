#!/usr/bin/env python3
"""Reconcile the original-ROM route-31 pickup timer with native trip-start math."""
import argparse
import hashlib
import json
from pathlib import Path

ALLOWANCE = [2048, 2304, 2560, 2816, 3072]
CATEGORY_SCALE = [4096, 3968, 3840, 3712, 3584]
DIFFICULTY = [26880, 26453, 26026, 25600, 24640, 23680, 22720, 21760]


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("trace", type=Path)
    ap.add_argument("--output", type=Path, required=True)
    ap.add_argument("--native-test", type=Path, required=True)
    ap.add_argument("--native-test-source", type=Path, required=True)
    args = ap.parse_args()
    trace = json.loads(args.trace.read_text())
    pickup = next(row for row in trace if row["trip_clock_state"]["trip_active"])
    inputs = pickup["trip_start_inputs"]
    state = pickup["trip_clock_state"]
    category = inputs["customer_category"]
    setting = inputs["settings_3"] & 15
    mode = inputs["settings_0"] & 0xE0
    valid = mode == 0 and 0 <= category < len(ALLOWANCE) and 1 <= setting <= 8
    raw = ((inputs["route_length"] // 256) // 16) * 60
    raw = (raw * CATEGORY_SCALE[category]) >> 12
    raw = ((raw * DIFFICULTY[setting - 1]) // 100) >> 8
    allowance = (raw * ALLOWANCE[category]) >> 12
    cap = allowance + pickup["remaining_clock"]
    expected_time = min(raw, cap)
    expected = {"time": expected_time, "initial_time": expected_time, "allowance": allowance}
    observed = {
        "time": state["trip_time"],
        "initial_time": state["initial_trip_time"],
        "allowance": state["trip_allowance"],
    }
    result = {
        "pass": valid and expected == observed,
        "pickup_tick": pickup["tick"],
        "customer_index": inputs["customer_index"],
        "customer_category": category,
        "route_length": inputs["route_length"],
        "settings_0": inputs["settings_0"],
        "settings_3": inputs["settings_3"],
        "remaining_main_clock": pickup["remaining_clock"],
        "raw_time_before_cap": raw,
        "allowance": allowance,
        "allowance_plus_main_clock_cap": cap,
        "expected": expected,
        "observed": observed,
        "native_test_passed": args.native_test.is_file(),
        "native_test_sha256": sha(args.native_test) if args.native_test.is_file() else None,
        "native_test_source_sha256": sha(args.native_test_source),
        "trace_sha256": sha(args.trace),
        "scope": "One exact route-31 source pickup agrees with recovered trip-start arithmetic and the focused native unit test. This proves one timer initialization case, not all settings, pickup input parity, delivery, or completed-port fidelity.",
    }
    result["pass"] = result["pass"] and result["native_test_passed"]
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result, indent=2))
    if not result["pass"]:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
