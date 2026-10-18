"""Pack an observed VBlank actor/global snapshot for an offline native traffic replay."""
import hashlib
import json
import pathlib
import struct
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from pack_native_traffic_event_fixture import actor_table, globals_pack


def sha(path: pathlib.Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main() -> None:
    if len(sys.argv) != 5:
        raise SystemExit("usage: pack_native_traffic_phase_checkpoint.py TRACE ROADS OUTPUT COUNTER")
    trace_path, roads_path, output_path = map(pathlib.Path, sys.argv[1:4])
    counter = int(sys.argv[4], 0)
    traces = json.loads(trace_path.read_text(encoding="utf-8"))
    rows = [row for row in traces if row.get("counter") == counter]
    if len(rows) != 1:
        raise SystemExit(f"expected one source row at counter {counter}, found {len(rows)}")
    row = rows[0]
    if "traffic_actor_records" not in row or "traffic_globals" not in row:
        raise SystemExit("source row has no captured actor pool or globals")
    raw = bytes.fromhex(row["traffic_actor_records"]["raw"])
    if len(raw) != 50 * 92:
        raise SystemExit(f"unexpected actor-pool size: {len(raw)}")
    roads = json.loads(roads_path.read_text(encoding="utf-8"))
    descriptors = {int(p["descriptor_address"], 16): int(p["index"]) for p in roads["paths"]}
    body = globals_pack(row["traffic_globals"]) + actor_table(raw, descriptors)
    output_path.write_bytes(struct.pack("<8sI", b"CTTPH001", counter) + body)
    report = {
        "status": "PASS_PACKED_SOURCE_VBLANK_PHASE_CHECKPOINT",
        "source_counter": counter,
        "source_tick": row["tick"],
        "source_trace": str(trace_path),
        "source_trace_sha256": sha(trace_path),
        "roads_sha256": sha(roads_path),
        "voice_25d6": row["traffic_globals"]["voice_25d6"],
        "rng_3b04": row["traffic_globals"]["rng_3b04"],
        "actor_pool_slots": 50,
        "output": str(output_path),
        "output_sha256": sha(output_path),
        "scope": "Exact captured source VBlank actor/global checkpoint for offline replay phase alignment. This is not a routine-entry trace or evidence of complete native gameplay.",
    }
    output_path.with_suffix(output_path.suffix + ".json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report))


if __name__ == "__main__":
    main()
