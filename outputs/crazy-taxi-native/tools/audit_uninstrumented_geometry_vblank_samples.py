#!/usr/bin/env python3
"""Count passive VBlank CPU snapshots whose IWRAM window matches geometry code.

This is a point-sample residency audit, never a CPU-time estimate.
"""
import argparse
import collections
import hashlib
import json
from pathlib import Path

ROM_SHA256 = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
ROM_BASE = 0x08000000
GEOMETRY_START = 0x080031EC
GEOMETRY_END = 0x08003AB8
CPU_FIELDS = ("tick", "master_cycles", "gprs", "cpsr", "spsr",
              "cycles_since_event", "next_event_cycles", "iwram_window_base",
              "iwram_window")


def sha256(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser()
    for name in ("rom", "cpu_trace", "bus_cpu_trace", "packed_trace",
                 "prior_packed_trace", "capture_metadata", "output"):
        parser.add_argument(name, type=Path)
    args = parser.parse_args()

    rom = args.rom.read_bytes()
    if hashlib.sha256(rom).hexdigest() != ROM_SHA256:
        raise SystemExit("ROM hash mismatch")
    metadata = json.loads(args.capture_metadata.read_text(encoding="utf-8"))
    if metadata.get("rom_sha256") != ROM_SHA256 or metadata.get("status") != "PASS":
        raise SystemExit("capture metadata does not pass ROM/status checks")
    if metadata.get("instrumentation"):
        raise SystemExit("capture metadata reports ROM instrumentation")

    cpu = json.loads(args.cpu_trace.read_text(encoding="utf-8"))["records"]
    bus_cpu = json.loads(args.bus_cpu_trace.read_text(encoding="utf-8"))["records"]
    packed_doc = json.loads(args.packed_trace.read_text(encoding="utf-8"))
    prior_doc = json.loads(args.prior_packed_trace.read_text(encoding="utf-8"))
    if len(cpu) != 1201 or len(bus_cpu) != 1201:
        raise SystemExit("expected initial CPU state plus 1,200 VBlank samples")
    for row, other in zip(cpu, bus_cpu):
        if any(row.get(key) != other.get(key) for key in CPU_FIELDS):
            raise SystemExit("CPU snapshot fields differ from bus capture")
    if packed_doc != prior_doc:
        raise SystemExit("packed driving trace differs from established reference")
    packed = packed_doc["records"]
    if len(packed) != 1201 or [r["tick"] for r in packed] != list(range(-1, 1200)):
        raise SystemExit("packed driving trace does not cover ticks -1..1199")
    by_tick = {r["tick"]: r for r in packed}

    matches = []
    start, end = GEOMETRY_START - ROM_BASE, GEOMETRY_END - ROM_BASE
    for row in cpu:
        if row["tick"] < 0 or not row.get("iwram_window"):
            continue
        window = bytes.fromhex(row["iwram_window"])
        offsets = []
        pos = 0
        while True:
            pos = rom.find(window, pos, end)
            if pos < 0:
                break
            if pos >= start:
                offsets.append(ROM_BASE + pos)
            pos += 1
        if offsets:
            matches.append({"tick": row["tick"],
                            "frame_delta": by_tick[row["tick"]]["frame_delta"],
                            "rom_window_candidates": [hex(x) for x in offsets]})
    if len(matches) != 104:
        raise SystemExit(f"expected 104 geometry window matches, got {len(matches)}")
    gaps = [b["tick"] - a["tick"] for a, b in zip(matches, matches[1:])]
    if gaps and min(gaps) <= 1:
        raise SystemExit("matches are not isolated VBlank samples")
    geometry_deltas = collections.Counter(r["frame_delta"] for r in matches)
    all_deltas = collections.Counter(r["frame_delta"] for r in packed if r["tick"] >= 0)
    report = {
        "status": "PASS_UNINSTRUMENTED_GEOMETRY_SAMPLE_AUDIT",
        "rom_sha256": ROM_SHA256,
        "capture": {"name": metadata.get("label", "cpuWindowsLeft"),
                    "frames": metadata["frames"], "instrumented": False,
                    "cpu_samples_including_initial": len(cpu),
                    "cpu_fields_match_bus_capture": True,
                    "packed_trace_matches_prior_reference": True},
        "post_vblank_samples": 1200,
        "geometry_window_matches": len(matches),
        "sample_fraction": len(matches) / 1200,
        "longest_consecutive_match_run": 1 if matches else 0,
        "minimum_match_gap_frames": min(gaps) if gaps else None,
        "frame_delta_histograms": {
            "geometry_sample_rows": dict(sorted(geometry_deltas.items())),
            "all_capture_rows": dict(sorted(all_deltas.items()))},
        "matched_samples": matches,
        "input_sha256": {"cpu_trace": sha256(args.cpu_trace),
                         "bus_cpu_trace": sha256(args.bus_cpu_trace),
                         "packed_trace": sha256(args.packed_trace),
                         "prior_packed_trace": sha256(args.prior_packed_trace),
                         "capture_metadata": sha256(args.capture_metadata)},
        "interpretation": (
            "Exact 64-byte IWRAM windows matching ROM routine 0x080031EC..0x08003AB8 "
            "appear in isolated passive VBlank snapshots. This is point-sample residency, "
            "not CPU-time percentage, duration, or cycle cost. Similar frame-delta "
            "histograms do not establish causation."),
        "scope": ("Source ROM is hash-checked; CPU snapshot fields and packed driving "
                  "trace are compared with independent references. No dispatcher events "
                  "are extrapolated; this is not proof of playable native parity.")}
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({"status": report["status"], "matches": len(matches),
                      "samples": 1200, "minimum_gap": report["minimum_match_gap_frames"]},
                     indent=2))


if __name__ == "__main__":
    main()
