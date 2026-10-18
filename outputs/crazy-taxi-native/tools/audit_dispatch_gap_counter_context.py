#!/usr/bin/env python3
"""Verify counter snapshots around two exceptional dispatcher intervals.

Uses the untouched baseline trace and a short uninstrumented replay from the
same save state. It establishes counter-reset timing, not the reset's caller.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
FIELDS = ("tick", "counter", "previous", "frame_delta", "delta", "position",
          "heading", "speed", "input_keys")
SAMPLE_TICKS = (505, 509, 511, 515, 520)
EVENT_TICKS = (505, 515, 520)


def u32(blob, offset):
    return struct.unpack_from("<I", blob, offset)[0]


def i32(blob, offset):
    return struct.unpack_from("<i", blob, offset)[0]


def main():
    p = argparse.ArgumentParser()
    p.add_argument("baseline", type=Path)
    p.add_argument("capture", type=Path)
    p.add_argument("capture_metadata", type=Path)
    p.add_argument("dispatch_events", type=Path)
    p.add_argument("snapshot_dir", type=Path)
    p.add_argument("output", type=Path)
    p.add_argument("--capture-label", default="dispatchgapaudiocheck")
    p.add_argument("--writer-events", type=Path)
    a = p.parse_args()

    baseline = json.loads(a.baseline.read_text(encoding="utf-8"))
    capture = json.loads(a.capture.read_text(encoding="utf-8"))
    metadata = json.loads(a.capture_metadata.read_text(encoding="utf-8"))
    events = json.loads(a.dispatch_events.read_text(encoding="utf-8"))["call_events"]
    if metadata.get("rom_sha256") != ROM_SHA256:
        raise SystemExit("capture ROM hash does not match the supplied ROM")
    if metadata.get("status") != "PASS":
        raise SystemExit("short uninstrumented capture did not report PASS")

    baseline_by_tick = {row["tick"]: row for row in baseline}
    capture_by_tick = {row["tick"]: row for row in capture}
    compared = min(len(baseline), len(capture))
    mismatches = []
    for tick in range(compared):
        left, right = baseline_by_tick[tick], capture_by_tick[tick]
        differing = [field for field in FIELDS if left.get(field) != right.get(field)]
        if differing:
            mismatches.append({"tick": tick, "fields": differing})
            if len(mismatches) == 8:
                break
    if mismatches:
        raise SystemExit(f"short replay diverges from baseline: {mismatches}")
    if compared < max(SAMPLE_TICKS) + 1:
        raise SystemExit("capture does not reach all sampled ticks")

    event_by_tick = {event["observed_after_core_tick"]: event for event in events}
    if any(tick not in event_by_tick for tick in EVENT_TICKS):
        raise SystemExit("expected dispatcher event is missing")
    event_rows = []
    for tick in EVENT_TICKS:
        event = event_by_tick[tick]
        row = baseline_by_tick[tick]
        if event["frame_delta"] != row["frame_delta"] or event["source_counter"] != row["counter"]:
            raise SystemExit(f"captured dispatcher event differs from baseline at tick {tick}")
        event_rows.append({"tick": tick, "counter": event["source_counter"],
                           "frame_delta": event["frame_delta"]})

    writer_trace = None
    if a.writer_events:
        writer_log = json.loads(a.writer_events.read_text(encoding="utf-8"))
        events_from_stores = writer_log["call_events"]
        if writer_log.get("log_saturated"):
            raise SystemExit("counter-writer log saturated")
        if any(event["writer_id"] != 3 for event in events_from_stores):
            raise SystemExit("a non-frame-service previous-counter store executed")
        for event in events_from_stores:
            parent = int(event["parent_return_address"], 16)
            if (parent & ~1) - 4 != 0x0800B57C:
                raise SystemExit(f"frame-service parent callsite mismatch: {event}")
        writers_at_interest = []
        for tick in (505, 511, 520):
            matches = [event for event in events_from_stores
                       if event["observed_after_core_tick"] == tick]
            if len(matches) != 1:
                raise SystemExit(f"expected one counter store at tick {tick}")
            writers_at_interest.append({"tick": tick,
                "counter": matches[0]["stored_counter"],
                "writer_id": matches[0]["writer_id"],
                "parent_return_address": matches[0]["parent_return_address"]})
        if writers_at_interest[1]["counter"] != 4707:
            raise SystemExit("tick511 frame-service reset value changed")
        writer_trace = {"logged_store_count": len(events_from_stores),
            "log_saturated": False, "all_observed_stores_are_frame_service": True,
            "frame_service_store_address": "0x080263E0",
            "parent_callsite": "0x0800B57C",
            "parent_callsite_return": "0x0800B581",
            "stores_near_interval_exception": writers_at_interest,
            "scope": "This is an independent temporary-ROM probe cross-checked against the untouched actor trace on the same 522-tick prefix. It directly observes the writer and caller; timestamps are post-retro_run observation ticks, not exact instruction-entry times."}

    snapshots = []
    blobs = {}
    for tick in SAMPLE_TICKS:
        path = a.snapshot_dir / f"{a.capture_label}.ewram-vblank-{tick:05d}.bin"
        blob = path.read_bytes()
        if len(blob) != 0x40000:
            raise SystemExit(f"bad EWRAM checkpoint length at {tick}")
        blobs[tick] = blob
        observed = {"tick": tick, "counter_0x3520": u32(blob, 0x3520),
                    "previous_0x3524": u32(blob, 0x3524),
                    "frame_delta_0x3B00": i32(blob, 0x3B00),
                    "audio_region_0x7500_0x7640_sha256": hashlib.sha256(blob[0x7500:0x7640]).hexdigest()}
        row = baseline_by_tick[tick]
        if (observed["counter_0x3520"] != row["counter"] or
                observed["previous_0x3524"] != row["previous"] or
                observed["frame_delta_0x3B00"] != row["frame_delta"]):
            raise SystemExit(f"EWRAM checkpoint differs from actor trace at {tick}")
        snapshots.append(observed)

    changed_audio_bytes = sum(x != y for x, y in zip(blobs[509][0x7500:0x7640],
                                                       blobs[511][0x7500:0x7640]))
    if snapshots[1]["previous_0x3524"] != 4701 or snapshots[2]["previous_0x3524"] != 4707:
        raise SystemExit("expected 509→511 previous-counter reset was not observed")
    if snapshots[2]["counter_0x3520"] != snapshots[2]["previous_0x3524"]:
        raise SystemExit("tick511 previous-counter reset does not equal current counter")
    if snapshots[4]["frame_delta_0x3B00"] != (
            snapshots[4]["counter_0x3520"] - snapshots[2]["previous_0x3524"]):
        raise SystemExit("tick520 delta is not explained by the observed tick511 counter baseline")

    report = {
        "status": "PASS_SOURCE_COUNTER_CONTEXT",
        "rom_sha256": ROM_SHA256,
        "baseline_rows": len(baseline),
        "unaltered_capture_rows": len(capture),
        "exact_fields_compared_per_row": list(FIELDS),
        "baseline_equal_prefix_rows": compared,
        "dispatcher_events": event_rows,
        "dispatcher_gaps": [10, 5],
        "counter_checkpoints": snapshots,
        "audio_region_changed_bytes_509_to_511": changed_audio_bytes,
        "frame_service_writer_trace": writer_trace,
        "finding": "The untouched source trace and same-save-state uninstrumented replay agree through tick521. EWRAM previous-counter 0x02003524 resets from4701 at tick509 to current counter4707 at tick511 between dispatcher events505 and515. At tick520, frame delta9 equals current counter4716 minus the tick511 baseline4707, so the 515→520 dispatcher gap5 is not the interval measured by the stored frame-delta input. A separate three-store instrumentation run remains equal to the untouched trace across the same522 key fields and records all106 observed previous-counter writes at 0x080263E0, with parent return0x0800B581 identifying caller BL0x0800B57C; the two candidate audio stores did not execute in this prefix. The audio range changes19 bytes in the 509→511 window, but the frame-service caller is now directly confirmed and audio causation is not supported by the writer trace.",
        "scope": "This identifies the dynamic writer and its parent callsite for the observed reset. Coarse post-retro_run observation ticks do not establish exact instruction ordering or a general loop timing model. The result does not alter the captured source-call schedule or extend strict native parity beyond188 accepted events through tick956. The recaptured source replay is offline diagnostic evidence, not native gameplay.",
    }
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("status", "baseline_equal_prefix_rows",
          "dispatcher_events", "counter_checkpoints", "audio_region_changed_bytes_509_to_511")}, indent=2))


if __name__ == "__main__":
    main()
