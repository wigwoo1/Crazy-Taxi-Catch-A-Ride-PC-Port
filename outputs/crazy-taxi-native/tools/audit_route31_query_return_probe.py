#!/usr/bin/env python3
"""Check a temporary route31 query hook against clean source frames."""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path


ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
CLEAN_TRACE_SHA = "9478d959bdbf4b8168a8a73890441f5b039bef84866d73c71f13d4d4901bfe3e"
COUNTERS = (8212, 8213, 8214)
FIELDS = (
    "input_keys", "keyinput", "heading", "speed", "counter", "previous",
    "frame_delta", "delta", "intro", "position", "angles", "orientation",
    "gravity", "airborne", "raw_input_history", "traffic_camera",
    "driver_owners", "traffic_actor_records", "traffic_globals",
    "traffic_frame_context",
)


def digest(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for block in iter(lambda: f.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def read_json(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


def unique_rows(path: Path, counters: tuple[int, ...]) -> dict[int, dict]:
    rows = read_json(path)
    if not isinstance(rows, list):
        raise SystemExit(f"unexpected actor trace shape: {path}")
    result = {}
    for counter in counters:
        matching = [row for row in rows if row.get("counter") == counter]
        if len(matching) != 1:
            raise SystemExit(f"expected one row for counter {counter} in {path}, got {len(matching)}")
        result[counter] = matching[0]
    return result


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--project", type=Path, default=Path("outputs/crazy-taxi-native"))
    parser.add_argument("--capture", type=Path, default=Path("work/route31busyret-out"))
    parser.add_argument("--label", default="route31querytrace")
    parser.add_argument("--output", type=Path,
                        default=Path("outputs/crazy-taxi-native/reports/original-core-reference/route31-query-path-zero-hit-audit.json"))
    args = parser.parse_args()
    ref = args.project / "reports/original-core-reference"
    clean_path = ref / "route31combineddriverhelper20261012.actor-trace.json"
    helper_path = ref / "route31-helper-index-trace-audit.json"
    probe_path = args.capture / f"{args.label}.actor-trace.json"
    probe_capture_path = args.capture / f"{args.label}.json"
    query_path = args.capture / f"{args.label}.traffic-query-return-trace.json"
    for path in (clean_path, helper_path, probe_path, probe_capture_path, query_path):
        if not path.is_file():
            raise SystemExit(f"required capture/evidence is missing: {path}")

    clean_hash = digest(clean_path)
    if clean_hash != CLEAN_TRACE_SHA:
        raise SystemExit(f"clean reference hash changed: {clean_hash}")
    capture = read_json(probe_capture_path)
    query = read_json(query_path)
    helper = read_json(helper_path)
    if capture.get("status") != "PASS" or capture.get("rom_sha256") != ROM_SHA:
        raise SystemExit("instrumented capture did not pass or used a different ROM")
    if helper.get("status") != "PASS_SHORT_EVENT_ALIGNED_HELPER_TRACE_WITH_SOURCE_PARITY":
        raise SystemExit("helper call evidence is not clean-source parity qualified")
    if helper.get("rom_sha256") != ROM_SHA or helper.get("clean_trace_sha256") != clean_hash:
        raise SystemExit("helper audit does not bind to the supplied ROM and selected clean trace")
    if query.get("instrumentation", {}).get("source_rom_sha256") != ROM_SHA:
        raise SystemExit("query hook metadata does not match the supplied ROM revision")
    if query.get("instrumentation", {}).get("hook") != "0x08006760":
        raise SystemExit("unexpected query hook location")
    if query.get("instrumentation", {}).get("displaced_bytes") != "011c002c":
        raise SystemExit("query hook did not declare the exact displaced instructions")
    if query.get("hook_reached") or query.get("logged_count") != 0 or query.get("call_events"):
        raise SystemExit("the captured probe unexpectedly executed the positive-timer query path")

    clean = unique_rows(clean_path, COUNTERS)
    probed = unique_rows(probe_path, COUNTERS)
    mismatches = []
    for counter in COUNTERS:
        if probed[counter].get("input_keys") != [8]:
            mismatches.append({"counter": counter, "field": "input_keys", "probe": probed[counter].get("input_keys")})
        for field in FIELDS:
            if field not in clean[counter] or field not in probed[counter]:
                mismatches.append({"counter": counter, "field": field, "reason": "missing field"})
            elif clean[counter][field] != probed[counter][field]:
                mismatches.append({"counter": counter, "field": field})
    if mismatches:
        raise SystemExit("instrumented source frame differs from clean reference: " + json.dumps(mismatches[:12]))

    calls = helper.get("helper_calls_by_source_counter", {})
    observed_calls = {str(k): [int(x["slot"]) for x in v] for k, v in calls.items() if int(k) in (8213, 8214)}
    if observed_calls != {"8213": [1, 2, 3, 4, 6], "8214": [8, 22]}:
        raise SystemExit(f"event helper callset changed: {observed_calls}")
    entry_gates = [int(x["word_plus_0x10_signed"])
                   for values in calls.values() for x in values]
    if any(value > 0 for value in entry_gates):
        raise SystemExit("an observed follow call had a positive +0x10 entry gate")

    report = {
        "status": "PASS_QUERY_PATH_NOT_TAKEN_WITH_CLEAN_INSTRUMENTED_REPLAY",
        "rom_sha256": ROM_SHA,
        "clean_actor_trace_sha256": clean_hash,
        "probe_actor_trace_sha256": digest(probe_path),
        "query_trace_sha256": digest(query_path),
        "instrumented_rom_sha256": query["instrumentation"]["instrumented_rom_sha256"],
        "query_hook": query["instrumentation"]["hook"],
        "query_hook_reached": False,
        "query_calls_logged": 0,
        "clean_row_parity": {str(counter): True for counter in COUNTERS},
        "compared_fields": list(FIELDS),
        "source_helper_calls": observed_calls,
        "source_call_entry_gate_words_nonpositive": True,
        "interpretation": (
            "The temporary hook after the second 0x08032454 path query records no calls during this "
            "counter8212-8214 route31 event, while every instrumented player, camera, traffic-table, "
            "global, input and timing field matches the clean source trace. All seven observed follow "
            "calls enter with a nonpositive +0x10 word, so this event takes the dispatcher’s separate "
            "nonpositive-timer/flag branch; the positive-timer expiry query path is not the missing "
            "selector for slots8 and22 in this event."
        ),
        "next_dependency": (
            "Trace the two nonpositive-timer branch callees at 0x08006908 and 0x08006CE8 with actor "
            "pointer, source counter, flag bits and return-state snapshots. Determine why radius-qualified "
            "slots8 and22 reach the follow/collision helper at counter8214 while slots1,2,3,4,6 do so at "
            "8213. Do not infer that the unexecuted 0x08032454 query results select this callset."
        ),
        "scope": "One clean route31 event and one state-preserving in-memory instrumentation replay; not full traffic-frame or gameplay parity.",
    }
    out = args.output
    if not out.is_absolute():
        out = Path.cwd() / out
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in (
        "status", "query_hook_reached", "clean_row_parity", "source_helper_calls", "next_dependency")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
