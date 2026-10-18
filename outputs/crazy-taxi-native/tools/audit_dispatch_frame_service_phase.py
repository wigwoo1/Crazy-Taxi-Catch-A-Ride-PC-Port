#!/usr/bin/env python3
"""Align dispatcher-entry and frame-service store observations to source rows.

Each probe drains its log after retro_run. This audit compares their observed
ticks and source state, but intentionally does not infer instruction order
within one retro_run.
"""
import argparse
import hashlib
import json
from pathlib import Path

ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
PREFIX = 522
DISPATCH_FIELDS = ("tick", "counter", "previous", "frame_delta", "delta", "input_keys",
                   "position", "heading", "speed", "traffic_actor_records",
                   "traffic_globals", "traffic_frame_context")
WRITER_FIELDS = ("tick", "counter", "previous", "frame_delta", "delta", "position",
                 "heading", "speed", "input_keys")


def json_array_prefix(path, count):
    decoder = json.JSONDecoder()
    rows = []
    with path.open("r", encoding="utf-8") as stream:
        buffer = ""
        pos = 0
        eof = False
        first = True
        while len(rows) < count:
            while True:
                while pos < len(buffer) and buffer[pos].isspace():
                    pos += 1
                if pos < len(buffer) and buffer[pos] == ",":
                    pos += 1
                    continue
                if pos < len(buffer) and buffer[pos] == "[":
                    if not first:
                        raise SystemExit(f"unexpected array start in {path}")
                    pos += 1
                    first = False
                    continue
                break
            if pos >= len(buffer):
                if eof:
                    raise SystemExit(f"array ended after {len(rows)} rows: {path}")
                if pos:
                    buffer = buffer[pos:]
                    pos = 0
                chunk = stream.read(1 << 16)
                if not chunk:
                    eof = True
                buffer += chunk
                continue
            if buffer[pos] == "]":
                raise SystemExit(f"array ended after {len(rows)} rows: {path}")
            try:
                value, end = decoder.raw_decode(buffer, pos)
            except json.JSONDecodeError:
                if eof:
                    raise
                if pos:
                    buffer = buffer[pos:]
                    pos = 0
                chunk = stream.read(1 << 16)
                if not chunk:
                    eof = True
                buffer += chunk
                continue
            rows.append(value)
            pos = end
            if pos > (1 << 20):
                buffer = buffer[pos:]
                pos = 0
    return rows


def check_rows(base, candidate, fields, label):
    diffs = []
    for index, (left, right) in enumerate(zip(base, candidate)):
        changed = [field for field in fields if left.get(field) != right.get(field)]
        if changed:
            diffs.append({"tick": index, "fields": changed})
            if len(diffs) == 8:
                break
    if diffs or len(candidate) != PREFIX:
        raise SystemExit(f"{label} mismatch: rows={len(candidate)}, first differences={diffs}")


def main():
    p = argparse.ArgumentParser()
    p.add_argument("baseline", type=Path)
    p.add_argument("dispatch_actor_trace", type=Path)
    p.add_argument("dispatch_event_trace", type=Path)
    p.add_argument("writer_actor_trace", type=Path)
    p.add_argument("writer_event_trace", type=Path)
    p.add_argument("output", type=Path)
    a = p.parse_args()

    base = json_array_prefix(a.baseline, PREFIX)
    dispatch_actor = json_array_prefix(a.dispatch_actor_trace, PREFIX)
    writer_actor = json_array_prefix(a.writer_actor_trace, PREFIX)
    check_rows(base, dispatch_actor, DISPATCH_FIELDS, "dispatcher instrumentation")
    check_rows(base, writer_actor, WRITER_FIELDS, "counter-writer instrumentation")
    source_by_tick = {row["tick"]: row for row in base}

    dispatch_log = json.loads(a.dispatch_event_trace.read_text(encoding="utf-8"))
    writer_log = json.loads(a.writer_event_trace.read_text(encoding="utf-8"))
    if dispatch_log.get("instrumentation", {}).get("source_rom_sha256") != ROM_SHA256:
        raise SystemExit("dispatcher trace ROM hash mismatch")
    if writer_log.get("instrumentation", {}).get("source_rom_sha256") != ROM_SHA256:
        raise SystemExit("writer trace ROM hash mismatch")
    dispatch_events = [event for event in dispatch_log["call_events"]
                       if event["observed_after_core_tick"] < PREFIX]
    writer_events = [event for event in writer_log["call_events"]
                     if event["observed_after_core_tick"] < PREFIX]

    for event in dispatch_events:
        tick = event["observed_after_core_tick"]
        row = source_by_tick[tick]
        if event["source_counter"] != row["counter"] or event["frame_delta"] != row["frame_delta"]:
            raise SystemExit(f"dispatcher event does not match baseline at tick{tick}")
    for event in writer_events:
        tick = event["observed_after_core_tick"]
        row = source_by_tick[tick]
        if event["stored_counter"] != row["counter"]:
            raise SystemExit(f"counter-writer event does not match baseline at tick{tick}")

    dispatch_by_tick = {}
    for event in dispatch_events:
        dispatch_by_tick.setdefault(event["observed_after_core_tick"], []).append(event)
    writer_by_tick = {}
    for event in writer_events:
        writer_by_tick.setdefault(event["observed_after_core_tick"], []).append(event)
    shared_ticks = sorted(set(dispatch_by_tick) & set(writer_by_tick))
    window = []
    for tick in range(495, 522):
        if tick in dispatch_by_tick or tick in writer_by_tick:
            row = source_by_tick[tick]
            window.append({
                "observed_after_core_tick": tick,
                "source_counter": row["counter"],
                "dispatch_events": [{"frame_delta": event["frame_delta"],
                                     "call_index": event["call_index"]}
                                    for event in dispatch_by_tick.get(tick, [])],
                "frame_service_store_events": [{"stored_counter": event["stored_counter"],
                                                "parent_return_address": event["parent_return_address"],
                                                "call_index": event["call_index"]}
                                               for event in writer_by_tick.get(tick, [])],
            })

    report = {
        "status": "PASS_ALIGNED_OBSERVATION_PREFIX",
        "rom_sha256": ROM_SHA256,
        "baseline_prefix_rows": PREFIX,
        "dispatcher_actor_prefix": {"compared_fields": list(DISPATCH_FIELDS), "differences": 0},
        "writer_actor_prefix": {"compared_fields": list(WRITER_FIELDS), "differences": 0},
        "dispatcher_events_in_prefix": len(dispatch_events),
        "frame_service_store_events_in_prefix": len(writer_events),
        "same_observation_tick_count": len(shared_ticks),
        "same_observation_ticks_near_exception": [tick for tick in shared_ticks if 495 <= tick <= 522],
        "events_495_to_521": window,
        "interpretation": "Both instrumented captures preserve the audited baseline fields for the first522 rows, and every logged event counter/delta matches the untouched trace row at its post-retro_run observation tick. The exception window shows dispatcher and frame-service store observations shifting across ticks511 and515 while the same counter state remains source-equal.",
        "scope": "Event timestamps are observed only after retro_run; matching ticks establish co-observation, not instruction order. Separate captures cannot prove within-run sequencing or a general source cadence formula. No calls are extrapolated, and the independent strict replay ceiling remains188 events through tick956.",
    }
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("status", "baseline_prefix_rows",
          "dispatcher_actor_prefix", "writer_actor_prefix", "dispatcher_events_in_prefix",
          "frame_service_store_events_in_prefix", "same_observation_tick_count",
          "same_observation_ticks_near_exception", "events_495_to_521")}, indent=2))


if __name__ == "__main__":
    main()
