#!/usr/bin/env python3
"""Verify the source radius gate against a clean route31 traffic trace.

This audit combines the statically recovered 0x5A00 per-axis bounds with the
event-aligned original-ROM actor/helper captures. It deliberately treats the
radius check as a necessary filter only; it does not infer the missing
per-record predicate or dispatcher timing.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path


EXPECTED_ROM = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
EXPECTED_TRACE = "9478d959bdbf4b8168a8a73890441f5b039bef84866d73c71f13d4d4901bfe3e"
EXPECTED_STRIDE = 92
EXPECTED_COUNT = 50
RADIUS = 0x5A00
CALLS_BY_COUNTER = {8213: [1, 2, 3, 4, 6], 8214: [8, 22]}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def i32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<i", data, offset)[0]


def fail(message: str) -> None:
    raise SystemExit(message)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--project", type=Path, default=Path("outputs/crazy-taxi-native"))
    parser.add_argument("--output", type=Path,
                        default=Path("reports/original-core-reference/route31-dispatch-radius-audit.json"))
    args = parser.parse_args()
    project = args.project
    reports = project / "reports/original-core-reference"
    actor_path = reports / "route31combineddriverhelper20261012.actor-trace.json"
    helper_path = reports / "route31-helper-index-trace-audit.json"
    bounds_path = reports / "route31-postgate-predicates-audit.json"
    for required in (actor_path, helper_path, bounds_path):
        if not required.is_file():
            fail(f"required source evidence is missing: {required}")

    actor_hash = sha256(actor_path)
    if actor_hash != EXPECTED_TRACE:
        fail(f"clean actor trace hash mismatch: {actor_hash}")
    helper = json.loads(helper_path.read_text(encoding="utf-8"))
    bounds = json.loads(bounds_path.read_text(encoding="utf-8"))
    if helper.get("status") != "PASS_SHORT_EVENT_ALIGNED_HELPER_TRACE_WITH_SOURCE_PARITY":
        fail("helper call report does not prove clean-source parity")
    if helper.get("rom_sha256") != EXPECTED_ROM or bounds.get("rom_sha256") != EXPECTED_ROM:
        fail("reference evidence belongs to a different ROM revision")
    if helper.get("clean_trace_sha256") != actor_hash:
        fail("helper report is not tied to the selected clean actor trace")
    if bounds.get("status") != "PASS_STATIC_ROM_POSTGATE_TRAFFIC_CALL_PREDICATES":
        fail("static ROM predicate audit did not pass")
    if bounds.get("source_derived_gate_sequence", []).count(
            "Before the ground and follow/collision helper region, both player-relative axis magnitudes must be <= (0xB4 << 7), or 0x5A00.") != 1:
        fail("expected source-derived per-axis bound is not present")

    rows = json.loads(actor_path.read_text(encoding="utf-8"))
    if not isinstance(rows, list):
        fail("actor trace has an unexpected shape")
    selected_rows = {}
    for counter in (8212, 8213, 8214):
        matches = [row for row in rows if row.get("counter") == counter]
        if len(matches) != 1:
            fail(f"expected exactly one clean row at counter {counter}, got {len(matches)}")
        selected_rows[counter] = matches[0]

    radius_candidates: dict[int, list[dict[str, int]]] = {}
    for counter, row in selected_rows.items():
        pos = row.get("position")
        raw_hex = row.get("traffic_actor_records", {}).get("raw", "")
        try:
            raw = bytes.fromhex(raw_hex)
        except ValueError as exc:
            fail(f"invalid actor table at counter {counter}: {exc}")
        if len(pos or []) != 3 or len(raw) != EXPECTED_COUNT * EXPECTED_STRIDE:
            fail(f"malformed clean actor snapshot at counter {counter}")
        px, pz = int(pos[0]), int(pos[2])
        candidates = []
        for slot in range(EXPECTED_COUNT):
            offset = slot * EXPECTED_STRIDE
            flags = struct.unpack_from("<I", raw, offset)[0]
            gate_word = i32(raw, offset + 0x10)
            x = i32(raw, offset + 0x14)
            z = i32(raw, offset + 0x1C)
            dx, dz = x - px, z - pz
            if abs(dx) <= RADIUS and abs(dz) <= RADIUS:
                candidates.append({"slot": slot, "flags": flags,
                                   "gate_word": gate_word, "dx": dx, "dz": dz})
        radius_candidates[counter] = candidates

    observed = {
        int(counter): sorted(int(call["slot"]) for call in calls)
        for counter, calls in helper.get("helper_calls_by_source_counter", {}).items()
        if int(counter) in CALLS_BY_COUNTER
    }
    if observed != CALLS_BY_COUNTER:
        fail(f"event-aligned helper callset changed: {observed}")
    union = sorted(set(observed[8213]) | set(observed[8214]))
    expected_candidates = [item["slot"] for item in radius_candidates[8212]]
    if expected_candidates != union:
        fail(f"source pre-event radius candidates {expected_candidates} != observed call union {union}")
    if not set(observed[8213]).issubset(expected_candidates):
        fail("first call group contains an actor outside the source radius")
    if not set(observed[8214]).issubset(expected_candidates):
        fail("second call group contains an actor outside the source radius")
    delayed = sorted(set(expected_candidates) - set(observed[8213]))
    if delayed != observed[8214]:
        fail(f"delayed radius candidates differ from second call group: {delayed}")

    report = {
        "status": "PASS_RADIUS_CANDIDATE_SET_BUT_NOT_SOURCE_DISPATCH_SCHEDULE",
        "rom_sha256": EXPECTED_ROM,
        "clean_actor_trace": actor_path.as_posix(),
        "clean_actor_trace_sha256": actor_hash,
        "helper_call_audit": helper_path.as_posix(),
        "helper_call_audit_sha256": sha256(helper_path),
        "static_bound_audit": bounds_path.as_posix(),
        "static_bound_audit_sha256": sha256(bounds_path),
        "source_bound_per_axis": RADIUS,
        "pre_event_counter": 8212,
        "pre_event_radius_candidates": radius_candidates[8212],
        "observed_calls_by_counter": {str(k): v for k, v in sorted(observed.items())},
        "radius_candidates_at_counters": {
            str(k): [item["slot"] for item in values]
            for k, values in sorted(radius_candidates.items())
        },
        "interpretation": (
            "The clean source state immediately before this event places exactly seven actors within "
            "the recovered per-axis 0x5A00 bound, and those seven are exactly the union of the seven "
            "observed ROM follow/collision helper calls. The ROM dispatches five in the 8213 counter "
            "window and the other two in 8214. Therefore the spatial bound is a necessary candidate "
            "filter for this event but is not sufficient to explain the per-record call timing; the "
            "remaining query/route/phase predicate must be recovered before encoding a native schedule."
        ),
        "next_dependency": (
            "Trace the ROM path-query return registers and selected route-table words for the two "
            "radius-qualified records delayed from counter8213 to8214 (slots8 and22), then compare "
            "the actual event order at the helper call boundary. Do not add radius-only scheduling "
            "to production traffic code."
        ),
        "scope": "One clean route31 event around counters8212-8214; no full traffic or gameplay parity claim.",
    }
    output = args.output
    if not output.is_absolute():
        output = Path.cwd() / output
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in (
        "status", "pre_event_radius_candidates", "observed_calls_by_counter",
        "radius_candidates_at_counters", "next_dependency")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
