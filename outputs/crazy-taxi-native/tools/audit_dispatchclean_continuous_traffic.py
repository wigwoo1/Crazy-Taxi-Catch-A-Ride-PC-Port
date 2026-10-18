#!/usr/bin/env python3
"""Run native driver+traffic continuously against clean source boundaries.

The ROM is only read. A clean fixed-input trace supplies the source schedule
and checkpoints; native state is carried between every tested boundary.
"""
from __future__ import annotations
import argparse, gzip, hashlib, json, struct, subprocess
from pathlib import Path

ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
STRIDE = 0x5C
FIELDS = ("flags", "type", "countdown", "x", "y", "z", "heading", "route",
          "collision_timer", "impulse_x", "impulse_z", "offset_x", "offset_z",
          "path_distance", "path_segment", "path_x", "path_z", "path_heading")

def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()

def u32(raw: bytes, off: int) -> int:
    return struct.unpack_from("<I", raw, off)[0]

def s32(raw: bytes, off: int) -> int:
    return struct.unpack_from("<i", raw, off)[0]

def read_actor(raw: bytes, slot: int, descriptors: dict[int, int]) -> dict[str, int]:
    p = slot * STRIDE
    descriptor = u32(raw, p + 0x24)
    if descriptor and descriptor not in descriptors:
        raise ValueError(f"unknown actor{slot} route descriptor {descriptor:#x}")
    return {
        "flags": u32(raw, p), "type": u32(raw, p + 4),
        "countdown": s32(raw, p + 0x10), "x": s32(raw, p + 0x14),
        "y": s32(raw, p + 0x18), "z": s32(raw, p + 0x1C),
        "heading": s32(raw, p + 0x20), "route": descriptors.get(descriptor, 0),
        "path_distance": s32(raw, p + 0x28), "path_segment": u32(raw, p + 0x2C),
        "path_x": s32(raw, p + 0x34), "path_z": s32(raw, p + 0x38),
        "path_heading": struct.unpack_from("<H", raw, p + 0x3C)[0],
        "impulse_x": s32(raw, p + 0x40), "impulse_z": s32(raw, p + 0x48),
        "offset_x": s32(raw, p + 0x4C), "offset_z": s32(raw, p + 0x54),
        "collision_timer": s32(raw, p + 0x58),
    }

def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--project-root", type=Path, required=True,
                    help="Crazy Taxi Native Port folder containing outputs/crazy-taxi-native")
    ap.add_argument("--events", type=int, default=100)
    ap.add_argument("--output", type=Path, required=True)
    a = ap.parse_args()
    root = a.project_root / "outputs/crazy-taxi-native"
    report_dir = root / "reports/original-core-reference"
    clean_name = "dispatchclean1200"
    hook_name = "callbackdispatchminimal1200"
    clean_doc = json.loads((report_dir / f"{clean_name}.json").read_text(encoding="utf-8"))
    hook_doc = json.loads((report_dir / f"{hook_name}.json").read_text(encoding="utf-8"))
    hook_route = json.loads((report_dir / f"{hook_name}.route-follow.json").read_text(encoding="utf-8"))
    clean_path = report_dir / f"{clean_name}.actor-trace.json.gz"
    event_path = report_dir / f"{hook_name}.traffic-dispatch-entry-trace.json"
    event_doc = json.loads(event_path.read_text(encoding="utf-8"))
    passenger_boundary_doc = json.loads((report_dir / "route31rngboundary-analysis.json").read_text(encoding="utf-8"))
    passenger_capture = json.loads((report_dir / "dispatchcleanpassengers181.json").read_text(encoding="utf-8"))
    passenger_rows = json.loads((report_dir / "dispatchcleanpassengers181.actor-trace.json").read_text(encoding="utf-8"))
    passenger_ram = {tick: (report_dir / f"dispatchcleanpassengers181.ewram-vblank-{tick:05d}.bin").read_bytes()
                     for tick in (150, 180)}
    late_capture = json.loads((report_dir / "dispatchcleanunhooked378.json").read_text(encoding="utf-8"))
    late_rows = json.loads((report_dir / "dispatchcleanunhooked378.actor-trace.json").read_text(encoding="utf-8"))
    late_ram = {tick: (report_dir / f"dispatchcleanunhooked378.ewram-vblank-{tick:05d}.bin").read_bytes()
                for tick in (376, 377)}
    rng_capture = json.loads((report_dir / "dispatchcleanrng420.json").read_text(encoding="utf-8"))
    rng_trace = json.loads((report_dir / "dispatchcleanrng420.rng-callers.json").read_text(encoding="utf-8"))
    rng_rows = json.loads((report_dir / "dispatchcleanrng420.actor-trace.json").read_text(encoding="utf-8"))
    rng_events = rng_trace["call_events"]
    with gzip.open(clean_path, "rt", encoding="utf-8") as f:
        rows = json.load(f)
    events = event_doc["call_events"]
    ram = (report_dir / "boot4200.ewram").read_bytes()
    iwram = (report_dir / "boot4200.03000000.bin").read_bytes()
    state = (report_dir / "boot4200.state").read_bytes()
    roads_doc = json.loads((root / "assets/traffic/city1-roads.json").read_text(encoding="utf-8"))
    descriptors = {int(p["descriptor_address"], 16): int(p["index"])
                   for p in roads_doc["paths"]}
    if clean_doc.get("rom_sha256") != ROM_SHA or hook_doc.get("rom_sha256") != ROM_SHA:
        raise SystemExit("source ROM identity mismatch")
    if clean_doc.get("core_sha256") != hook_doc.get("core_sha256"):
        raise SystemExit("clean/hooked core mismatch")
    if sha(state) != hook_route.get("state_sha256"):
        raise SystemExit("fixed starting state mismatch")
    if (passenger_capture.get("status") != "PASS" or passenger_capture.get("frames") != 181
            or passenger_capture.get("rom_sha256") != ROM_SHA
            or passenger_capture.get("core_sha256") != clean_doc.get("core_sha256")
            or passenger_capture.get("instrumentation") is not None or len(passenger_rows) != 181
            or any(len(raw) != 0x40000 for raw in passenger_ram.values())):
        raise SystemExit("clean passenger-owner boundary capture identity/shape mismatch")
    passenger_keys = ("tick", "input_keys", "keyinput", "heading", "speed", "counter", "previous",
                      "frame_delta", "delta", "intro", "position", "angles", "orientation", "gravity",
                      "airborne", "raw_input_history", "driver_owners", "traffic_frame_context")
    passenger_trace_matches_clean = all(
        all(passenger_rows[i].get(k) == rows[i].get(k) for k in passenger_keys)
        for i in range(len(passenger_rows)))
    if not passenger_trace_matches_clean:
        raise SystemExit("clean passenger boundary capture does not match accepted input/driver schedule")
    if (late_capture.get("status") != "PASS" or late_capture.get("frames") != 378
            or late_capture.get("rom_sha256") != ROM_SHA
            or late_capture.get("core_sha256") != clean_doc.get("core_sha256")
            or late_capture.get("instrumentation") is not None or len(late_rows) != 378
            or any(len(raw) != 0x40000 for raw in late_ram.values())):
        raise SystemExit("clean tick377 source capture identity/shape mismatch")
    if (rng_capture.get("status") != "PASS" or rng_capture.get("frames") != 420
            or rng_capture.get("rom_sha256") != ROM_SHA
            or rng_capture.get("core_sha256") != clean_doc.get("core_sha256")
            or rng_trace.get("instrumentation", {}).get("source_rom_sha256") != ROM_SHA
            or len(rng_rows) != 420):
        raise SystemExit("tick377 RNG callsite trace identity/shape mismatch")
    late_trace_matches_clean = all(
        all(late_rows[i].get(k) == rows[i].get(k) for k in passenger_keys)
        for i in range(len(late_rows)))
    if not late_trace_matches_clean:
        raise SystemExit("clean tick377 capture does not match accepted input/driver schedule")
    rng_trace_matches_clean = all(
        all(rng_rows[i].get(k) == rows[i].get(k) for k in passenger_keys)
        for i in range(len(rng_rows)))
    if not rng_trace_matches_clean:
        raise SystemExit("RNG callsite trace does not match accepted input/driver schedule")
    if len(rows) != 1200 or len(events) != 236 or not 1 <= a.events <= len(events):
        raise SystemExit("expected reviewed 1,200-frame clean capture and 236 dispatcher events")
    if len(ram) != 0x40000 or len(iwram) != 0x8000 or len(ram[0xF4:0xF4 + 50 * STRIDE]) != 50 * STRIDE:
        raise SystemExit("saved source RAM inputs are malformed")

    # The fixture begins at the untouched saved state. V4 rows then apply the
    # exact clean-capture input and observed 4/5/6-tick timing at every event.
    schedule_path = report_dir / "dispatchclean-native-continuous.schedule.txt"
    actor_path = report_dir / "dispatchclean-native-continuous.actors.bin"
    config_path = report_dir / "dispatchclean-native-continuous.cfg"
    chosen = events[:a.events]
    schedule = [f"SOURCE_DRIVING_REFERENCE_V4 {len(chosen)}"]
    schedule_map = []
    elapsed = 0
    for index, event in enumerate(chosen):
        tick = int(event["observed_after_core_tick"])
        row = rows[tick]
        delta = int(event["frame_delta"])
        if int(row["counter"]) != int(event["source_counter"]) or int(row["delta"]) != delta:
            raise SystemExit(f"source schedule mismatch at clean trace row{tick}")
        elapsed += delta
        owners = row.get("driver_owners", {})
        schedule.append(" ".join(str(x) for x in (
            int(row["keyinput"]), delta, int(row["delta"]), delta,
            *row["position"], int(row["heading"]), int(row["speed"]), elapsed,
            int(owners.get("flag_1697", 0)), int(owners.get("flag_153c", 0)),
            int(owners.get("freeze_2620", 0)), int(owners.get("freeze_177b", 0)), 0)))
        schedule_map.append({"step": index, "source_tick": tick,
                             "source_counter": int(event["source_counter"]), "delta": delta})
    schedule_bytes = ("\n".join(schedule) + "\n").encode("ascii")
    schedule_path.write_bytes(schedule_bytes)

    raw_pool = ram[0xF4:0xF4 + 50 * STRIDE]
    actor_rows = []
    for slot in range(50):
        actor = read_actor(raw_pool, slot, descriptors)
        actor_rows.append([actor[k] for k in (
            "flags", "type", "countdown", "x", "y", "z", "heading", "route",
            "path_distance", "path_segment", "path_x", "path_z", "path_heading",
            "impulse_x", "impulse_z", "offset_x", "offset_z", "collision_timer")])
    actor_bytes = b"".join(struct.pack("<18i", *v) for v in actor_rows)
    actor_path.write_bytes(actor_bytes)
    setting = ram[0x1EB3]
    config = (setting, (0), ram[0x25D5], u32(iwram, 0x4C98),
              struct.unpack_from("<h", ram, 0xE4)[0], ram[0x25D3], ram[0x25D6],
              ram[0x9AA6], s32(ram, 0x9AA8), s32(ram, 0x9AAC),
              u32(ram, 0x3B04), ram[0x7602], ram[0x1694])
    # The active slot count is the exact recovered density-table selection.
    density = (5, 11, 17, 22, 27, 33, 39, 44, 50, 28, 26, 24, 22, 20, 18, 16)[setting >> 4]
    config = (setting, density, *config[2:])
    config_path.write_text(" ".join(str(x) for x in config) + "\n", encoding="ascii")

    binary = root / "bin/continuous-driver-traffic-probe.exe"
    log_path = report_dir / "dispatchclean-native-continuous.native.log"
    proc = subprocess.run([str(binary), str(actor_path), str(config_path), str(schedule_path)],
                          cwd=a.project_root, capture_output=True, text=True, timeout=180)
    log_path.write_text(proc.stdout, encoding="utf-8")
    if proc.stderr:
        (report_dir / "dispatchclean-native-continuous.native.stderr.txt").write_text(proc.stderr, encoding="utf-8")

    native_frames: dict[int, dict] = {}
    native_passengers: dict[int, dict] = {}
    native_passengers2: dict[int, dict] = {}
    native_meta: dict[int, dict] = {}
    native_camera_grid: dict[int, dict] = {}
    for line in proc.stdout.splitlines():
        p = line.split()
        if not p:
            continue
        if p[0] == "M" and len(p) == 8:
            v = list(map(int, p[1:])); native_meta[v[0]] = {
                "candidate": v[1], "distance_squared": v[2], "rng": v[3],
                "voice": v[4], "timer": v[5], "owner_index": v[6]}
        elif p[0] == "C" and len(p) == 11:
            v = list(map(int, p[1:])); native_camera_grid[v[0]] = {
                "position": v[1:4], "yaw": v[4], "pitch": v[5],
                "grid_result": v[6], "cell_index": v[7],
                "route": v[8], "phase": v[9]}
        elif p[0] == "P" and len(p) == 4:
            native_passengers[int(p[1])] = {"rng": int(p[2]), "bytes": p[3]}
        elif p[0] == "Q" and len(p) == 4:
            native_passengers2[int(p[1])] = {"rng": int(p[2]), "bytes": p[3]}
        elif p[0] == "F" and len(p) == 21:
            v = list(map(int, p[1:]))
            native_frames[v[0]] = {"tick": v[1], "delta": v[2], "frame_delta": v[3],
                "spawned": v[4], "count": v[5], "timer": v[6], "counter": v[7],
                "cooldown": v[8], "near": v[9], "voice": v[10], "combo_flag": v[11],
                "combo_whole": v[12], "combo_fraction": v[13], "rng": v[14],
                "position": v[15:18], "heading": v[18], "speed": v[19], "actors": {}}
        elif p[0] == "A" and len(p) == 21:
            v = list(map(int, p[1:])); step, slot = v[:2]
            frame = native_frames.get(step)
            if frame is None:
                raise SystemExit(f"actor output precedes frame summary for update{step}")
            frame["actors"][slot] = dict(zip(FIELDS, v[2:]))

    comparisons = []
    for mapping in schedule_map:
        step = mapping["step"]
        native = native_frames.get(step)
        if native is None:
            continue
        source = rows[mapping["source_tick"]]
        driver_bad = {}
        expected_vehicle = {"position": source["position"], "heading": source["heading"], "speed": source["speed"]}
        for key, expected in expected_vehicle.items():
            actual = native[key]
            if actual != expected:
                driver_bad[key] = {"native": actual, "source": expected}
        source_g = source["traffic_globals"]
        global_map = {"timer": "timer_25d5", "counter": "selection_counter_4c98",
            "cooldown": "cooldown_e4", "near": "near_miss_25d3", "voice": "voice_25d6",
            "combo_flag": "combo_flag_9aa6", "combo_whole": "combo_9aa8",
            "combo_fraction": "combo_9aac", "rng": "rng_3b04"}
        global_bad = {k: {"native": native[k], "source": source_g[v]}
                      for k, v in global_map.items() if native[k] != source_g[v]}
        raw = bytes.fromhex(source["traffic_actor_records"]["raw"])
        actor_bad = []
        for slot in range(50):
            expected = read_actor(raw, slot, descriptors)
            got = native["actors"].get(slot)
            if got is None:
                actor_bad.append({"slot": slot, "missing": True}); continue
            diff = {k: {"native": got[k], "source": expected[k]}
                    for k in FIELDS if got[k] != expected[k]}
            if diff:
                actor_bad.append({"slot": slot, "fields": diff})
        comparisons.append({"step": step, "source_tick": mapping["source_tick"],
            "source_counter": mapping["source_counter"], "delta": mapping["delta"],
            "driver_mismatches": driver_bad, "traffic_global_mismatches": global_bad,
            "actor_match_count": 50 - len(actor_bad), "actor_mismatches": actor_bad})

    camera_grid_comparisons = []
    for mapping in schedule_map:
        step = mapping["step"]
        native = native_camera_grid.get(step)
        if native is None:
            continue
        source = rows[mapping["source_tick"]].get("traffic_camera", {})
        expected_position = source.get("position")
        camera_grid_comparisons.append({
            "step": step, "source_tick": mapping["source_tick"],
            "native_camera_position": native["position"],
            "source_camera_position": expected_position,
            "camera_position_matches": native["position"] == expected_position,
            "native_camera_yaw_pitch": [native["yaw"], native["pitch"]],
            "native_grid_selection": {k: native[k] for k in
                ("grid_result", "cell_index", "route", "phase")},
        })

    complete = proc.returncode == 0 and len(native_frames) == a.events and all(
        len(x["actors"]) == 50 for x in native_frames.values())
    exact_steps = [x["step"] for x in comparisons if not x["driver_mismatches"]
                   and not x["traffic_global_mismatches"] and not x["actor_mismatches"]]
    source_idle = passenger_boundary_doc["vblank150_rng_cause"]["source_state"]
    native_idle = native_passengers.get(30)
    idle_boundary = {
        "source_owner": source_idle["passenger_owner_address"],
        "source_rng_before": source_idle["rng_before"],
        "source_rng_after": source_idle["rng_after"],
        "native_step": 30,
        "native_rng_after": native_idle["rng"] if native_idle else None,
        "native_passenger_bytes_match_source_after": bool(native_idle and
            native_idle["bytes"] == source_idle["state_after_44_bytes"]),
        "native_rng_matches_source_after": bool(native_idle and
            native_idle["rng"] == source_idle["rng_after"]),
    }
    passenger_boundaries = {}
    for step, tick, owner, output in ((30, 150, 0x7004, native_passengers),
                                      (36, 180, 0x6ed0, native_passengers2)):
        expected_bytes = passenger_ram[tick][owner:owner + 44].hex()
        actual = output.get(step)
        passenger_boundaries[str(tick)] = {
            "owner": hex(0x02000000 + owner), "native_step": step,
            "source_passenger_bytes": expected_bytes,
            "native_passenger_bytes_match_source": bool(actual and actual["bytes"] == expected_bytes),
            "source_rng_after_complete_frame": u32(passenger_ram[tick], 0x3b04),
            "native_rng_after_owner_before_traffic": actual["rng"] if actual else None,
        }
    second_passenger_matches = passenger_boundaries["180"]["native_passenger_bytes_match_source"]
    first_passenger_matches = passenger_boundaries["150"]["native_passenger_bytes_match_source"]
    voice_step, voice_tick = 77, 377
    voice_ram = late_ram[voice_tick]
    owner_pointer = u32(voice_ram, 0x168c)
    owner_delta = owner_pointer - 0x02005f2c
    owner_index = owner_delta // 44 if owner_delta >= 0 and owner_delta % 44 == 0 else None
    source_voice_event = next((e for e in rng_events if e.get("tick") == voice_tick
        and e.get("call_site") == "0x800b24e"), None)
    native_voice_meta = native_meta.get(voice_step)
    native_voice_customer = native_passengers.get(voice_step)
    voice_boundary = {
        "source_capture_uninstrumented": late_capture.get("instrumentation") is None,
        "late_input_driver_rows_match_clean": late_trace_matches_clean,
        "rng_hook_input_driver_rows_match_clean": rng_trace_matches_clean,
        "source_selected_customer_pointer": hex(owner_pointer),
        "source_selected_customer_index": owner_index,
        "source_selected_customer_flags": voice_ram[0x7004],
        "source_voice": voice_ram[0x25d6],
        "source_timer": s32(voice_ram, 0x2614),
        "source_rng_after_event": u32(voice_ram, 0x3b04),
        "source_dynamic_rng_event": source_voice_event,
        "native_candidate_index": native_voice_meta["candidate"] if native_voice_meta else None,
        "native_rng_after_event": native_voice_meta["rng"] if native_voice_meta else None,
        "native_voice": native_voice_meta["voice"] if native_voice_meta else None,
        "native_timer": native_voice_meta["timer"] if native_voice_meta else None,
        "native_owner_index": native_voice_meta["owner_index"] if native_voice_meta else None,
        "native_customer_bytes_match_source": bool(native_voice_customer and
            native_voice_customer["bytes"] == voice_ram[0x7004:0x7004 + 44].hex()),
        "owner_voice_rng_timer_match": bool(native_voice_meta and source_voice_event and
            owner_index == 98 and native_voice_meta["candidate"] == owner_index and
            native_voice_meta["owner_index"] == owner_index and
            native_voice_meta["rng"] == u32(voice_ram, 0x3b04) and
            native_voice_meta["voice"] == voice_ram[0x25d6] and
            native_voice_meta["timer"] == s32(voice_ram, 0x2614) and
            source_voice_event.get("rng_before") == u32(late_ram[376], 0x3b04)),
    }
    report = {
        "status": "PASS_DIAGNOSTIC_EXECUTION" if complete else "INCOMPLETE_DIAGNOSTIC_EXECUTION",
        "source_rom_sha256": ROM_SHA, "source_core_sha256": clean_doc["core_sha256"],
        "starting_state_sha256": sha(state), "source_trace_sha256": sha(clean_path.read_bytes()),
        "hook_schedule_sha256": sha(event_path.read_bytes()),
        "input_schedule_sha256": sha((root / "work/dispatcher-1200-inputs.json").read_bytes()),
        "tested_dispatch_updates": a.events, "native_updates_completed": len(native_frames),
        "process_return_code": proc.returncode, "source_update_delta_histogram": {
            str(d): sum(m["delta"] == d for m in schedule_map) for d in (4,5,6)},
        "exact_all_state_match_updates": exact_steps,
        "camera_grid_diagnostics": camera_grid_comparisons,
        "first_camera_position_mismatch": next((x for x in camera_grid_comparisons
            if not x["camera_position_matches"]), None),
        "passenger_idle_rng_boundary": idle_boundary,
        "clean_passenger_capture_matches_clean_input_driver_schedule": passenger_trace_matches_clean,
        "passenger_owner_boundaries": passenger_boundaries,
        "clean_tick377_capture_matches_clean_input_driver_schedule": late_trace_matches_clean,
        "passenger_search_voice_boundary_tick377": voice_boundary,
        "first_driver_mismatch": next((x for x in comparisons if x["driver_mismatches"]), None),
        "first_traffic_global_mismatch": next((x for x in comparisons if x["traffic_global_mismatches"]), None),
        "first_actor_pool_mismatch": next((x for x in comparisons if x["actor_mismatches"]), None),
        "comparisons": comparisons,
        "native_log": log_path.name,
        "scope": "Continuous native driver, full128-record passenger search plus recovered idle/boarding/frame routines and the flag16 voice/timer/RNG branch, then the full recovered native 50-record traffic scene at each clean-derived dispatcher boundary. State carries forward without per-boundary reseeding. Source tick150/180/377 snapshots come from separate uninstrumented captures whose ROM/core and input/driver rows are checked against the accepted clean trace. This remains a controlled source replay, not a complete source-callsite scheduler or playable trip/fare loop.",
        "next_dependency": "Resolve the first tick414 source/native divergence: determine the original owner that changes voice 25D6 and the traffic slot8 spawn/counter phase using clean consecutive VBlank snapshots; then continue source-driven pickup/countdown/delivery/fare integration.",
    }
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({k: report[k] for k in ("status", "tested_dispatch_updates",
        "native_updates_completed", "source_update_delta_histogram", "exact_all_state_match_updates",
        "first_driver_mismatch", "passenger_idle_rng_boundary", "first_traffic_global_mismatch", "first_actor_pool_mismatch",
        "first_camera_position_mismatch",
        "scope")}, indent=2))
    return 0 if complete else 2

if __name__ == "__main__":
    raise SystemExit(main())
