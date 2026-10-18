#!/usr/bin/env python3
"""Correlate clean counter5294 actor-pool phases with serialized CPU stacks."""
from __future__ import annotations

import argparse
import hashlib
import json
import struct
from pathlib import Path

ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
PRE_EWRAM_SHA = "11d97d1b48374a08bcf42f5bbeb2d9b8f9afb5b2316a9345cebb29db8b80c9d2"
CORE_SHA = "73411273b6b9bb99e22f2278f8da79f92feb3b80a007a21e18262a915a0f84d4"
BASELINE_SHA = "414f46f98c745134c24c5759668a044517446ba221b975d896bdca5dd2f4fa2e"
STRIDE = 92


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def file_sha(path: Path) -> str:
    return sha(path.read_bytes())


def thumb_bl_target(rom: bytes, return_address: int) -> tuple[int, int] | None:
    site = (return_address & ~1) - 4
    if site < 0x08000000 or site + 4 > 0x08800000:
        return None
    first, second = struct.unpack_from("<HH", rom, site - 0x08000000)
    if first & 0xF800 != 0xF000 or second & 0xF800 != 0xF800:
        return None
    relative = ((first & 0x7FF) << 12) | ((second & 0x7FF) << 1)
    if relative & 0x400000:
        relative -= 0x800000
    return site, (site + 4 + relative) & 0xFFFFFFFF


def actor_pool(row: dict) -> bytes:
    raw = bytes.fromhex(row["traffic_actor_records"]["raw"])
    if len(raw) != 50 * STRIDE:
        raise SystemExit("expected a complete 50-record source actor pool")
    return raw


def changed_slots(before: bytes, after: bytes) -> list[int]:
    return [slot for slot in range(50)
            if before[slot * STRIDE:(slot + 1) * STRIDE]
            != after[slot * STRIDE:(slot + 1) * STRIDE]]


def main() -> int:
    p = argparse.ArgumentParser()
    p.add_argument("--rom", type=Path, required=True)
    p.add_argument("--pre-ewram", type=Path, required=True)
    p.add_argument("--clean-baseline-trace", type=Path, required=True)
    p.add_argument("--trace", type=Path, required=True)
    p.add_argument("--capture-report", type=Path, required=True)
    p.add_argument("--cpu-trace", type=Path, required=True)
    p.add_argument("--state-prefix", type=Path, required=True,
                   help="checkpoint path prefix before '.state-vblank-NNNNN.state'")
    p.add_argument("--iwram-prefix", type=Path, required=True,
                   help="checkpoint path prefix before '.iwram-vblank-NNNNN.bin'")
    p.add_argument("--output", type=Path, required=True)
    a = p.parse_args()

    rom = a.rom.read_bytes()
    if sha(rom) != ROM_SHA:
        raise SystemExit("source ROM hash mismatch")
    pre_ewram = a.pre_ewram.read_bytes()
    if len(pre_ewram) != 0x40000 or sha(pre_ewram) != PRE_EWRAM_SHA:
        raise SystemExit("counter5294 pre-event EWRAM checkpoint hash mismatch")
    baseline = json.loads(a.clean_baseline_trace.read_text(encoding="utf-8"))
    rows = json.loads(a.trace.read_text(encoding="utf-8"))
    if len(baseline) != 3 or len(rows) != 3:
        raise SystemExit("expected two three-VBlank traces")
    if [int(row["counter"]) for row in rows] != [5294, 5295, 5296]:
        raise SystemExit("unhooked trace is not aligned to counters5294–5296")
    if [row.get("input_keys") for row in rows] != [[6, 8], [6, 8], [6, 8]]:
        raise SystemExit("source input schedule differs from the held brake+accelerator event")
    # Current capture versions add telemetry keys. Compare every shared gameplay
    # field, including all raw actors and player/camera state, while ignoring only
    # route-driver telemetry and capture-local tick fields.
    parity = []
    for index, (old, new) in enumerate(zip(baseline, rows)):
        ignored = {"tick", "route_driver", "trip_clock_state", "trip_start_inputs"}
        keys = (set(old) & set(new)) - ignored
        differing = [key for key in sorted(keys) if old[key] != new[key]]
        if differing:
            raise SystemExit(f"fresh unhooked capture differs from clean baseline row{index}: {differing}")
        parity.append({"tick": index, "gameplay_fields_equal": True,
                       "shared_fields_compared": sorted(keys)})

    capture = json.loads(a.capture_report.read_text(encoding="utf-8"))
    if capture.get("status") != "PASS" or capture.get("rom_sha256") != ROM_SHA:
        raise SystemExit("unhooked capture report failed source identity")
    if capture.get("core_sha256") != CORE_SHA:
        raise SystemExit("reference core hash differs from accepted counter5294 run")
    if any(capture.get(name) for name in (
            "instrumentation", "traffic_dispatch_instrumentation",
            "traffic_collision_entry_state_instrumentation",
            "traffic_postimpact_entry_state_instrumentation",
            "traffic_follow_entry_instrumentation")):
        raise SystemExit("capture contains ROM/runtime instrumentation")
    if set(capture.get("state_checkpoints", {})) != {"0", "1", "2"}:
        raise SystemExit("full source-state checkpoints missing")
    if set(capture.get("iwram_checkpoints", {})) != {"0", "1", "2"}:
        raise SystemExit("full IWRAM checkpoints missing")

    pools = [actor_pool(row) for row in rows]
    pre_pool = pre_ewram[0xF4:0xF4 + 50 * STRIDE]
    phases = [changed_slots(pre_pool, pools[0]),
              changed_slots(pools[0], pools[1]),
              changed_slots(pools[1], pools[2])]
    if phases != [list(range(12)), list(range(12, 27)), []]:
        raise SystemExit(f"unhooked source actor-pool phase changed: {phases}")

    cpu = json.loads(a.cpu_trace.read_text(encoding="utf-8"))
    records = {int(record["tick"]): record for record in cpu.get("records", [])}
    stack_observations = {}
    for tick in range(3):
        state_path = Path(str(a.state_prefix) + f".state-vblank-{tick:05d}.state")
        iwram_path = Path(str(a.iwram_prefix) + f".iwram-vblank-{tick:05d}.bin")
        state, iwram = state_path.read_bytes(), iwram_path.read_bytes()
        if len(state) != 397888 or len(iwram) != 0x8000:
            raise SystemExit(f"invalid serialized state/IWRAM size at tick{tick}")
        regs = struct.unpack_from("<16I", state, 0x20)
        cpsr = struct.unpack_from("<I", state, 0x60)[0]
        record = records.get(tick)
        if not record or list(regs) != record["gprs"]:
            raise SystemExit(f"serialized CPU snapshot differs from CPU trace at tick{tick}")
        if regs[15] != record["gprs"][15]:
            raise SystemExit(f"saved PC mismatch at tick{tick}")
        # These two stack words are live saved return-address slots in the
        # unhooked dispatcher-parent frame on the counter5294 VBlank.
        inner_return = struct.unpack_from("<I", iwram, 0x7E2C)[0]
        parent_return = struct.unpack_from("<I", iwram, 0x7E74)[0]
        # At this nested helper, the dispatcher saved r9/SB (loop index) at
        # +0x14 and r6 (current actor pointer) at +0x24.
        dispatcher_loop_index = struct.unpack_from("<I", iwram, 0x7E14)[0]
        dispatcher_actor_pointer = struct.unpack_from("<I", iwram, 0x7E24)[0]
        inner_call = thumb_bl_target(rom, inner_return)
        parent_call = thumb_bl_target(rom, parent_return)
        stack_observations[str(tick)] = {
            "counter": int(rows[tick]["counter"]),
            "pc": hex(regs[15]), "lr": hex(regs[14]), "sp": hex(regs[13]),
            "cpsr": hex(cpsr),
            "stack_word_0x03007E2C": hex(inner_return),
            "stack_word_0x03007E74": hex(parent_return),
            "saved_dispatcher_loop_index_r9": dispatcher_loop_index,
            "saved_dispatcher_actor_pointer_r6": hex(dispatcher_actor_pointer),
            "inner_return_bl": ({"callsite": hex(inner_call[0]),
                                  "target": hex(inner_call[1])} if inner_call else None),
            "parent_return_bl": ({"callsite": hex(parent_call[0]),
                                   "target": hex(parent_call[1])} if parent_call else None),
            "serialized_state_sha256": sha(state),
            "iwram_sha256": sha(iwram),
        }
    tick0, tick1 = stack_observations["0"], stack_observations["1"]
    if (tick0["counter"], tick0["parent_return_bl"], tick0["inner_return_bl"]) != (
            5294, {"callsite": "0x800b378", "target": "0x80066b4"},
            {"callsite": "0x800681c", "target": "0x8029b44"}):
        raise SystemExit(f"counter5294 unhooked stack does not contain the expected dispatcher call chain: {tick0}")
    actor_base = struct.unpack_from("<I", rom, 0x08006788 - 0x08000000)[0]
    selector_ptr, count_table = 0x02001EB0, 0x084A54B0
    selector_byte = pre_ewram[selector_ptr - 0x02000000 + 3]
    selector_index = selector_byte >> 4
    prefix_count = rom[count_table - 0x08000000 + selector_index]
    actor_pointer = int(tick0["saved_dispatcher_actor_pointer_r6"], 16)
    pointer_delta = actor_pointer - actor_base
    if (actor_base != 0x020000F4 or prefix_count != 27
            or selector_index != 4 or pointer_delta < 0
            or pointer_delta % STRIDE != 0
            or pointer_delta // STRIDE != 11
            or tick0["saved_dispatcher_loop_index_r9"] != 11):
        raise SystemExit(f"counter5294 stack does not identify dispatcher actor index11: {tick0}")
    if (tick1["counter"], tick1["parent_return_bl"]) != (
            5295, {"callsite": "0x800b482", "target": "0x802b6a0"}):
        raise SystemExit(f"counter5295 stack did not move to the post-dispatch caller path: {tick1}")
    if tick0["pc"] != "0x8029cea" or tick1["pc"] != "0x30029e0":
        raise SystemExit("unexpected unhooked CPU locations at the two VBlank returns")
    copy_loop = Path(str(a.iwram_prefix) + ".iwram-vblank-00001.bin").read_bytes()[0x29D0:0x2A0C]
    if bytes.fromhex("ff00bae8ff00abe8") not in copy_loop:
        raise SystemExit("counter5295 IWRAM window no longer contains the observed block-copy loop")

    report = {
        "status": "PASS_UNHOOKED_COUNTER5294_DISPATCHER_VBLANK_STACK_CORRELATION",
        "rom_sha256": ROM_SHA,
        "core_sha256": CORE_SHA,
        "pre_event_ewram_sha256": sha(pre_ewram),
        "baseline_trace_sha256": file_sha(a.clean_baseline_trace),
        "unhooked_trace_sha256": file_sha(a.trace),
        "capture_report_sha256": file_sha(a.capture_report),
        "cpu_trace_sha256": file_sha(a.cpu_trace),
        "state_and_iwram_checkpoints": stack_observations,
        "clean_gameplay_field_parity": parity,
        "source_actor_pool_phase_changes": {
            "pre_event_to_counter5294_vblank": phases[0],
            "counter5294_to_counter5295_vblank": phases[1],
            "counter5295_to_counter5296_vblank": phases[2],
        },
        "dispatcher_prefix_selection_from_clean_pre_event_ram": {
            "selector_address": hex(selector_ptr),
            "selected_byte_address": hex(selector_ptr + 3),
            "selected_byte": selector_byte,
            "table_index": selector_index,
            "rom_count_table": hex(count_table),
            "prefix_count": prefix_count,
            "changed_prefix_slots_across_the_two_vblanks": phases[0] + phases[1],
            "unchanged_tail_slots_27_to_49": pre_pool[27 * STRIDE:] == pools[1][27 * STRIDE:],
        },
        "finding": "The uninstrumented counter5294 VBlank stack contains the direct caller return 0x0800B37D for BL site 0x0800B378 to dispatcher 0x080066B4 and nested return 0x08006821 for callsite 0x0800681C. The dispatcher frame's saved r9/SB loop index is 11; its saved r6 actor pointer is 0x020004E8, exactly actor-table base 0x020000F4 + 11*0x5C. The exact pre-event RAM selector selects ROM prefix count27. Thus this single source dispatcher invocation is processing actor slot11 when interrupted at counter5294 VBlank. At counter5295, those dispatcher returns are absent and 0x0800B487 maps from callsite 0x0800B482 to 0x0802B6A0. Clean actor-pool snapshots show slots0–11 changed by the first return and slots12–26 by the second; slots27–49 remain untouched. This establishes one complete 27-record dispatcher prefix crossing counter5294 and completing before counter5295, with exact active loop index11 at the first VBlank.",
        "instrumentation_perturbation_note": "The corrected in-memory r7 gate probe logs delta5 but moves slot11's actor update to a different VBlank than the clean trace. Its per-counter gate counts are therefore diagnostic only; the unhooked CPU/stack checkpoints are the canonical phase evidence.",
        "scope": "Uninstrumented source-ROM execution and read-only serialized CPU/IWRAM checkpoints from the exact counter5294 state. This resolves the direct dispatcher call chain, active actor loop index11, 27-record source prefix, and dispatcher completion across two VBlank returns. It does not establish native parity for the complete 50-record table plus shared player/camera/global/RNG state, route completion/fare parity, or a playable port."
    }
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"],
                      "actor_changes": report["source_actor_pool_phase_changes"],
                      "tick0_dispatch_return": tick0["parent_return_bl"],
                      "tick1_parent_return": tick1["parent_return_bl"]}, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
