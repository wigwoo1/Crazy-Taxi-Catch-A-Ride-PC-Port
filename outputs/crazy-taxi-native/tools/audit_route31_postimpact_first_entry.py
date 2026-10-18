#!/usr/bin/env python3
"""Qualify the first 0x08006CE8 entry probe and record its parity limit."""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path

ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
FIELDS = (
    "input_keys", "keyinput", "heading", "speed", "counter", "previous",
    "frame_delta", "delta", "intro", "position", "angles", "orientation",
    "gravity", "airborne", "raw_input_history", "traffic_camera",
    "driver_owners", "traffic_actor_records", "traffic_globals",
    "traffic_frame_context",
)


def load(path: Path):
    return json.loads(path.read_text(encoding="utf-8"))


def sha(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def row_map(path: Path) -> dict[int, dict]:
    rows = load(path)
    result = {int(r["counter"]): r for r in rows}
    if len(result) != len(rows):
        raise SystemExit(f"duplicate source counter in {path}")
    return result


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--capture", type=Path, default=Path("work/route31busyret-out"))
    p.add_argument("--label", default="route31ce8firstentry30safe")
    p.add_argument("--clean-label", default="route31clean30b")
    p.add_argument("--project", type=Path,
                   default=Path(r"./"))
    p.add_argument("--output", type=Path, default=Path(
        "outputs/crazy-taxi-native/reports/original-core-reference/route31-postimpact-first-entry-audit.json"))
    a = p.parse_args()
    probe = a.capture / f"{a.label}.traffic-postimpact-entry-state.json"
    probe_capture = a.capture / f"{a.label}.json"
    probe_rows_path = a.capture / f"{a.label}.actor-trace.json"
    clean_capture = a.capture / f"{a.clean_label}.json"
    clean_rows_path = a.capture / f"{a.clean_label}.actor-trace.json"
    for path in (probe, probe_capture, probe_rows_path, clean_capture, clean_rows_path):
        if not path.is_file():
            raise SystemExit(f"required evidence missing: {path}")
    event = load(probe)
    cfg = load(a.project / "reports/traffic-postimpact-cfg.json")
    cfg_bytes = {int(i["address"], 16): i["bytes"] for i in cfg["instructions"]}
    required_cfg = {0x08006CFA: "b16d", 0x08006CFE: "091a", 0x08006D00: "b165"}
    if cfg.get("rom_sha256") != ROM_SHA or any(cfg_bytes.get(pc) != data for pc, data in required_cfg.items()):
        raise SystemExit("post-impact collision-timer source instructions changed")
    pi = event.get("instrumentation", {})
    if event.get("source_counter") != 8218 or pi.get("source_rom_sha256") != ROM_SHA:
        raise SystemExit("post-impact entry identity mismatch")
    if pi.get("patched_entry") != "0x8006ce8" or "UNFILTERED first" not in event.get("scope", ""):
        raise SystemExit("capture is not the declared unfiltered first-entry probe")
    pointer = int(event["incoming_r0_actor_pointer"], 16)
    if pointer < 0x020000F4 or (pointer - 0x020000F4) % 0x5C:
        raise SystemExit("incoming pointer is not a traffic-record base")
    slot = (pointer - 0x020000F4) // 0x5C
    if slot >= 50 or slot != 3:
        raise SystemExit(f"unexpected first post-impact slot: {slot}")
    pc = load(probe_capture)
    cc = load(clean_capture)
    if any(x.get("status") != "PASS" or x.get("rom_sha256") != ROM_SHA
           for x in (pc, cc)):
        raise SystemExit("capture run or source ROM identity did not pass")
    clean, instrumented = row_map(clean_rows_path), row_map(probe_rows_path)
    if set(clean) != set(instrumented) or len(clean) != 30:
        raise SystemExit("30-frame clean and instrumented windows do not align")
    differences = []
    for counter in sorted(clean):
        for field in FIELDS:
            if clean[counter].get(field) != instrumented[counter].get(field):
                differences.append({"counter": counter, "field": field})
    expected = [{"counter": 8218, "field": "traffic_actor_records"}]
    if differences != expected:
        raise SystemExit("parity boundary changed: " + json.dumps(differences[:20]))

    def record(row: dict, index: int) -> bytes:
        return bytes.fromhex(row["traffic_actor_records"]["raw"])[index * 0x5C:(index + 1) * 0x5C]

    before, after = record(clean[8217], slot), record(clean[8218], slot)
    gate_countdown = struct.unpack_from("<i", before, 0x10)[0]
    timer_before = struct.unpack_from("<i", before, 0x58)[0]
    timer_after = struct.unpack_from("<i", after, 0x58)[0]
    global_delta = clean[8218]["delta"]
    if (gate_countdown, timer_before, timer_after, global_delta) != (-7, 120, 115, 5):
        raise SystemExit("clean source countdown/collision-timer transition changed")
    changed_words = []
    for offset in range(0, 0x5C, 4):
        if before[offset:offset + 4] != after[offset:offset + 4]:
            changed_words.append({
                "offset": f"0x{offset:02X}",
                "before_u32": struct.unpack_from("<I", before, offset)[0],
                "after_u32": struct.unpack_from("<I", after, offset)[0],
            })
    entry_record_copy = bytes.fromhex(event["actor0_raw"])
    report = {
        "status": "PASS_DYNAMIC_POSTIMPACT_ENTRY_WITH_PARITY_BOUNDARY",
        "rom_sha256": ROM_SHA,
        "entry_snapshot_sha256": sha(probe),
        "instrumented_actor_trace_sha256": sha(probe_rows_path),
        "clean_actor_trace_sha256": sha(clean_rows_path),
        "entry": {
            "address": pi["patched_entry"],
            "source_counter": event["source_counter"],
            "incoming_actor_pointer": event["incoming_r0_actor_pointer"],
            "traffic_slot": slot,
            "capture_tick": event.get("observed_after_core_tick"),
            "entry_record_semantics_note": "The one-shot hook separately copies actor slot 0 into actor0_raw; it does not log the incoming slot's full record.",
        },
        "clean_preceding_row": {
            "counter": 8217,
            "slot": slot,
            "word_0_u32": struct.unpack_from("<I", before)[0],
            "countdown_gate_0x10_i32": gate_countdown,
            "collision_timer_0x58_i32": timer_before,
        },
        "clean_following_row": {
            "counter": 8218,
            "slot": slot,
            "global_delta": global_delta,
            "collision_timer_0x58_i32": timer_after,
            "collision_timer_transition_matches_0x08006CE8": timer_after == timer_before - global_delta,
            "changed_aligned_words": changed_words,
        },
        "source_instruction_evidence": {
            "cfg_report": "reports/traffic-postimpact-cfg.json",
            "rom_sha256": cfg["rom_sha256"],
            "load_timer": {"address": "0x08006CFA", "bytes": cfg_bytes[0x08006CFA], "operand": "[r6,#0x58]"},
            "subtract_delta": {"address": "0x08006CFE", "bytes": cfg_bytes[0x08006CFE]},
            "store_timer": {"address": "0x08006D00", "bytes": cfg_bytes[0x08006D00], "operand": "[r6,#0x58]"},
        },
        "replay_parity": {
            "clean_frames": len(clean),
            "instrumented_frames": len(instrumented),
            "all_fields_match_before_entry_frame": all(
                clean[c].get(f) == instrumented[c].get(f)
                for c in clean if c < 8218 for f in FIELDS),
            "differences_at_entry_frame": differences,
            "unrelated_timer_deltas_at_slot_9_to_13": {
                str(i): struct.unpack_from("<i", record(instrumented[8218], i), 0x10)[0]
                - struct.unpack_from("<i", record(clean[8218], i), 0x10)[0]
                for i in range(9, 14)
            },
            "source_parity_qualified_for_return_state": False,
        },
        "interpretation": "The first observed 0x08006CE8 call in this replay is for slot3 at counter8218. Its +0x10 countdown gate is -7 before the call. The clean source record's +0x58 collision timer changes from120 to115 with global delta5, matching the routine's load/subtract/store at 0x08006CFA-0x08006D00. The instrumented traffic table still diverges in five unrelated records at that frame, so the hooked return state remains unqualified.",
        "next_dependency": "Develop a cycle- and state-preserving capture of 0x08006CE8 input/output (or a clean-frame alternative), verify the exact caller-order callset, and require complete actor-table parity before using it in the native traffic implementation.",
        "scope": "One first-entry capture in an in-memory ROM replay plus a 30-frame clean control. Instrumented replay is exact before the routine frame but has a bounded actor-table mismatch at counter8218; no native game implementation changed.",
    }
    out = a.output if a.output.is_absolute() else Path.cwd() / a.output
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in (
        "status", "entry", "replay_parity", "next_dependency")}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
