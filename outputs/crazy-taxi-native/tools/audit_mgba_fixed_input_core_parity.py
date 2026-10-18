#!/usr/bin/env python3
"""Audit fixed-input parity between the installed mGBA core and source builds."""
from pathlib import Path
import csv
import hashlib
import json
import struct

ROOT = Path(__file__).resolve().parents[1]
REF = ROOT / "reports/original-core-reference"
ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
LABELS = {
    "adaptive_installed": "mainloopvblankpickupbase",
    "fixed_installed": "mgbasefixedref",
    "fixed_source": "mgbasefixedzigmingw",
}


def load_json(path):
    return json.loads(path.read_text(encoding="utf-8"))


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def gameplay_fields(row):
    # route_driver is test-driver telemetry, not emulated game state.
    return {key: value for key, value in row.items() if key != "route_driver"}


def main():
    traces = {
        label: load_json(REF / f"{name}.actor-trace.json")
        for label, name in LABELS.items()
    }
    runs = {label: load_json(REF / f"{name}.json") for label, name in LABELS.items()}
    for label, run in runs.items():
        assert run["rom_sha256"] == ROM_SHA256, (label, "ROM hash mismatch")
    assert all(len(rows) == 1200 for rows in traces.values())

    baseline = traces["adaptive_installed"]
    installed = traces["fixed_installed"]
    candidate = traces["fixed_source"]
    input_csv = REF / "route31pickupstate516-inputs.csv"
    with input_csv.open(newline="", encoding="utf-8") as stream:
        replay = list(csv.DictReader(stream))
    assert len(replay) == 1200
    for index, row in enumerate(baseline):
        keys = set(row["input_keys"])
        steer = -1 if 6 in keys else 1 if 7 in keys else 0
        brake = 1 if 0 in keys else 0
        assert (int(replay[index]["tick"]), int(replay[index]["steer"]),
                int(replay[index]["brake"])) == (index, steer, brake), index

    installed_differences = [
        index for index, pair in enumerate(zip(baseline, installed))
        if gameplay_fields(pair[0]) != gameplay_fields(pair[1])
    ]
    assert not installed_differences, installed_differences[:8]
    assert all(a["input_keys"] == b["input_keys"]
               for a, b in zip(installed, candidate))

    source_differences = [
        index for index, pair in enumerate(zip(installed, candidate))
        if gameplay_fields(pair[0]) != gameplay_fields(pair[1])
    ]
    assert source_differences and source_differences[0] == 255
    first = source_differences[0]
    installed_raw = bytes.fromhex(installed[first]["traffic_actor_records"]["raw"])
    source_raw = bytes.fromhex(candidate[first]["traffic_actor_records"]["raw"])
    changed_bytes = [
        index for index, (left, right) in enumerate(zip(installed_raw, source_raw))
        if left != right
    ]
    assert changed_bytes == [945], changed_bytes[:16]
    slot, offset = divmod(changed_bytes[0], 92)
    installed_y = struct.unpack_from("<i", installed_raw, slot * 92 + 0x18)[0]
    source_y = struct.unpack_from("<i", source_raw, slot * 92 + 0x18)[0]
    assert (slot, offset, installed_y, source_y) == (10, 25, 19200, 0)

    installed_y_256 = struct.unpack_from(
        "<i", bytes.fromhex(installed[256]["traffic_actor_records"]["raw"]), slot * 92 + 0x18
    )[0]
    source_y_256 = struct.unpack_from(
        "<i", bytes.fromhex(candidate[256]["traffic_actor_records"]["raw"]), slot * 92 + 0x18
    )[0]
    assert installed_y_256 == source_y_256

    variant_hashes = {
        name: sha256(REF / f"{name}.actor-trace.json")
        for name in ("mgbasecandidate", "mgbasecandidaterelease",
                     "mgbasecandidatezigmingw")
    }
    assert len(set(variant_hashes.values())) == 1, variant_hashes
    tick384_installed = installed[384]
    tick384_candidate = candidate[384]

    report = {
        "status": "PASS_FIXED_INPUT_REPLAY_ISOLATES_INSTALLED_CORE_CADENCE_MISMATCH",
        "source_rom_sha256": ROM_SHA256,
        "baseline": "mainloopvblankpickupbase.actor-trace.json",
        "fixed_input_csv": input_csv.name,
        "fixed_input_csv_sha256": sha256(input_csv),
        "frames": 1200,
        "input_schedule_rows": len(replay),
        "installed_core_replay_matches_baseline": True,
        "installed_match_scope": (
            "All 1,200 per-VBlank fields match after excluding only route_driver telemetry; "
            "active-low input keys and all 50 raw traffic actor records are identical."
        ),
        "source_candidate_revision": "c30aaa8f42b5b786924d955630b29cd990176968",
        "source_candidate_core_sha256": runs["fixed_source"]["core_sha256"],
        "installed_core_sha256": runs["fixed_installed"]["core_sha256"],
        "source_build_variants_adaptive_trace_sha256": variant_hashes,
        "source_build_variants_produce_identical_trace": True,
        "first_fixed_input_mismatch": {
            "tick": first,
            "fields": [key for key in set(installed[first]) | set(candidate[first])
                       if key != "route_driver" and
                       installed[first].get(key) != candidate[first].get(key)],
            "traffic_actor_changed_bytes": len(changed_bytes),
            "actor_slot": slot,
            "actor_stride": 92,
            "record_byte_offset": offset,
            "source_record_field": "actor_y",
            "source_record_field_offset": "0x18",
            "traffic_actor_y_installed": installed_y,
            "traffic_actor_y_candidate": source_y,
            "actor_y_matches_by_tick_256": True,
        },
        "source_mismatch_rows": len(source_differences),
        "tick_384": {
            "source_counter_both": tick384_installed["counter"],
            "installed_previous": tick384_installed["previous"],
            "candidate_previous": tick384_candidate["previous"],
            "same_input_keys": tick384_installed["input_keys"] == tick384_candidate["input_keys"],
        },
        "route_result": {
            "installed_fixed_final_waypoint": load_json(REF / "mgbasefixedref.route-follow.json")["final_waypoint"],
            "source_fixed_final_waypoint": load_json(REF / "mgbasefixedzigmingw.route-follow.json")["final_waypoint"],
            "installed_fare": load_json(REF / "mgbasefixedref.route-follow.json")["fare"],
            "source_fare": load_json(REF / "mgbasefixedzigmingw.route-follow.json")["fare"],
        },
        "interpretation": (
            "Fixed-input playback removes adaptive steering as the explanation. The installed core "
            "reproduces the baseline source-state trace exactly; the public-source Zig/MinGW build does not. "
            "At tick255 only one raw traffic byte first differs; at tick384 the previous-update counters "
            "differ by five while source counter and fixed input match. Do not use this source-built core "
            "for instruction-level timing claims."
        ),
        "scope": (
            "Scripted incomplete route only, zero fare. Core parity evidence, not native gameplay parity, "
            "a playable route, or sustained native 60 Hz."
        ),
    }
    out = REF / "mgba-fixed-input-core-parity-audit.json"
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({
        "status": report["status"],
        "installed_fixed_input_match": report["installed_core_replay_matches_baseline"],
        "source_mismatch_rows": len(source_differences),
        "first_mismatch_tick": first,
        "tick384_previous": [tick384_installed["previous"], tick384_candidate["previous"]],
        "report": str(out),
    }, indent=2))


if __name__ == "__main__":
    main()
