"""Extract schedule-aligned, ROM-captured traffic state for native differential work.

This packages source post-VBlank snapshots; it does not execute the game or
claim that each snapshot is an exact native traffic-dispatch call boundary.
"""
import hashlib
import json
import pathlib
import sys


def sha(path):
    h = hashlib.sha256()
    with pathlib.Path(path).open("rb") as f:
        for chunk in iter(lambda: f.read(1024 * 1024), b""):
            h.update(chunk)
    return h.hexdigest()


def array_items(path):
    text = pathlib.Path(path).read_text(encoding="utf-8")
    decoder = json.JSONDecoder()
    pos = 0
    while pos < len(text) and text[pos].isspace():
        pos += 1
    assert text[pos] == "[", "trace root must be a JSON array"
    pos += 1
    index = 0
    while True:
        while pos < len(text) and (text[pos].isspace() or text[pos] == ","):
            pos += 1
        if pos >= len(text) or text[pos] == "]":
            return
        item, pos = decoder.raw_decode(text, pos)
        yield index, item
        index += 1


def main():
    if len(sys.argv) != 5:
        raise SystemExit("usage: extract_traffic_actor_replay_fixture.py TRACE ANALYSIS SCHEDULE OUTPUT")
    trace_path, analysis_path, schedule_path, output_path = map(pathlib.Path, sys.argv[1:])
    analysis = json.loads(analysis_path.read_text(encoding="utf-8"))
    lines = schedule_path.read_text(encoding="utf-8").splitlines()
    assert lines and lines[0].startswith("SOURCE_DRIVING_REFERENCE_V3 ")
    expected = int(lines[0].split()[1])
    schedule = [[int(value) for value in line.split()] for line in lines[1:] if line.strip()]
    indices = analysis["schedule_trace_indices"]
    assert len(schedule) == expected == len(indices)
    assert analysis["schedule_rows"] == expected and analysis["schedule_actor_pool_alignment_pass"]
    selected = dict(zip(indices, enumerate(schedule)))
    assert len(selected) == expected and indices == sorted(indices)

    header = {
        "format": "SOURCE_TRAFFIC_REPLAY_FIXTURE_V2",
        "rom_sha256": analysis["rom_sha256"],
        "trace_sha256": sha(trace_path),
        "analysis_sha256": sha(analysis_path),
        "schedule_sha256": sha(schedule_path),
        "schedule_rows": expected,
        "actor_count": 50,
        "actor_stride": 92,
        "actor_bytes": 4600,
        "player_actor_bytes": 0xEA0,
        "camera_raw_bytes": 64,
        "scope": "Selected original-ROM post-VBlank actor/global/camera snapshots aligned to the observed V3 input schedule. These are source checkpoints, not proven routine-entry states or proof of native traffic dispatch parity.",
    }
    output_path.parent.mkdir(parents=True, exist_ok=True)
    written = context_rows = 0
    first_source_counter = None
    with output_path.open("w", encoding="utf-8", newline="\n") as out:
        out.write(json.dumps({"header": header}, separators=(",", ":")) + "\n")
        for trace_index, frame in array_items(trace_path):
            chosen = selected.get(trace_index)
            if chosen is None:
                continue
            row_index, row = chosen
            owners = frame.get("driver_owners", {})
            assert (frame["keyinput"], frame["delta"], frame["frame_delta"], *frame["position"], frame["heading"], frame["speed"]) == (row[0], row[2], row[3], *row[4:9]), (row_index, trace_index)
            assert (owners.get("flag_1697"), owners.get("flag_153c"), owners.get("freeze_2620"), owners.get("freeze_177b")) == tuple(row[10:14]), (row_index, trace_index)
            if first_source_counter is None:
                first_source_counter = frame["counter"]
            assert frame["counter"] - first_source_counter == row[9] - schedule[0][9], (row_index, trace_index, frame["counter"], row[9])
            records = frame["traffic_actor_records"]
            assert records["address"] == "0x020000F4" and records["stride"] == 92 and records["count"] == 50
            assert len(records["raw"]) == header["actor_bytes"] * 2
            assert frame.get("traffic_globals") is not None and frame.get("traffic_camera", {}).get("position") is not None
            context = frame.get("traffic_frame_context")
            player_state = frame.get("traffic_player_state")
            camera_raw = frame.get("traffic_camera", {}).get("raw_40")
            assert player_state is not None and len(player_state.get("actor", "")) == header["player_actor_bytes"] * 2, (row_index, trace_index, "missing packed player actor")
            assert camera_raw is not None and len(camera_raw) == header["camera_raw_bytes"] * 2, (row_index, trace_index, "missing raw camera vector")
            context_rows += context is not None
            entry = {
                "schedule_row": row_index,
                "trace_index": trace_index,
                "tick": frame["tick"],
                "counter": frame["counter"],
                "schedule": row,
                "source": {
                    "position": frame["position"],
                    "heading": frame["heading"],
                    "speed": frame["speed"],
                    "raw_input_history": frame.get("raw_input_history"),
                    "angles": frame.get("angles"),
                    "orientation": frame.get("orientation"),
                    "gravity": frame.get("gravity"),
                    "airborne": frame.get("airborne"),
                    "driver_owners": owners,
                    "frame_context": context,
                    "camera": {**frame["traffic_camera"], "raw_40": camera_raw},
                    "player_state": player_state,
                    "globals": frame["traffic_globals"],
                    "actors_raw_hex": records["raw"],
                },
            }
            out.write(json.dumps(entry, separators=(",", ":")) + "\n")
            written += 1
    assert written == expected, (written, expected)
    print(json.dumps({"status": "PASS", "records": written, "frame_context_records": context_rows, "fixture_bytes": output_path.stat().st_size, "fixture_sha256": sha(output_path), "output": str(output_path), "scope": header["scope"]}))


if __name__ == "__main__":
    main()
