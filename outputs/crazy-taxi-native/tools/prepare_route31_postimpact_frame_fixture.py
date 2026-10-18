#!/usr/bin/env python3
"""Export clean-ROM route31 native traffic-scene fixtures across two capture phases."""
import hashlib,json,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]; R=ROOT/"reports/original-core-reference"
TRACE=R/"route31clean30b.actor-trace.json"; CAP=R/"route31clean30b.capture.json"; SELECTOR_TRACE=R/"route31selectorentry8211.traffic-follow-entry-trace.json"
ROADS=ROOT/"assets/traffic/city1-roads.json"
ROM="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
def u32(b,o):return struct.unpack_from("<I",b,o)[0]
def s32(b,o):return struct.unpack_from("<i",b,o)[0]
def actors(row,descriptors):
    raw=bytes.fromhex(row["traffic_actor_records"]["raw"]); assert len(raw)==50*92
    out=[]
    for i in range(50):
        p=i*92; desc=u32(raw,p+0x24); route=descriptors.get(desc,0)
        assert not desc or route,(i,hex(desc))
        out.append((u32(raw,p),u32(raw,p+4),s32(raw,p+0x10),s32(raw,p+0x14),s32(raw,p+0x18),s32(raw,p+0x1c),s32(raw,p+0x20),route,s32(raw,p+0x28),u32(raw,p+0x2c),s32(raw,p+0x34),s32(raw,p+0x38),struct.unpack_from("<H",raw,p+0x3c)[0],s32(raw,p+0x40),s32(raw,p+0x48),s32(raw,p+0x4c),s32(raw,p+0x54),s32(raw,p+0x58)))
    return out
cap=json.loads(CAP.read_text()); rows=json.loads(TRACE.read_text()); by={r["counter"]:r for r in rows}
selector_trace=json.loads(SELECTOR_TRACE.read_text()); selector_events=selector_trace["call_events"]
assert len(selector_events)==13 and all(e["selector_counter_4c98_entry"]==111 for e in selector_events)
selector_calls_by_counter={str(c):[e["selector_counter_4c98_entry"] for e in selector_events if e["source_counter"]==c] for c in sorted({e["source_counter"] for e in selector_events})}
assert cap["status"]=="PASS" and cap["rom_sha256"]==ROM and cap["frames"]==30 and len(rows)==30
roads=json.loads(ROADS.read_text()); assert roads["rom_sha256"]==ROM and roads["city"]=="city1"
descriptors={int(x["descriptor_address"],16):int(x["index"]) for x in roads["paths"]}
cases=[]
for pool_row,driver_row,result_row in ((8217,8218,8219),(8221,8223,8224)):
    pool,driver,result=by[pool_row],by[driver_row],by[result_row]
    assert pool["delta"]==driver["delta"]==result["delta"]==5 and driver["frame_delta"]==5
    assert driver["position"]==result["position"] and driver["heading"]==result["heading"] and driver["speed"]==result["speed"]
    pre=actors(pool,descriptors); expected=actors(result,descriptors)
    selector_rows={name:row["traffic_globals"]["selection_counter_4c98"] for name,row in (("pool",pool),("driver",driver),("result",result))}
    assert selector_rows=={"pool":111,"driver":111,"result":111},(pool_row,driver_row,result_row,selector_rows)
    stem=f"route31-postimpact-frame-{pool_row}-{driver_row}-{result_row}"
    pool_file=stem+".pool.bin"; expected_file=stem+".expected.bin"; cfg_file=stem+".cfg"
    (R/pool_file).write_bytes(b"".join(struct.pack("<18i",*a) for a in pre))
    (R/expected_file).write_bytes(b"".join(struct.pack("<18i",*a) for a in expected))
    g=pool["traffic_globals"]; vehicle=driver; camera=driver["traffic_camera"]["position"]
    setting=68; active_count=27
    cfg=[setting,active_count,g["timer_25d5"],g["selection_counter_4c98"],g["near_miss_25d3"],g["voice_25d6"],g["combo_flag_9aa6"],pool["traffic_frame_context"]["sound_7602"],pool["traffic_frame_context"]["trip_active_1694"],0,driver["delta"],driver["frame_delta"],*vehicle["position"],vehicle["heading"],vehicle["speed"],*camera,g["cooldown_e4"],g["combo_9aa8"],g["combo_9aac"],g["rng_3b04"]]
    assert len(cfg)==24
    (R/cfg_file).write_text(" ".join(map(str,cfg))+"\n",encoding="ascii")
    cases.append({"actor_pool_row":pool_row,"driver_state_row":driver_row,"traffic_result_row":result_row,"delta":driver["delta"],"frame_delta":driver["frame_delta"],"active_count_assumption":active_count,"active_count_basis":"count27 corresponds to scene setting0x44 from the earlier event-aligned source probe; it is distinct from selection_counter_4c98, which is111 in all three clean fixture rows and directly sampled as111 at the helper-entry calls listed in selector_calls_by_counter.","selection_counter_4c98":selector_rows,"helper_entry_selector_samples":selector_calls_by_counter,"pool_fixture":pool_file,"expected_fixture":expected_file,"config":cfg_file,"native_log":stem+".native.log"})
report={"status":"PASS_FIXTURES_EXPORTED","rom_sha256":ROM,"capture_report_sha256":hashlib.sha256(CAP.read_bytes()).hexdigest(),"selector_trace_sha256":hashlib.sha256(SELECTOR_TRACE.read_bytes()).hexdigest(),"direct_helper_selector_values":sorted({e["selector_counter_4c98_entry"] for e in selector_events}),"capture_core_sha256":cap["core_sha256"],"source_trace_sha256":hashlib.sha256(TRACE.read_bytes()).hexdigest(),"cases":cases,"sequence_log":"route31-postimpact-frame-native-sequence.log","scope":"Each case uses the clean pre-update actor pool/globals and the same logical frame's driver-updated player/camera state, then compares one native traffic-scene update with the later source row after the source's split actor update becomes visible. This is a phase-aligned single-update check, not replay of the original driver input or multi-frame schedule."}
(R/"route31-postimpact-frame-fixtures.json").write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8"); print(json.dumps(report,indent=2))
