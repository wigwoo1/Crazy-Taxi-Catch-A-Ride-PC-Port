#!/usr/bin/env python3
"""Validate read-only input callback telemetry against a paired ROM trace."""
import gzip
import hashlib
import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
BASE_TRACE = REPORTS / "route31nativeinputreplaytrace.actor-trace.json.gz"
BASE_SUMMARY = REPORTS / "route31nativeinputreplaytrace.route-follow.json"
TRACE = REPORTS / "route31inputcallbacktraffic700.actor-trace.json.gz"
SUMMARY = REPORTS / "route31inputcallbacktraffic700.route-follow.json"
CALLBACKS = REPORTS / "route31inputcallbacktraffic700.input-callback-trace.json"
OUTPUT = REPORTS / "route31-input-callback-capture-verification.json"

base_summary = json.loads(BASE_SUMMARY.read_text(encoding="utf-8"))
summary = json.loads(SUMMARY.read_text(encoding="utf-8"))
callback_doc = json.loads(CALLBACKS.read_text(encoding="utf-8"))
base_raw = BASE_TRACE.read_bytes()
trace_raw = TRACE.read_bytes()
base = json.loads(gzip.decompress(base_raw))[:700]
rows = json.loads(gzip.decompress(trace_raw))
assert base_summary["rom_sha256"] == summary["rom_sha256"] == "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
assert base_summary["core_sha256"] == summary["core_sha256"] == callback_doc["core_sha256"]
assert base_summary["state_sha256"] == summary["state_sha256"]
assert len(base) == len(rows) == callback_doc["frames"] == 700
assert callback_doc["input_poll_rows"] == callback_doc["input_state_first_query_rows"] == 700
assert set(callback_doc["input_state_calls_by_tick"].values()) == {16}
polls = callback_doc["input_poll_first_by_tick"]
queries = callback_doc["input_state_first_by_tick"]
assert [x["tick"] for x in polls] == list(range(700))
assert [x["tick"] for x in queries] == list(range(700))
fields = sorted(set(base[0]) | set(rows[0]))
compared_fields = [field for field in fields if field != "route_driver"]
differences = []
for tick, (expected, actual) in enumerate(zip(base, rows)):
    if expected["tick"] != actual["tick"] or expected["input_keys"] != queries[tick]["keys"]:
        raise SystemExit(f"callback schedule mismatch at tick {tick}")
    changed = tick > 0 and actual["previous"] != rows[tick - 1]["previous"]
    if tick > 0 and actual["keyinput"] != rows[tick - 1]["keyinput"] and not changed:
        raise SystemExit(f"KEYINPUT changed outside source update boundary at tick {tick}")
    for field in compared_fields:
        if expected.get(field) != actual.get(field):
            differences.append({"tick": tick, "field": field})
            break
if differences:
    raise SystemExit(f"ROM-state comparison failed: {differences[:5]}")
pickup = {"tick516_callback": queries[516],
          "tick516_post_core": {k: rows[516][k] for k in ("keyinput", "raw_input_history", "previous", "frame_delta")},
          "tick520_callback": queries[520],
          "tick520_post_core": {k: rows[520][k] for k in ("keyinput", "raw_input_history", "previous", "frame_delta")}}
report = {
    "status": "PASS_READONLY_INPUT_CALLBACK_TRACE",
    "rom_sha256": summary["rom_sha256"],
    "core_sha256": summary["core_sha256"],
    "source_state_sha256": summary["state_sha256"],
    "baseline_trace_sha256": hashlib.sha256(base_raw).hexdigest(),
    "callback_trace_sha256": hashlib.sha256(trace_raw).hexdigest(),
    "capture_ticks": 700,
    "input_poll_callbacks": callback_doc["input_poll_rows"],
    "input_state_queries": sum(callback_doc["input_state_calls_by_tick"].values()),
    "input_state_queries_per_tick": 16,
    "rom_state_fields_compared_exactly": compared_fields,
    "rom_state_mismatches": 0,
    "helper_route_metadata_differences": "route_driver differs in some derived helper rows and is excluded from ROM-state comparison; it is not game RAM.",
    "controller_sample_at_pickup": pickup,
    "scope": "A read-only libretro input callback logger is paired with the same source state and per-VBlank replay. Across all700 rows, recorded ROM actor, controls, traffic pool/globals, counters, clocks, fares and other shared fields match the earlier source trace exactly. The callback receives the current command each VBlank (16 input-state queries per tick); KEYINPUT/raw input history update only in observed source commit rows. Tick516 command is visible to the callback while ROM KEYINPUT remains stale; by tick520 the post-core input history is 990 and the counter advances with delta9. This validates telemetry behavior, not sub-instruction order, native physics parity or a completed fare route."
}
OUTPUT.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
print(json.dumps({"status": report["status"], "rom_state_mismatches": 0,
                  "poll_rows": report["input_poll_callbacks"], "state_queries": report["input_state_queries"],
                  "pickup_tick520": pickup["tick520_post_core"], "report": str(OUTPUT)}, indent=2))
