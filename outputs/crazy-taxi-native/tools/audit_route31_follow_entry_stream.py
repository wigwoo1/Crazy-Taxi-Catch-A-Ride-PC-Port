#!/usr/bin/env python3
"""Audit a temporary 0x08006908 stream against clean route31 source frames."""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path

ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
COUNTERS = (8212, 8213, 8214)
FIELDS = (
    "input_keys", "keyinput", "heading", "speed", "counter", "previous",
    "frame_delta", "delta", "intro", "position", "angles", "orientation",
    "gravity", "airborne", "raw_input_history", "traffic_camera",
    "driver_owners", "traffic_actor_records", "traffic_globals",
    "traffic_frame_context",
)


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def rows(path: Path) -> dict[int, dict]:
    data = json.loads(path.read_text(encoding="utf-8"))
    result = {}
    for counter in COUNTERS:
        found = [r for r in data if r.get("counter") == counter]
        if len(found) != 1:
            raise SystemExit(f"expected one row for counter {counter}: {path}")
        result[counter] = found[0]
    return result


def actor_summary(event: dict) -> dict:
    ptr = int(event["actor_pointer"], 16)
    offset = ptr - 0x020000F4
    if offset < 0 or offset % 0x5C or offset // 0x5C >= 50:
        raise SystemExit(f"actor pointer outside table: {event['actor_pointer']}")
    before = bytes.fromhex(event["actor_raw"])
    after = bytes.fromhex(event["actor_after_raw"])
    if len(before) != 0x5C or len(after) != 0x5C:
        raise SystemExit("actor record size mismatch")
    changed = [i for i, (a, b) in enumerate(zip(before, after)) if a != b]
    return {
        "counter": event["source_counter"],
        "completion_tick": event["observed_return_tick"],
        "slot": offset // 0x5C,
        "actor_pointer": event["actor_pointer"],
        "argument_r1": event["argument_r1"],
        "word_plus_0x10_signed": struct.unpack_from("<i", before, 0x10)[0],
        "actor_changed_offsets": changed,
        "changed_aligned_words": {
            f"0x{i:02X}": {"before_signed": struct.unpack_from("<i", before, i)[0],
                           "after_signed": struct.unpack_from("<i", after, i)[0]}
            for i in range(0, len(before), 4)
            if before[i:i + 4] != after[i:i + 4]
        },
    }


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--project", type=Path, default=Path("outputs/crazy-taxi-native"))
    p.add_argument("--capture", type=Path, default=Path("work/route31busyret-out"))
    p.add_argument("--label", default="route31branchtrace6908")
    p.add_argument("--output", type=Path, default=Path(
        "outputs/crazy-taxi-native/reports/original-core-reference/route31-follow-entry-stream-audit.json"))
    a = p.parse_args()
    ref = a.project / "reports/original-core-reference"
    clean_path = ref / "route31combineddriverhelper20261012.actor-trace.json"
    helper_path = ref / "route31-helper-index-trace-audit.json"
    probe_path = a.capture / f"{a.label}.actor-trace.json"
    capture_path = a.capture / f"{a.label}.json"
    stream_path = a.capture / f"{a.label}.traffic-follow-entry-trace.json"
    for path in (clean_path, helper_path, probe_path, capture_path, stream_path):
        if not path.is_file():
            raise SystemExit(f"required evidence missing: {path}")
    if sha(clean_path) != "9478d959bdbf4b8168a8a73890441f5b039bef84866d73c71f13d4d4901bfe3e":
        raise SystemExit("clean source trace hash changed")
    capture = json.loads(capture_path.read_text(encoding="utf-8"))
    stream = json.loads(stream_path.read_text(encoding="utf-8"))
    helper = json.loads(helper_path.read_text(encoding="utf-8"))
    if capture.get("status") != "PASS" or capture.get("rom_sha256") != ROM_SHA:
        raise SystemExit("instrumented replay failed or used another ROM")
    if stream.get("instrumentation", {}).get("source_rom_sha256") != ROM_SHA:
        raise SystemExit("entry hook source ROM hash mismatch")
    if stream.get("instrumentation", {}).get("patched_entry") != "0x8006908":
        raise SystemExit("unexpected hook address")
    clean, probe = rows(clean_path), rows(probe_path)
    mismatches = []
    for counter in COUNTERS:
        for field in FIELDS:
            if clean[counter].get(field) != probe[counter].get(field):
                mismatches.append({"counter": counter, "field": field})
    if mismatches:
        raise SystemExit("clean parity failed: " + json.dumps(mismatches[:12]))
    summaries = [actor_summary(e) for e in stream.get("call_events", [])]
    # The slot-8 call begins in capture tick 1 but its return hook is observed
    # in tick 2. The independent dispatcher trace assigns it to source row 8214.
    # Group calls by completion tick to match that source-row convention while
    # preserving the entry's actual source counter in each record.
    observed = {str(8212 + tick): [x["slot"] for x in summaries if x["completion_tick"] == tick]
                for tick in (1, 2)}
    expected = {"8213": [1, 2, 3, 4, 6], "8214": [8, 22]}
    helper_calls = {str(c): [x["slot"] for x in helper["helper_calls_by_source_counter"][str(c)]] for c in (8213, 8214)}
    if observed != expected or observed != helper_calls:
        raise SystemExit(f"callset mismatch: entry={observed}; independent helper={helper_calls}")
    report = {
        "status": "PASS_FOLLOW_ENTRY_STREAM_WITH_CLEAN_SOURCE_PARITY",
        "rom_sha256": ROM_SHA,
        "clean_actor_trace_sha256": sha(clean_path),
        "probe_actor_trace_sha256": sha(probe_path),
        "entry_stream_sha256": sha(stream_path),
        "hook": stream["instrumentation"]["patched_entry"],
        "clean_row_parity": {str(c): True for c in COUNTERS},
        "compared_fields": list(FIELDS),
        "observed_calls": observed,
        "actor_calls": summaries,
        "interpretation": "The seven observed 0x08006908 entries exactly match the independent clean-parity helper trace when grouped by return frame; slot 8 spans the VBlank boundary. The routine changes record fields at the reported offsets. Their semantic mapping is not yet established, so no position or facing claim is made. This does not explain dispatch selection or other actor writers.",
        "next_dependency": "Capture and audit the same event's 0x08006CE8 post-impact entry calls and return-state changes, then join them to dispatch ordering and require full 50-record parity before implementing the native traffic frame.",
        "scope": "Three-frame event-window source parity with one in-memory hook; not full traffic frame or gameplay parity.",
    }
    out = a.output if a.output.is_absolute() else Path.cwd() / a.output
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("status", "clean_row_parity", "observed_calls", "next_dependency")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
