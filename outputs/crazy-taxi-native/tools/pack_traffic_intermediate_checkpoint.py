"""Pack one original-ROM per-VBlank actor/global checkpoint for offline replay.

The output is a test fixture, not a runtime asset. It retains the supplied
ROM's frame counter and selection counter beside the full 50-slot pool.
"""
import hashlib
import json
import pathlib
import struct
import sys

from pack_native_traffic_dispatch_fixture import actor_table, globals_pack


def main():
    if len(sys.argv) != 5:
        raise SystemExit("usage: pack_traffic_intermediate_checkpoint.py EWRAM ACTOR_TRACE TICK OUTPUT")
    ewram_path = pathlib.Path(sys.argv[1])
    trace_path = pathlib.Path(sys.argv[2])
    tick = int(sys.argv[3])
    output_path = pathlib.Path(sys.argv[4])
    ewram = ewram_path.read_bytes()
    assert len(ewram) == 0x40000
    trace = json.loads(trace_path.read_text(encoding="utf-8"))
    frame = next((item for item in trace if item.get("tick") == tick), None)
    assert frame is not None, f"tick {tick} missing from actor trace"
    actor_raw = ewram[0xF4:0xF4 + 50 * 0x5C]
    assert frame["traffic_actor_records"]["raw"] == actor_raw.hex(), "trace and EWRAM actor pools disagree"
    root = pathlib.Path(__file__).resolve().parents[1]
    roads = json.loads((root / "assets/traffic/city1-roads.json").read_text(encoding="utf-8"))
    descriptors = {int(path["descriptor_address"], 16): int(path["index"]) for path in roads["paths"]}
    actors = actor_table(actor_raw, descriptors)
    source_globals = frame["traffic_globals"]
    packed_globals = globals_pack({
        "cooldown_e4": source_globals["cooldown_e4"],
        "near_miss_25d3": source_globals["near_miss_25d3"],
        "voice_25d6": source_globals["voice_25d6"],
        "combo_flag_9aa6": source_globals["combo_flag_9aa6"],
        "timer_25d5": source_globals["timer_25d5"],
        "combo_9aa8": source_globals["combo_9aa8"],
        "combo_9aac": source_globals["combo_9aac"],
        "rng_3b04": source_globals["rng_3b04"],
        "selection_counter_4c98": source_globals["selection_counter_4c98"],
    })
    payload = struct.pack("<8sI", b"CTTPH001", frame["counter"]) + packed_globals + actors
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_bytes(payload)
    print(json.dumps({
        "status": "PASS",
        "tick": tick,
        "source_counter": frame["counter"],
        "bytes": len(payload),
        "sha256": hashlib.sha256(payload).hexdigest(),
        "output": str(output_path),
        "scope": "Unmodified original-ROM per-VBlank actor/global test checkpoint; not runtime data or a parity claim.",
    }))


if __name__ == "__main__":
    main()
