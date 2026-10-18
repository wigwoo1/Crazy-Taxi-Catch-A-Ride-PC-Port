#!/usr/bin/env python3
"""Recover the main gameplay loop's reachable Thumb CFG and test call dominance.

This is static control-flow evidence only. Calls are modeled as returning to
their fallthrough; called routines and runtime conditions are not executed.
"""
import argparse
import hashlib
import json
from collections import deque
from pathlib import Path

ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
ROM_BASE = 0x08000000
ENTRY = 0x0800AF80
END = 0x0800B590
DISPATCH = 0x0800B378
RENDER_CALLBACK = 0x0800B474
RENDER_CALLBACK_TARGET = 0x0802C840
FRAME_SERVICE = 0x0800B57C


def signed(value, bits):
    return value - (1 << bits) if value & (1 << (bits - 1)) else value


def decode(rom, address):
    off = address - ROM_BASE
    h = int.from_bytes(rom[off:off + 2], "little")
    nxt = int.from_bytes(rom[off + 2:off + 4], "little") if off + 4 <= len(rom) else 0
    if h & 0xF800 == 0xF000 and nxt & 0xF800 == 0xF800:
        rel = ((h & 0x7FF) << 12) | ((nxt & 0x7FF) << 1)
        target = address + 4 + signed(rel, 23)
        # For caller-loop recovery the callee is summarized by its return to
        # fallthrough; its body belongs to a separate CFG.
        return 4, "bl", [address + 4], {"target": target}
    if h & 0xF000 == 0xD000 and (h >> 8) & 0xF < 0xE:
        cond = (h >> 8) & 0xF
        target = address + 4 + (signed(h & 0xFF, 8) << 1)
        return 2, "conditional_branch", [target, address + 2], {"condition": cond, "target": target}
    if h & 0xF800 == 0xE000:
        target = address + 4 + (signed(h & 0x7FF, 11) << 1)
        return 2, "branch", [target], {"target": target}
    if h & 0xFF87 == 0x4700:
        return 2, "bx", [], {}
    if h & 0xFE00 == 0xBC00 and h & 0x0100:
        return 2, "pop_pc", [], {}
    return 2, "other", [address + 2], {}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()
    rom = args.rom.read_bytes()
    digest = hashlib.sha256(rom).hexdigest()
    if digest != ROM_SHA256:
        raise SystemExit(f"ROM SHA-256 mismatch: {digest}")

    nodes = {}
    pending = deque([ENTRY])
    while pending:
        address = pending.popleft()
        if address in nodes or not ENTRY <= address < END:
            continue
        size, kind, successors, info = decode(rom, address)
        nodes[address] = {"size": size, "kind": kind, "successors": successors, **info}
        pending.extend(target for target in successors if ENTRY <= target < END)

    if DISPATCH not in nodes or FRAME_SERVICE not in nodes or RENDER_CALLBACK not in nodes:
        raise SystemExit("Expected dispatcher/render-callback/frame-service call site is not reachable")
    if nodes[RENDER_CALLBACK].get("target") != RENDER_CALLBACK_TARGET:
        raise SystemExit("Gameplay render callback call target differs from the reviewed ROM")
    predecessors = {n: set() for n in nodes}
    for source, data in nodes.items():
        for target in data["successors"]:
            if target in nodes:
                predecessors[target].add(source)

    dominators = {n: ({ENTRY} if n == ENTRY else set(nodes)) for n in nodes}
    changed = True
    while changed:
        changed = False
        for node in sorted(nodes):
            if node == ENTRY:
                continue
            preds = predecessors[node]
            incoming = set.intersection(*(dominators[p] for p in preds)) if preds else set()
            new = {node} | incoming
            if new != dominators[node]:
                dominators[node] = new
                changed = True

    calls = []
    for address, data in sorted(nodes.items()):
        if data["kind"] == "bl":
            calls.append({"address": address, "target": data["target"],
                          "dispatcher_target": data["target"] == 0x080066B4,
                          "frame_service_target": data["target"] == 0x08026318})
    loop_back_edges = [{"from": f"0x{address:08X}", "to": f"0x{target:08X}"}
                       for address, data in sorted(nodes.items())
                       for target in data["successors"] if target == ENTRY]
    report = {
        "status": "PASS_STATIC_CFG",
        "rom_sha256": digest,
        "entry": f"0x{ENTRY:08X}",
        "end_exclusive": f"0x{END:08X}",
        "reachable_instruction_count": len(nodes),
        "conditional_branch_count": sum(data["kind"] == "conditional_branch" for data in nodes.values()),
        "direct_call_count": len(calls),
        "reachable_dispatcher_calls": [
            {"address": f"0x{c['address']:08X}", "target": f"0x{c['target']:08X}"}
            for c in calls if c["dispatcher_target"]],
        "reachable_render_callback_calls": [
            {"address": f"0x{c['address']:08X}", "target": f"0x{c['target']:08X}"}
            for c in calls if c["address"] == RENDER_CALLBACK],
        "reachable_frame_service_calls": [
            {"address": f"0x{c['address']:08X}", "target": f"0x{c['target']:08X}"}
            for c in calls if c["frame_service_target"]],
        "dispatcher_dominates_render_callback": DISPATCH in dominators[RENDER_CALLBACK],
        "dispatcher_dominates_frame_service": DISPATCH in dominators[FRAME_SERVICE],
        "render_callback_dominates_frame_service": RENDER_CALLBACK in dominators[FRAME_SERVICE],
        "dispatcher_dominates_each_loop_back_edge": all(
            DISPATCH in dominators[int(edge["from"], 16)] for edge in loop_back_edges),
        "frame_service_dominates_each_loop_back_edge": all(
            FRAME_SERVICE in dominators[int(edge["from"], 16)] for edge in loop_back_edges),
        "loop_back_edges": loop_back_edges,
        "scope": "ROM-static Thumb CFG. BL routines modeled as returning; no runtime branch choice, timing, frame-service behavior, or source-call cadence is proved. Dominance applies to all statically reachable in-range main-loop paths.",
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in (
        "status", "reachable_instruction_count", "conditional_branch_count", "direct_call_count", "reachable_dispatcher_calls",
        "reachable_frame_service_calls", "dispatcher_dominates_render_callback", "dispatcher_dominates_frame_service",
        "dispatcher_dominates_each_loop_back_edge", "frame_service_dominates_each_loop_back_edge",
        "loop_back_edges")}, indent=2))


if __name__ == "__main__":
    main()
