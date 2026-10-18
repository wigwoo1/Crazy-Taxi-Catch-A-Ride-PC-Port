#!/usr/bin/env python3
"""Stateful native-vs-source replay for the clean counter5294 60-VBlank run."""
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
SOURCE = REPORTS / "r5294cpu60pool.actor-trace.json"
CAPTURE = REPORTS / "r5294cpu60pool.json"
PRE_RAM = REPORTS / "counter5294-source-pre.ewram"
PRE_IWRAM = REPORTS / "r5294base1100.03000000.bin"
ACTOR_FILE = REPORTS / "counter5294-stateful-60-input.actors.bin"
CFG_FILE = REPORTS / "counter5294-stateful-60-input.cfg"
BIN = ROOT / "bin/counter5294-stateful-traffic-replay.exe"
ROADS = ROOT / "assets/traffic/city1-roads.json"
OUTPUT = REPORTS / "counter5294-native-stateful-60frame-audit.json"
AUDIO_CLEAN_REPORT = REPORTS / "r5294audioownerfix.json"
AUDIO_CLEAN_TRACE = REPORTS / "r5294audioownerfix.actor-trace.json"
AUDIO_BUSY_REPORT = REPORTS / "r5294audiobusy.json"
AUDIO_BUSY_TRACE = REPORTS / "r5294audiobusy.audio-busy-return-trace.json"
AUDIO_PRE_RAM = REPORTS / "r5294audioownerfix.ewram-vblank-00052.bin"
AUDIO_POST_RAM = REPORTS / "r5294audioownerfix.ewram-vblank-00053.bin"
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
    pre = PRE_RAM.read_bytes()
    iwram = PRE_IWRAM.read_bytes()
    if sha(rom) != ROM_SHA or capture.get("status") != "PASS" or capture.get("rom_sha256") != ROM_SHA:
        raise SystemExit("source ROM/capture identity check failed")
    if capture.get("frames") != 60 or capture.get("instrumentation") is not None:
        raise SystemExit("reference is not the accepted uninstrumented 60-frame run")
    if len(rows) != 60 or len(pre) != 0x40000 or len(iwram) != 0x8000:
        raise SystemExit("unexpected source trace or memory checkpoint size")
    if [int(r["counter"]) for r in rows] != list(range(5294,5354)):
        raise SystemExit("source trace counters are not 5294 through 5353")
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
    busy_events=[e for e in busy_trace["call_events"] if int(e["source_counter"])==int(rows[53]["counter"])]
    if len(busy_events)!=1 or int(busy_events[0]["busy_helper_return"])!=0 or busy_events[0]["observed_after_core_tick"]!=53:
        raise SystemExit("source audio busy transition is not uniquely captured at counter5347/tick53")
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
        if i==53:
            audio_fields=(int(busy_events[0]["busy_helper_return"]),*event_voice,1,0)
        else:
            audio_fields=(-1,0,0,0,0,0,0,0,0,0,0,0)
        lines.append(f"{active_low} {row['delta']} {row['frame_delta']} {int(boundary)} {i} {int(boundary)} {camera[0]} {camera[2]} "
            +" ".join(map(str,audio_fields))+"\n")
    result=subprocess.run([str(BIN),str(PRE_RAM),str(PRE_IWRAM),str(ACTOR_FILE),str(CFG_FILE)],
        input="".join(lines),text=True,capture_output=True,cwd=ROOT.parent.parent,
        timeout=120,check=True)

    events:dict[int,dict]={}; voice_events={}; voice_counts={}
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
        elif p[0]=="Q":
            v=list(map(int,p[1:]));tick=v[0]
            voice_events[tick]=dict(zip(("busy","handle_before","selection_before","handle_after",
                "selection_after","followup_before","followup_after","last_before","last_after",
                "classification_before","classification_after","rng_before","rng_after"),v[1:]))
        elif p[0]=="N":
            v=list(map(int,p[1:]));voice_counts[v[0]]=v[1]
    if sorted(events) != event_ticks:
        raise SystemExit(f"stateful harness event ticks differ: {sorted(events)} vs {event_ticks}")

    # Reconcile the one audio-owner transition within this source window from
    # clean RAM snapshots plus a temporary hook whose entire core trace was
    # checked against the uninstrumented capture. No source voice/RNG values
    # are copied into the native result; the recovered C routine computes them.
    clean_capture=json.loads(AUDIO_CLEAN_REPORT.read_text(encoding="utf-8"))
    clean_rows=json.loads(AUDIO_CLEAN_TRACE.read_text(encoding="utf-8"))
    hooked_capture=json.loads(AUDIO_BUSY_REPORT.read_text(encoding="utf-8"))
    hooked_rows=json.loads((REPORTS/"r5294audiobusy.actor-trace.json").read_text(encoding="utf-8"))
    busy_trace=json.loads(AUDIO_BUSY_TRACE.read_text(encoding="utf-8"))
    if clean_capture.get("rom_sha256")!=ROM_SHA or clean_capture.get("instrumentation") is not None or len(clean_rows)!=len(rows):
        raise SystemExit("clean audio checkpoint replay is not an uninstrumented matching source capture")
    if hooked_capture.get("rom_sha256")!=ROM_SHA or len(hooked_rows)!=len(rows):
        raise SystemExit("audio busy probe does not use the expected ROM and 60-frame schedule")
    harness_only={"route_driver","completed_count","fare_whole","fare_fraction","remaining_clock"}
    for reference, observed in ((rows,clean_rows),(clean_rows,hooked_rows)):
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

    native_pool=bytearray(initial_pool)
    completed=[]; next_completed_row=-1
    for event_tick in event_ticks:
        event=events[event_tick]
        target_tick=event_tick+1
        if target_tick>=len(rows):
            raise SystemExit("native event lacks a following completed source VBlank")
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
        completed.append({"event_tick":event_tick,"source_completion_tick":target_tick,
            "actor_fields_matched":prefix_count-len(actor_field_mismatches),
            "actor_field_mismatches":actor_field_mismatches,
            "full_50_actor_table_byte_equal":not raw_bad,"raw_record_mismatches":raw_bad,
            "traffic_global_mismatches":g_bad,"vehicle_mismatches":v_bad})
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
            stable_g=stable["traffic_globals"]
            stable_expected={"timer":stable_g["timer_25d5"],"counter":stable_g["selection_counter_4c98"],
                "cooldown":stable_g["cooldown_e4"],"near":stable_g["near_miss_25d3"],
                "voice":stable_g["voice_25d6"],"combo_flag":stable_g["combo_flag_9aa6"],
                "combo_whole":stable_g["combo_9aa8"],"combo_fraction":stable_g["combo_9aac"],
                "rng":stable_g["rng_3b04"]}
            stable_g_bad={k:{"native":native_g[k],"source":v}
                for k,v in stable_expected.items() if native_g.get(k)!=v}
            if stable_bad or stable_v_bad or stable_g_bad:
                completed[-1].setdefault("between_boundary_mismatches",[]).append({
                    "source_tick":stable_tick,"actor_records":stable_bad,
                    "vehicle":stable_v_bad,"globals":stable_g_bad})

    passes=all(not r["actor_field_mismatches"] and r["full_50_actor_table_byte_equal"]
        and not r["traffic_global_mismatches"] and not r["vehicle_mismatches"]
        and not r.get("between_boundary_mismatches") for r in completed)
    report={"status":"PASS_COUNTER5294_STATEFUL_NATIVE_60FRAME_PARITY" if passes else "MISMATCH_COUNTER5294_STATEFUL_NATIVE_60FRAME_PARITY",
        "rom_sha256":ROM_SHA,"reference_core_sha256":capture.get("core_sha256"),
        "source_trace_sha256":sha(SOURCE.read_bytes()),"capture_report_sha256":sha(CAPTURE.read_bytes()),
        "pre_ewram_sha256":sha(pre),"pre_iwram_sha256":sha(iwram),
        "frames":len(rows),"event_ticks":event_ticks,"completed_source_ticks":[t+1 for t in event_ticks],
        "native_state_persisted_between_events":True,"event_results":completed,
        "source_scheduled_native_context_voice_event":voice_context_audit,
        "audio_source_evidence":{"clean_report_sha256":sha(AUDIO_CLEAN_REPORT.read_bytes()),
            "clean_trace_sha256":sha(AUDIO_CLEAN_TRACE.read_bytes()),
            "busy_report_sha256":sha(AUDIO_BUSY_REPORT.read_bytes()),
            "busy_trace_sha256":sha(AUDIO_BUSY_TRACE.read_bytes()),
            "pre_ewram_sha256":sha(audio_pre),"post_ewram_sha256":sha(audio_post),
            "instrumented_vs_clean_shared_source_fields_equal":True},
        "reference_instrumentation":capture.get("instrumentation"),
        "scope":"Stateful 60-VBlank diagnostic from the exact counter5294 checkpoint. Native vehicle/input and 50-record traffic state are carried across updates. The counter5347 voice cleanup is executed inside the same C frame harness through the ROM-compared native context-voice routine, with only that event's captured busy return and pre-event owner state supplied as inputs; the resulting state and RNG are checked against source RAM. This does not verify Windows audio playback, driver busy timing, camera implementation, passenger/fare completion, long-run speed, or a complete playable trip."}
    OUTPUT.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps({"status":report["status"],"frames":report["frames"],
        "event_ticks":event_ticks,"all_events_pass":passes,
        "failures":[r for r in completed if r["actor_field_mismatches"] or not r["full_50_actor_table_byte_equal"] or r["traffic_global_mismatches"] or r["vehicle_mismatches"] or r.get("between_boundary_mismatches")]},indent=2))
    return 0 if passes else 2


if __name__=="__main__":
    raise SystemExit(main())
