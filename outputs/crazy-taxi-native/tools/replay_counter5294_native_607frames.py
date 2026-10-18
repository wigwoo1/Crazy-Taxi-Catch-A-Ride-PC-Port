#!/usr/bin/env python3
"""Stateful native-vs-source replay through the next clean counter5294 dispatch checkpoint."""
from __future__ import annotations

import hashlib
import json
import struct
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REPORTS = ROOT / "reports/original-core-reference"
ROM = Path(r"Crazy Taxi - Catch a Ride (USA).gba")
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
SOURCE = REPORTS / "r5294input600.actor-trace.json"
CAPTURE = REPORTS / "r5294input600.json"
TAIL_SOURCE = REPORTS / "r5294input600tail.actor-trace.json"
TAIL_CAPTURE = REPORTS / "r5294input600tail.json"
TAIL_KEYS = REPORTS / "r5294input600-tail-key.json"
TAIL_MANIFEST = REPORTS / "r5294input600tail-resume-manifest.json"
TAIL_PARENT_STATE = REPORTS / "r5294input600.state"
TAIL_OUTPUT_STATE = REPORTS / "r5294input600tail.state"
TAIL2_SOURCE = REPORTS / "r5294input600tail2.actor-trace.json"
TAIL2_CAPTURE = REPORTS / "r5294input600tail2.json"
TAIL2_KEYS = REPORTS / "r5294input600tail2-keys.json"
TAIL2_MANIFEST = REPORTS / "r5294input600tail2-resume-manifest.json"
TAIL2_PARENT_STATE = REPORTS / "r5294input600tail.state"
TAIL2_OUTPUT_STATE = REPORTS / "r5294input600tail2.state"
TAIL3_SOURCE = REPORTS / "r5294input600tail3.actor-trace.json"
TAIL3_CAPTURE = REPORTS / "r5294input600tail3.json"
TAIL3_KEYS = REPORTS / "r5294input600tail3-keys.json"
TAIL3_MANIFEST = REPORTS / "r5294input600tail3-resume-manifest.json"
TAIL3_PARENT_STATE = REPORTS / "r5294input600tail2.state"
TAIL3_OUTPUT_STATE = REPORTS / "r5294input600tail3.state"
PRE_RAM = REPORTS / "counter5294-source-pre.ewram"
PRE_IWRAM = REPORTS / "r5294base1100.03000000.bin"
ACTOR_FILE = REPORTS / "counter5294-stateful-601-input.actors.bin"
CFG_FILE = REPORTS / "counter5294-stateful-601-input.cfg"
BIN = ROOT / "bin/counter5294-stateful-traffic-replay.exe"
ROADS = ROOT / "assets/traffic/city1-roads.json"
OUTPUT = REPORTS / "counter5294-native-stateful-607frame-audit.json"
AUDIO_CLEAN_REPORT = REPORTS / "r5294audioownerfix.json"
AUDIO_CLEAN_TRACE = REPORTS / "r5294audioownerfix.actor-trace.json"
AUDIO_BUSY_REPORT = REPORTS / "r5294audiobusy600.json"
AUDIO_BUSY_TRACE = REPORTS / "r5294audiobusy600.audio-busy-return-trace.json"
AUDIO_PRE_RAM = REPORTS / "r5294audioownerfix.ewram-vblank-00052.bin"
AUDIO_POST_RAM = REPORTS / "r5294audioownerfix.ewram-vblank-00053.bin"
VOICE_START_TRACE = REPORTS / "r5294voicecontextstart600.audio-context-start-trace.json"
VOICE_START_REPORT = REPORTS / "r5294voicecontextstart600.json"
VOICE_START_ROWS = REPORTS / "r5294voicecontextstart600.actor-trace.json"
VOICE_START_PRE_RAM = REPORTS / "r5294voicesnap600.ewram-vblank-00137.bin"
VOICE_START_POST_RAM = REPORTS / "r5294voicesnap600.ewram-vblank-00138.bin"
VOICE_CLEAN_PRE_RAM = REPORTS / "r5294voicecleanup600.ewram-vblank-00202.bin"
VOICE_CLEAN_POST_RAM = REPORTS / "r5294voicecleanup600.ewram-vblank-00203.bin"
VOICE_FINAL_START_TRACE = REPORTS / "r5294voicecontextstart600.audio-context-start-trace.json"
VOICE_FINAL_PRE_RAM = REPORTS / "r5294voicefinalstart600.ewram-vblank-00587.bin"
VOICE_FINAL_POST_RAM = REPORTS / "r5294voicefinalstart600.ewram-vblank-00588.bin"
RNG_FINAL_TRACE = REPORTS / "r5294rng29086filtered600.rng-callers.json"
RNG_FINAL_REPORT = REPORTS / "r5294rng29086filtered600.json"
RNG_FINAL_ROWS = REPORTS / "r5294rng29086filtered600.actor-trace.json"
STRIDE = 0x5C
KEY_BIT = {0: 1, 6: 5, 7: 4, 8: 0}
ACTOR_FIELDS = ("flags", "type", "countdown", "x", "y", "z", "heading", "route",
                "collision_timer", "impulse_x", "impulse_z", "offset_x", "offset_z",
                "path_distance", "path_segment", "path_x", "path_z", "path_heading")
EXPECTED_FIELDS = ("flags", "type", "countdown", "x", "y", "z", "heading", "route",
                   "path_distance", "path_segment", "path_x", "path_z", "path_heading",
                   "impulse_x", "impulse_z", "offset_x", "offset_z", "collision_timer")
WRITE_FIELDS = (("flags", 0x00), ("type", 0x04), ("countdown", 0x10),
                ("x", 0x14), ("y", 0x18), ("z", 0x1C), ("heading", 0x20),
                ("collision_timer", 0x58), ("path_distance", 0x28),
                ("path_segment", 0x2C), ("path_x", 0x34), ("path_z", 0x38),
                ("impulse_x", 0x40), ("impulse_z", 0x48),
                ("offset_x", 0x4C), ("offset_z", 0x54))


def sha(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def u32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<I", data, offset)[0]


def s32(data: bytes, offset: int) -> int:
    return struct.unpack_from("<i", data, offset)[0]


def s8(value: int) -> int:
    return value - 256 if value & 0x80 else value


def actors_from_raw(raw: bytes, descriptor_to_route: dict[int, int]) -> list[tuple[int, ...]]:
    if len(raw) != 50 * STRIDE:
        raise ValueError("expected a complete 50-record source table")
    actors = []
    for i in range(50):
        p = i * STRIDE
        descriptor = u32(raw, p + 0x24)
        route = descriptor_to_route.get(descriptor, 0)
        if descriptor and not route:
            raise ValueError(f"unknown route descriptor at actor{i}: {descriptor:#x}")
        actors.append((u32(raw,p),u32(raw,p+4),s32(raw,p+0x10),s32(raw,p+0x14),
            s32(raw,p+0x18),s32(raw,p+0x1c),s32(raw,p+0x20),route,s32(raw,p+0x28),
            u32(raw,p+0x2c),s32(raw,p+0x34),s32(raw,p+0x38),
            struct.unpack_from("<H",raw,p+0x3c)[0],s32(raw,p+0x40),
            s32(raw,p+0x48),s32(raw,p+0x4c),s32(raw,p+0x54),s32(raw,p+0x58)))
    return actors


def rebuild_pool(pool: bytearray, actors: list[dict], route_to_descriptor: dict[int, int],
                route_to_points: dict[int, int]) -> None:
    for actor in actors:
        base = actor["index"] * STRIDE
        for field, offset in WRITE_FIELDS:
            struct.pack_into("<I", pool, base + offset, actor[field] & 0xFFFFFFFF)
        descriptor = route_to_descriptor.get(actor["route"], 0)
        struct.pack_into("<I", pool, base + 0x24, descriptor)
        point_pointer = route_to_points.get(actor["route"], 0) + actor["path_segment"] * 16
        struct.pack_into("<I", pool, base + 0x30, point_pointer if descriptor else 0)
        struct.pack_into("<H", pool, base + 0x3C, actor["path_heading"] & 0xFFFF)


def pool_mismatches(native: bytes, source: bytes) -> list[dict]:
    mismatches = []
    for slot in range(50):
        start = slot * STRIDE
        a, b = native[start:start+STRIDE], source[start:start+STRIDE]
        if a != b:
            offsets = [i for i, (x,y) in enumerate(zip(a,b)) if x != y]
            mismatches.append({"slot": slot, "byte_offsets": offsets[:20],
                               "mismatch_count": len(offsets)})
    return mismatches


def main() -> int:
    rom = ROM.read_bytes()
    capture = json.loads(CAPTURE.read_text(encoding="utf-8"))
    rows = json.loads(SOURCE.read_text(encoding="utf-8"))
    tail_capture = json.loads(TAIL_CAPTURE.read_text(encoding="utf-8"))
    tail_rows = json.loads(TAIL_SOURCE.read_text(encoding="utf-8"))
    continuation = json.loads(TAIL_MANIFEST.read_text(encoding="utf-8"))
    tail2_capture = json.loads(TAIL2_CAPTURE.read_text(encoding="utf-8"))
    tail2_rows = json.loads(TAIL2_SOURCE.read_text(encoding="utf-8"))
    continuation2 = json.loads(TAIL2_MANIFEST.read_text(encoding="utf-8"))
    tail3_capture = json.loads(TAIL3_CAPTURE.read_text(encoding="utf-8"))
    tail3_rows = json.loads(TAIL3_SOURCE.read_text(encoding="utf-8"))
    continuation3 = json.loads(TAIL3_MANIFEST.read_text(encoding="utf-8"))
    pre = PRE_RAM.read_bytes()
    iwram = PRE_IWRAM.read_bytes()
    if sha(rom) != ROM_SHA or capture.get("status") != "PASS" or capture.get("rom_sha256") != ROM_SHA:
        raise SystemExit("source ROM/capture identity check failed")
    if capture.get("frames") != 600 or capture.get("instrumentation") is not None:
        raise SystemExit("reference is not the accepted uninstrumented 600-frame run")
    if len(rows) != 600 or len(pre) != 0x40000 or len(iwram) != 0x8000:
        raise SystemExit("unexpected source trace or memory checkpoint size")
    if [int(r["counter"]) for r in rows] != list(range(5294,5894)):
        raise SystemExit("source trace does not span counters 5294 through 5893")
    if tail_capture.get("status") != "PASS" or tail_capture.get("frames") != 1 or tail_capture.get("rom_sha256") != ROM_SHA or tail_capture.get("core_sha256") != capture.get("core_sha256") or tail_capture.get("instrumentation") is not None:
        raise SystemExit("tail source capture is not one clean frame from the matching ROM/core")
    if len(tail_rows) != 1 or len(json.loads(TAIL_KEYS.read_text(encoding="utf-8"))) != 1:
        raise SystemExit("tail source capture/input schedule is not exactly one frame")
    if continuation.get("status") != "PASS_SOURCE_CAPTURE_RESUMED_FROM_600FRAME_STATE" or continuation.get("rom_sha256") != ROM_SHA or continuation.get("core_sha256") != capture.get("core_sha256"):
        raise SystemExit("source continuation manifest identity check failed")
    for key, path in (("parent_capture_report_sha256", CAPTURE), ("parent_serialized_state_sha256", TAIL_PARENT_STATE),
                      ("resumed_capture_report_sha256", TAIL_CAPTURE), ("resumed_serialized_state_sha256", TAIL_OUTPUT_STATE),
                      ("input_schedule_sha256", TAIL_KEYS), ("resumed_trace_sha256", TAIL_SOURCE)):
        if continuation.get(key) != sha(path.read_bytes()):
            raise SystemExit(f"source continuation manifest hash mismatch: {key}")
    tail = tail_rows[0]
    if int(rows[-1]["counter"]) != 5893 or int(tail["counter"]) != 5894 or int(tail["previous"]) != 5893 or tail["input_keys"] != [8]:
        raise SystemExit("resumed source frame does not continue the expected counter/input boundary")
    tail = dict(tail)
    tail["tick"] = 600
    rows.append(tail)
    for cap, sample_rows, key_path, manifest, parent_cap, parent_state, output_state, trace_path, expected_frames in (
        (tail2_capture, tail2_rows, TAIL2_KEYS, continuation2, TAIL_CAPTURE, TAIL2_PARENT_STATE, TAIL2_OUTPUT_STATE, TAIL2_SOURCE, 5),
        (tail3_capture, tail3_rows, TAIL3_KEYS, continuation3, TAIL2_CAPTURE, TAIL3_PARENT_STATE, TAIL3_OUTPUT_STATE, TAIL3_SOURCE, 1)):
        if cap.get("status") != "PASS" or cap.get("frames") != expected_frames or cap.get("rom_sha256") != ROM_SHA or cap.get("core_sha256") != capture.get("core_sha256") or cap.get("instrumentation") is not None or len(sample_rows) != expected_frames:
            raise SystemExit("chained tail capture is not the expected clean source segment")
        if manifest.get("rom_sha256") != ROM_SHA or manifest.get("core_sha256") != capture.get("core_sha256"):
            raise SystemExit("chained tail manifest identity mismatch")
        if manifest.get("parent_capture_report_sha256") != sha(parent_cap.read_bytes()) or manifest.get("parent_serialized_state_sha256") != sha(parent_state.read_bytes()):
            raise SystemExit("chained tail capture did not resume from the prior serialized endpoint")
        if manifest.get("resumed_capture_report_sha256") != sha((TAIL2_CAPTURE if expected_frames == 5 else TAIL3_CAPTURE).read_bytes()):
            raise SystemExit("chained tail capture report hash mismatch")
        if manifest.get("resumed_serialized_state_sha256") != sha(output_state.read_bytes()) or manifest.get("input_schedule_sha256") != sha(key_path.read_bytes()) or manifest.get("resumed_trace_sha256") != sha(trace_path.read_bytes()):
            raise SystemExit("chained tail state/input/trace hash mismatch")
        keys = json.loads(key_path.read_text(encoding="utf-8"))
        if len(keys) != expected_frames or any(row.get("input_keys") != [8] for row in keys) or any(row.get("input_keys") != [8] for row in sample_rows):
            raise SystemExit("chained source continuation changed the held input schedule")
        start_tick = len(rows)
        rows.extend(dict(sample, tick=start_tick + i) for i, sample in enumerate(sample_rows))
    expected_counters = list(range(5294, 5901))
    if [int(row["counter"]) for row in rows] != expected_counters:
        raise SystemExit("extended clean source trace has a counter discontinuity")
    if int(rows[605]["previous"]) != 5899 or int(rows[606]["previous"]) != 5899:
        raise SystemExit("expected counter5899 dispatcher boundary and following source checkpoint were not captured")
    if sha(pre) != "11d97d1b48374a08bcf42f5bbeb2d9b8f9afb5b2316a9345cebb29db8b80c9d2":
        raise SystemExit("pre-event EWRAM checkpoint hash changed")

    road_data = json.loads(ROADS.read_text(encoding="utf-8"))
    descriptor_to_route = {int(p["descriptor_address"],16):int(p["index"])
                           for p in road_data["paths"]}
    route_to_descriptor = {int(p["index"]):int(p["descriptor_address"],16)
                           for p in road_data["paths"]}
    route_to_points = {int(p["index"]):int(p["point_address"],16)
                       for p in road_data["paths"]}
    initial_pool = pre[0xF4:0xF4 + 50*STRIDE]
    initial_fields = actors_from_raw(initial_pool, descriptor_to_route)
    ACTOR_FILE.write_bytes(b"".join(struct.pack("<18i", *actor) for actor in initial_fields))
    selected_setting = pre[0x1EB3]
    prefix_count = rom[0x084A54B0 - 0x08000000 + (selected_setting >> 4)]
    if prefix_count != 27:
        raise SystemExit(f"source selected traffic prefix changed: {prefix_count}")
    g = {"setting":selected_setting,"count":prefix_count,"timer":pre[0x25D5],
         "counter":u32(iwram,0x4C98),"cooldown":struct.unpack_from("<h",pre,0xE4)[0],
         "near":pre[0x25D3],"voice":pre[0x25D6],"combo_flag":pre[0x9AA6],
         "combo_whole":s32(pre,0x9AA8),"combo_fraction":s32(pre,0x9AAC),
         "rng":u32(pre,0x3B04),"sound":int(rows[0]["traffic_frame_context"]["sound_7602"]),
         "active":int(rows[0]["traffic_frame_context"]["trip_active_1694"]),
         "camera_x":int(rows[0]["traffic_camera"]["position"][0]),
         "camera_z":int(rows[0]["traffic_camera"]["position"][2])}
    CFG_FILE.write_text(" ".join(str(g[k]) for k in (
        "setting","count","timer","counter","cooldown","near","voice","combo_flag",
        "combo_whole","combo_fraction","rng","sound","active","camera_x","camera_z"))+"\n",
        encoding="ascii")

    audio_pre=AUDIO_PRE_RAM.read_bytes()
    if len(audio_pre)!=0x40000:
        raise SystemExit("audio owner pre-event EWRAM checkpoint has unexpected size")
    busy_trace=json.loads(AUDIO_BUSY_TRACE.read_text(encoding="utf-8"))
    busy_by_tick={int(e["observed_after_core_tick"]):e for e in busy_trace["call_events"]}
    first_busy=busy_by_tick.get(53)
    if first_busy is None or int(first_busy["busy_helper_return"])!=0 or int(first_busy["source_counter"])!=int(rows[53]["counter"]):
        raise SystemExit("source audio busy transition is not uniquely captured at counter5347/tick53")
    start_trace=json.loads(VOICE_START_TRACE.read_text(encoding="utf-8"))
    start_by_tick={int(e["observed_after_core_tick"]):e for e in start_trace["call_events"]}
    if set(start_by_tick)!={5,138,588}:
        raise SystemExit(f"unexpected source contextual voice starts: {sorted(start_by_tick)}")
    start138_pre=VOICE_START_PRE_RAM.read_bytes();start138_post=VOICE_START_POST_RAM.read_bytes()
    cleanup_pre=VOICE_CLEAN_PRE_RAM.read_bytes();cleanup_post=VOICE_CLEAN_POST_RAM.read_bytes()
    start588_pre=VOICE_FINAL_PRE_RAM.read_bytes();start588_post=VOICE_FINAL_POST_RAM.read_bytes()
    if any(len(x)!=0x40000 for x in (start138_pre,start138_post,cleanup_pre,cleanup_post,start588_pre,start588_post)):
        raise SystemExit("context-voice lifecycle EWRAM checkpoint has unexpected size")
    if start_trace.get("instrumentation",{}).get("source_rom_sha256")!=ROM_SHA:
        raise SystemExit("context voice start probe does not match the supplied ROM")
    rng_final=json.loads(RNG_FINAL_TRACE.read_text(encoding="utf-8"))
    rng_report=json.loads(RNG_FINAL_REPORT.read_text(encoding="utf-8"))
    rng_rows=json.loads(RNG_FINAL_ROWS.read_text(encoding="utf-8"))
    final_rng_events=[e for e in rng_final["call_events"] if e["call_site"].lower()=="0x8029086"]
    if len(final_rng_events)!=1 or int(final_rng_events[0]["tick"])!=588 or int(final_rng_events[0]["rng_before"])!=u32(start588_pre,0x3b04):
        raise SystemExit("filtered counter588 voice-selection RNG call does not match the clean pre-event snapshot")
    if rng_report.get("instrumentation",{}).get("source_rom_sha256")!=ROM_SHA or rng_report.get("frames")!=600:
        raise SystemExit("filtered counter588 RNG callsite capture is not the expected source run")
    event_voice=(u32(audio_pre,0x2604),u32(audio_pre,0x2608),s8(audio_pre[0x25d6]),
        s8(audio_pre[0x2610]),s8(audio_pre[0x25d7]),audio_pre[0x25d8],
        audio_pre[0x1696],audio_pre[0x1697],audio_pre[0x5f20])
    lines=[]; event_ticks=[]; last_previous=rows[0]["previous"]
    for i,row in enumerate(rows):
        boundary = (i == 0) or (row["previous"] != last_previous)
        if boundary: event_ticks.append(i)
        last_previous=row["previous"]
        bits=sum(1 << KEY_BIT[key] for key in row["input_keys"])
        active_low=0x3FF ^ bits
        camera=row["traffic_camera"]["position"]
        audio_action=-1; audio_fields=(0,0,0,0,0,0,0,0,0,0,0)
        if i==53:
            audio_action=first_busy["busy_helper_return"]
            audio_fields=(*event_voice,1,0)
        elif i==133:
            # The source selection byte is observed at the clean boundary;
            # its producing game-side selector is still unresolved.
            if rows[132]["traffic_globals"]["voice_25d6"]!=255 or row["traffic_globals"]["voice_25d6"]!=23 or rows[132]["traffic_globals"]["rng_3b04"]!=row["traffic_globals"]["rng_3b04"]:
                raise SystemExit("counter5427 selection-23 source boundary changed")
            audio_action=-2; audio_fields=(0,0,23,0,0,0,0,0,0,0,0)
        elif i==138:
            audio_action=-3
            audio_fields=(u32(start138_pre,0x2604),u32(start138_pre,0x2608),s8(start138_pre[0x25d6]),
                s8(start138_pre[0x2610]),s8(start138_pre[0x25d7]),start138_pre[0x25d8],
                start138_pre[0x1696],start138_pre[0x1697],start138_pre[0x5f20],1,
                int(start_by_tick[138]["returned_handle"],0))
        elif i==588:
            audio_action=-4
            audio_fields=(u32(start588_pre,0x2604),u32(start588_pre,0x2608),s8(start588_pre[0x25d6]),
                s8(start588_pre[0x2610]),s8(start588_pre[0x25d7]),start588_pre[0x25d8],
                start588_pre[0x1696],start588_pre[0x1697],start588_pre[0x5f20],1,
                int(start_by_tick[588]["returned_handle"],0))
        elif i in busy_by_tick:
            audio_action=int(busy_by_tick[i]["busy_helper_return"])
            audio_fields=(0,0,0,0,0,0,0,0,0,0,0)
        lines.append(f"{active_low} {row['delta']} {row['frame_delta']} {int(boundary)} {i} {int(boundary)} {camera[0]} {camera[2]} {audio_action} "
            +" ".join(map(str,audio_fields))+"\n")
    result=subprocess.run([str(BIN),str(PRE_RAM),str(PRE_IWRAM),str(ACTOR_FILE),str(CFG_FILE)],
        input="".join(lines),text=True,capture_output=True,cwd=ROOT.parent.parent,
        timeout=120,check=True)

    events:dict[int,dict]={}; camera_events={}; voice_events={}; voice_counts={}; selector_events={}; random_events={}; reward_events={}
    for line in result.stdout.splitlines():
        p=line.split()
        if p[0]=="S":
            v=list(map(int,p[1:]));tick=v[0]
            events[tick]={"source_tick":tick,"spawned":v[1],"count":v[2],"timer":v[3],
                "counter":v[4],"cooldown":v[5],"near":v[6],"voice":v[7],
                "combo_flag":v[8],"combo_whole":v[9],"combo_fraction":v[10],"rng":v[11],
                "actors":[]}
        elif p[0]=="A":
            v=list(map(int,p[1:])); tick,index=v[:2]
            events[tick]["actors"].append(dict(zip(("index",*ACTOR_FIELDS),(index,*v[2:]))) )
        elif p[0]=="V":
            v=list(map(int,p[1:]));tick=v[0]
            events[tick]["vehicle"] = dict(zip(("x","y","z","heading","speed"),v[1:]))
        elif p[0]=="C":
            v=list(map(int,p[1:]));tick=v[0]
            camera_events[tick]=dict(zip(("x","y","z","pitch","yaw","roll","tracked_heading","tracked_pitch","yaw_offset"),v[1:]))
        elif p[0]=="Q":
            v=list(map(int,p[1:]));tick=v[0]
            voice_events[tick]=dict(zip(("busy","handle_before","selection_before","handle_after",
                "selection_after","followup_before","followup_after","last_before","last_after",
                "classification_before","classification_after","rng_before","rng_after"),v[1:]))
        elif p[0]=="N":
            v=list(map(int,p[1:]));voice_counts[v[0]]=v[1]
        elif p[0]=="U":
            v=list(map(int,p[1:]));selector_events[v[0]]=dict(before=v[1],after=v[2])
        elif p[0]=="R":
            v=list(map(int,p[1:]));random_events[v[0]]=dict(rng_before=v[1],selection=v[2])
        elif p[0]=="W":
            v=list(map(int,p[1:]));tick=v[0]
            reward_events[tick]=dict(zip(("pending_before","pending_after","cooldown_before",
                "cooldown_after","request_count","reward_applied","voice","combo_flag",
                "combo_whole","combo_fraction","rng_before","rng_after"),v[1:]))
    if sorted(events) != event_ticks:
        raise SystemExit(f"stateful harness event ticks differ: {sorted(events)} vs {event_ticks}")

    # Reconcile the one audio-owner transition within this source window from
    # clean RAM snapshots plus a temporary hook whose entire core trace was
    # checked against the uninstrumented capture. No source voice/RNG values
    # are copied into the native result; the recovered C routine computes them.
    clean_capture=json.loads(AUDIO_CLEAN_REPORT.read_text(encoding="utf-8"))
    clean_rows=json.loads(AUDIO_CLEAN_TRACE.read_text(encoding="utf-8"))
    hooked_capture=json.loads(AUDIO_BUSY_REPORT.read_text(encoding="utf-8"))
    hooked_rows=json.loads((REPORTS/"r5294audiobusy600.actor-trace.json").read_text(encoding="utf-8"))
    busy_trace=json.loads(AUDIO_BUSY_TRACE.read_text(encoding="utf-8"))
    if clean_capture.get("rom_sha256")!=ROM_SHA or clean_capture.get("instrumentation") is not None or len(clean_rows)!=60:
        raise SystemExit("clean audio checkpoint replay is not an uninstrumented matching source capture")
    if hooked_capture.get("rom_sha256")!=ROM_SHA or len(hooked_rows)!=600:
        raise SystemExit("audio busy probe does not use the expected ROM and 600-frame schedule")
    start_capture=json.loads(VOICE_START_REPORT.read_text(encoding="utf-8"))
    start_rows=json.loads(VOICE_START_ROWS.read_text(encoding="utf-8"))
    if start_capture.get("rom_sha256")!=ROM_SHA or len(start_rows)!=600 or start_capture.get("instrumentation",{}).get("source_rom_sha256")!=ROM_SHA:
        raise SystemExit("voice-start probe does not use the expected ROM and 600-frame schedule")
    harness_only={"route_driver","completed_count","fare_whole","fare_fraction","remaining_clock"}
    for reference, observed in ((rows[:60],clean_rows),(rows,hooked_rows),(rows,start_rows),(rows,rng_rows)):
        for tick,(expected_row,observed_row) in enumerate(zip(reference,observed)):
            for key in set(expected_row)&set(observed_row)-harness_only:
                if expected_row[key]!=observed_row[key]:
                    raise SystemExit(f"audio checkpoint replay mismatch at tick{tick}, field {key}")
    busy_events=[e for e in busy_trace["call_events"] if int(e["source_counter"])==int(rows[53]["counter"])]
    if len(busy_events)!=1 or int(busy_events[0]["busy_helper_return"])!=0 or busy_events[0]["observed_after_core_tick"]!=53:
        raise SystemExit("source audio busy transition is not uniquely captured at counter5347/tick53")
    if hooked_capture.get("rom_sha256")!=clean_capture.get("rom_sha256"):
        raise SystemExit("audio hook source ROM does not match the clean capture")
    audio_pre=AUDIO_PRE_RAM.read_bytes();audio_post=AUDIO_POST_RAM.read_bytes()
    for tick,ram in ((52,audio_pre),(53,audio_post)):
        source_row=clean_rows[tick]
        if len(ram)!=0x40000 or ram[0xF4:0xF4+50*STRIDE]!=bytes.fromhex(source_row["traffic_actor_records"]["raw"]):
            raise SystemExit(f"audio checkpoint RAM does not match source actor table at tick{tick}")
        if u32(ram,0x2604)!=(0x020075D4 if tick==52 else 0):
            raise SystemExit(f"audio context handle did not match source owner at tick{tick}")
    if 53 not in voice_events or voice_counts.get(53)!=2:
        raise SystemExit("native frame harness did not execute the captured context-voice cleanup at tick53")
    voice_context_audit=voice_events[53]
    expected_voice={"handle_after":u32(audio_post,0x2604),
        "selection_after":s8(audio_post[0x25d6]),"followup_after":s8(audio_post[0x25d7]),
        "last_after":audio_post[0x25d8],"classification_after":s8(audio_post[0x2610]),
        "rng_after":u32(audio_post,0x3b04)}
    if any(voice_context_audit[k]!=v for k,v in expected_voice.items()):
        raise SystemExit(f"native frame audio event differs from source post-checkpoint: {voice_context_audit}")
    if voice_context_audit["selection_before"]!=s8(audio_pre[0x25d6]):
        raise SystemExit("native in-loop voice event did not start from the captured pre-event selection")
    if events[53]["rng"]!=voice_context_audit["rng_after"]:
        raise SystemExit("native stateful traffic replay RNG differs after in-loop voice cleanup")
    if selector_events.get(133)!={"before":-1,"after":23}:
        raise SystemExit(f"tick133 source selector boundary was not recorded as an external input: {selector_events.get(133)}")
    if voice_counts.get(138)!=1 or voice_events.get(138,{}).get("busy")!=-3:
        raise SystemExit("native contextual voice start did not run at source tick138")
    start138_expected={"handle_after":u32(start138_post,0x2604),"selection_after":s8(start138_post[0x25d6]),
        "last_after":start138_post[0x25d8],"classification_after":s8(start138_post[0x2610]),
        "rng_after":u32(start138_post,0x3b04)}
    if any(voice_events[138][k]!=v for k,v in start138_expected.items()) or voice_events[138]["handle_before"]!=u32(start138_pre,0x2604):
        raise SystemExit(f"native tick138 voice start differs from clean RAM: {voice_events[138]}")
    if any(u32(start138_pre,offset)!=u32(start138_post,offset) for offset in (0x3b04,)):
        raise SystemExit("source contextual voice start unexpectedly advanced RNG")
    expected_busy_ticks={int(e["observed_after_core_tick"]):int(e["busy_helper_return"]) for e in busy_trace["call_events"]}
    if set(expected_busy_ticks)-set(voice_events):
        raise SystemExit(f"native context-voice dispatch missed busy events: {sorted(set(expected_busy_ticks)-set(voice_events))}")
    for tick,busy_value in expected_busy_ticks.items():
        if voice_events[tick]["busy"]!=busy_value:
            raise SystemExit(f"native context-voice busy input differs at tick{tick}")
    if voice_counts.get(203)!=1:
        raise SystemExit("native context-voice cleanup at tick203 was not executed")
    clean203={"handle_after":u32(cleanup_post,0x2604),"selection_after":s8(cleanup_post[0x25d6]),
        "followup_after":s8(cleanup_post[0x25d7]),"last_after":cleanup_post[0x25d8],
        "classification_after":s8(cleanup_post[0x2610]),"rng_after":u32(cleanup_post,0x3b04)}
    if any(voice_events[203][k]!=v for k,v in clean203.items()) or voice_events[203]["handle_before"]!=u32(cleanup_pre,0x2604):
        raise SystemExit(f"native tick203 voice cleanup differs from clean RAM: {voice_events[203]}")
    if random_events.get(588)!={"rng_before":u32(start588_pre,0x3b04),"selection":s8(start588_post[0x25d6])}:
        raise SystemExit(f"native tick588 voice selector differs from source RNG checkpoint: {random_events.get(588)}")
    source587=rows[587]["traffic_globals"];source588=rows[588]["traffic_globals"]
    source592=rows[592]["traffic_globals"];source593=rows[593]["traffic_globals"]
    reward588=reward_events.get(588);reward593=reward_events.get(593)
    if reward588 is None or reward593 is None:
        raise SystemExit(f"native near-miss reward dispatch missing: {sorted(reward_events)}")
    expected_reward588={"pending_before":source587["near_miss_25d3"],"pending_after":0,
        "cooldown_before":source587["timer_25d5"],"cooldown_after":20,"request_count":2,
        "reward_applied":1,"voice":source588["voice_25d6"],"combo_flag":source588["combo_flag_9aa6"],
        "combo_whole":source588["combo_9aa8"],"combo_fraction":source588["combo_9aac"],
        "rng_before":source587["rng_3b04"],"rng_after":source588["rng_3b04"]}
    if reward588!=expected_reward588:
        raise SystemExit(f"native reward routine differs from source tick588 inputs/outputs: {reward588}")
    expected_reward593={"pending_before":source592["near_miss_25d3"],"pending_after":0,
        "cooldown_before":source592["timer_25d5"],"cooldown_after":source592["timer_25d5"],
        "request_count":0,"reward_applied":0,"voice":source592["voice_25d6"],
        "combo_flag":source592["combo_flag_9aa6"],"combo_whole":source592["combo_9aa8"],
        "combo_fraction":source592["combo_9aac"],"rng_before":source592["rng_3b04"],
        "rng_after":source592["rng_3b04"]}
    if reward593!=expected_reward593 or source588["timer_25d5"]!=reward588["cooldown_after"]-rows[588]["frame_delta"] or source593["timer_25d5"]!=reward593["cooldown_after"]-rows[593]["frame_delta"]:
        raise SystemExit(f"native reward cooldown/pending lifecycle differs from source ticks588/593: {reward593}")
    start588_expected={"handle_after":u32(start588_post,0x2604),"selection_after":s8(start588_post[0x25d6]),
        "last_after":start588_post[0x25d8],"classification_after":s8(start588_post[0x2610]),
        "rng_after":u32(start588_post,0x3b04)}
    if any(voice_events[588][k]!=v for k,v in start588_expected.items()):
        raise SystemExit(f"native tick588 voice selection/start differs from clean RAM: {voice_events[588]}")

    native_pool=bytearray(initial_pool)
    completed=[]; next_completed_row=-1; unverified_final_events=[]
    for event_tick in event_ticks:
        event=events[event_tick]
        target_tick=event_tick+1
        if target_tick>=len(rows):
            unverified_final_events.append(event_tick)
            continue
        if len(event["actors"])!=prefix_count:
            raise SystemExit(f"native event at {event_tick} returned {len(event['actors'])} actors")
        rebuild_pool(native_pool,event["actors"],route_to_descriptor,route_to_points)
        target=rows[target_tick]
        source_pool=bytes.fromhex(target["traffic_actor_records"]["raw"])
        actor_field_mismatches=[]
        expected=actors_from_raw(source_pool,descriptor_to_route)
        for actor in event["actors"]:
            expected_fields=dict(zip(EXPECTED_FIELDS,expected[actor["index"]]))
            diff={k:{"native":actor[k],"source":v} for k,v in expected_fields.items() if actor[k]!=v}
            if diff: actor_field_mismatches.append({"slot":actor["index"],"fields":diff})
        raw_bad=pool_mismatches(native_pool,source_pool)
        srcg=target["traffic_globals"]
        wanted={"count":prefix_count,"timer":srcg["timer_25d5"],
            "counter":srcg["selection_counter_4c98"],"cooldown":srcg["cooldown_e4"],
            "near":srcg["near_miss_25d3"],"voice":srcg["voice_25d6"],
            "combo_flag":srcg["combo_flag_9aa6"],"combo_whole":srcg["combo_9aa8"],
            "combo_fraction":srcg["combo_9aac"],"rng":srcg["rng_3b04"]}
        native_g={k:event[k] for k in wanted}
        g_bad={k:{"native":native_g[k],"source":v} for k,v in wanted.items() if native_g[k]!=v}
        vehicle_source={"x":target["position"][0],"y":target["position"][1],
            "z":target["position"][2],"heading":target["heading"],"speed":target["speed"]}
        v_bad={k:{"native":event["vehicle"][k],"source":v}
               for k,v in vehicle_source.items() if event["vehicle"][k]!=v}
        native_camera=camera_events.get(event_tick)
        if native_camera is None: raise SystemExit(f"native camera result missing at event {event_tick}")
        source_camera=target["traffic_camera"]["position"]
        camera_bad={axis:{"native":native_camera[axis],"source":source_camera[i]}
            for i,axis in enumerate(("x","y","z")) if native_camera[axis]!=source_camera[i]}
        completed.append({"event_tick":event_tick,"source_completion_tick":target_tick,
            "actor_fields_matched":prefix_count-len(actor_field_mismatches),
            "actor_field_mismatches":actor_field_mismatches,
            "full_50_actor_table_byte_equal":not raw_bad,"raw_record_mismatches":raw_bad,
            "traffic_global_mismatches":g_bad,"vehicle_mismatches":v_bad,
            "native_camera":native_camera,"source_camera_position":source_camera,
            "camera_position_mismatches":camera_bad})
        next_completed_row=target_tick

        # Between dispatcher boundaries, the completed native state should stay
        # equal to the source. Skip each event's first (partially processed) VBlank.
        next_event=next((t for t in event_ticks if t>event_tick),len(rows))
        for stable_tick in range(target_tick+1,next_event):
            stable=rows[stable_tick]
            stable_pool=bytes.fromhex(stable["traffic_actor_records"]["raw"])
            stable_bad=pool_mismatches(native_pool,stable_pool)
            stable_pos={"x":stable["position"][0],"y":stable["position"][1],
                "z":stable["position"][2],"heading":stable["heading"],"speed":stable["speed"]}
            stable_v_bad={k:{"native":event["vehicle"][k],"source":v}
                for k,v in stable_pos.items() if event["vehicle"][k]!=v}
            stable_camera=stable["traffic_camera"]["position"]
            stable_camera_bad={axis:{"native":native_camera[axis],"source":stable_camera[i]}
                for i,axis in enumerate(("x","y","z")) if native_camera[axis]!=stable_camera[i]}
            stable_g=stable["traffic_globals"]
            stable_expected={"timer":stable_g["timer_25d5"],"counter":stable_g["selection_counter_4c98"],
                "cooldown":stable_g["cooldown_e4"],"near":stable_g["near_miss_25d3"],
                "voice":stable_g["voice_25d6"],"combo_flag":stable_g["combo_flag_9aa6"],
                "combo_whole":stable_g["combo_9aa8"],"combo_fraction":stable_g["combo_9aac"],
                "rng":stable_g["rng_3b04"]}
            stable_g_bad={k:{"native":native_g[k],"source":v}
                for k,v in stable_expected.items() if native_g.get(k)!=v}
            if stable_bad or stable_v_bad or stable_g_bad or stable_camera_bad:
                completed[-1].setdefault("between_boundary_mismatches",[]).append({
                    "source_tick":stable_tick,"actor_records":stable_bad,
                    "vehicle":stable_v_bad,"camera":stable_camera_bad,"globals":stable_g_bad})

    passes=all(not r["actor_field_mismatches"] and r["full_50_actor_table_byte_equal"]
        and not r["traffic_global_mismatches"] and not r["vehicle_mismatches"]
        and not r["camera_position_mismatches"] and not r.get("between_boundary_mismatches") for r in completed)
    report={"status":"PASS_COUNTER5294_STATEFUL_NATIVE_607FRAME_PARITY" if passes else "MISMATCH_COUNTER5294_STATEFUL_NATIVE_607FRAME_PARITY",
        "rom_sha256":ROM_SHA,"reference_core_sha256":capture.get("core_sha256"),
        "source_trace_sha256":sha(SOURCE.read_bytes()),"capture_report_sha256":sha(CAPTURE.read_bytes()),
        "source_tail_trace_sha256":sha(TAIL_SOURCE.read_bytes()),"tail_capture_report_sha256":sha(TAIL_CAPTURE.read_bytes()),
        "source_continuation_manifest_sha256":sha(TAIL_MANIFEST.read_bytes()),
        "source_tail2_trace_sha256":sha(TAIL2_SOURCE.read_bytes()),"tail2_capture_report_sha256":sha(TAIL2_CAPTURE.read_bytes()),
        "source_tail2_continuation_manifest_sha256":sha(TAIL2_MANIFEST.read_bytes()),
        "source_tail3_trace_sha256":sha(TAIL3_SOURCE.read_bytes()),"tail3_capture_report_sha256":sha(TAIL3_CAPTURE.read_bytes()),
        "source_tail3_continuation_manifest_sha256":sha(TAIL3_MANIFEST.read_bytes()),
        "pre_ewram_sha256":sha(pre),"pre_iwram_sha256":sha(iwram),
        "frames":len(rows),"event_ticks":event_ticks,"completed_source_ticks":[t+1 for t in event_ticks if t+1<len(rows)],
        "terminal_events_without_following_checkpoint":unverified_final_events,
        "native_state_persisted_between_events":True,"event_results":completed,
        "source_scheduled_native_context_voice_event":voice_context_audit,
        "source_scheduled_native_near_miss_reward_event":{
            "tick588":reward588,"tick593":reward593,
            "source_tick588_final_timer":source588["timer_25d5"],
            "source_tick593_final_timer":source593["timer_25d5"],
            "input_sources":"near-miss, cooldown, speed, active/sound/settings, RNG and audio handle are read from carried native/source state and the clean captured callback boundary; only the source-observed start handle is supplied.",
            "scope":"Native TaxiGameAudioRewardEvent and TaxiReward state transitions; channel-44 audio playback and tip rendering are not verified."},
        "audio_source_evidence":{"clean_report_sha256":sha(AUDIO_CLEAN_REPORT.read_bytes()),
            "clean_trace_sha256":sha(AUDIO_CLEAN_TRACE.read_bytes()),
            "busy_report_sha256":sha(AUDIO_BUSY_REPORT.read_bytes()),
            "busy_trace_sha256":sha(AUDIO_BUSY_TRACE.read_bytes()),
            "pre_ewram_sha256":sha(audio_pre),"post_ewram_sha256":sha(audio_post),
            "instrumented_vs_clean_shared_source_fields_equal":True},
        "reference_instrumentation":capture.get("instrumentation"),
        "scope":"Stateful 607-VBlank diagnostic from the exact counter5294 checkpoint, using the accepted clean 600-frame source run plus separately resumed, uninstrumented source segments chained from saved core states. Native vehicle/input and 50-record traffic state are carried across updates. The counter5347 voice cleanup and tick588/593 near-miss reward lifecycle use recovered native routines and captured source boundary inputs. All119 native dispatcher events with following source checkpoints, including the formerly terminal tick599 event and the next counter5899 event, match their 27 active actor records, reconstructed full50-record table, traffic globals/timers/RNG, and vehicle position/heading/speed. The native camera-follow output position is also compared with source camera position at each completed boundary and stable intervening source sample. The final source sample is tick606 and has no following checkpoint. This does not verify Windows audio playback, all reward/tip visuals, camera implementation, passenger/fare completion, long-run speed, or a complete playable trip."}
    OUTPUT.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps({"status":report["status"],"frames":report["frames"],
        "event_count":len(event_ticks),"last_event_tick":event_ticks[-1],"all_events_pass":passes,
        "failures":[r for r in completed if r["actor_field_mismatches"] or not r["full_50_actor_table_byte_equal"] or r["traffic_global_mismatches"] or r["vehicle_mismatches"] or r["camera_position_mismatches"] or r.get("between_boundary_mismatches")]},indent=2))
    return 0 if passes else 2


if __name__=="__main__":
    raise SystemExit(main())
