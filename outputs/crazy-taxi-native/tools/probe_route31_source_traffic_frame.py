#!/usr/bin/env python3
"""Run one full native traffic-frame update against source tick1412."""
import gzip,hashlib,json,struct,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]; R=ROOT/"reports/original-core-reference"
TRACE=R/"route31nativeinputreplaytrace.actor-trace.json.gz"; CAP=R/"route31nativeinputreplaytrace.route-follow.json"
BOUND=R/"route31-controller-sample-boundaries.json"; FIX=R/"route31pickupstate516.ewram-vblank-00516.bin"; IW=R/"route31pickupstate516.03000000.bin"
ROADS=ROOT/"assets/traffic/city1-roads.json"; BIN=ROOT/"bin/route31-source-traffic-frame-probe.exe"
ACTORS=R/"route31-source-traffic-frame-slot1411.bin"; CFG=R/"route31-source-traffic-frame-slot1411.cfg"; OUT=R/"route31-source-traffic-frame-slot1412.json"
ROM="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"; KEYS={0:1,6:5,7:4,8:0}
def u32(b,o):return struct.unpack_from("<I",b,o)[0]
def s32(b,o):return struct.unpack_from("<i",b,o)[0]
def actors_for(row,descriptors):
    raw=bytes.fromhex(row["traffic_actor_records"]["raw"]); out=[]
    assert len(raw)==50*92
    for i in range(50):
        p=i*92; desc=u32(raw,p+0x24); route=descriptors.get(desc,0)
        assert not desc or route,(i,hex(desc))
        out.append((u32(raw,p),u32(raw,p+4),s32(raw,p+0x10),s32(raw,p+0x14),s32(raw,p+0x18),s32(raw,p+0x1c),s32(raw,p+0x20),route,s32(raw,p+0x28),u32(raw,p+0x2c),s32(raw,p+0x34),s32(raw,p+0x38),struct.unpack_from("<H",raw,p+0x3c)[0],s32(raw,p+0x40),s32(raw,p+0x48),s32(raw,p+0x4c),s32(raw,p+0x54),s32(raw,p+0x58)))
    return out
raw_trace=TRACE.read_bytes(); th=hashlib.sha256(raw_trace).hexdigest(); cap=json.loads(CAP.read_text()); boundary=json.loads(BOUND.read_text()); mapping=json.loads(ROADS.read_text())
assert cap["rom_sha256"]==ROM and cap["route_source"]==31 and cap["frames"]==12000
assert boundary["trace_sha256"]==th
rows=json.loads(gzip.decompress(raw_trace)); before,after=rows[1411],rows[1412]
fixture=FIX.read_bytes(); assert fixture[0x1eb3]==68
setting=fixture[0x1eb3]; count=(5,11,17,22,27,33,39,44,50,28,26,24,22,20,18,16)[setting>>4]
assert before["traffic_frame_context"]["trip_active_1694"]==1
paths=mapping["paths"]; descriptors={int(x["descriptor_address"],16):int(x["index"]) for x in paths}
pre=actors_for(before,descriptors); expected=actors_for(after,descriptors)
ACTORS.write_bytes(b"".join(struct.pack("<18i",*a) for a in pre))
g=before["traffic_globals"]; cam=before["traffic_camera"]["position"]
config=[setting,count,g["timer_25d5"],g["selection_counter_4c98"],g["cooldown_e4"],g["near_miss_25d3"],g["voice_25d6"],g["combo_flag_9aa6"],g["combo_9aa8"],g["combo_9aac"],g["rng_3b04"],before["traffic_frame_context"]["sound_7602"],before["traffic_frame_context"]["trip_active_1694"],cam[0],cam[2]]
CFG.write_text(" ".join(map(str,config))+"\n",encoding="ascii")
last=rows[516]["previous"]; inp=[]
for tick in range(517,1413):
    row=rows[tick]; bits=0
    for key in row["input_keys"]:bits|=1<<KEYS[key]
    boundary_tick=int(row["previous"]!=last); last=row["previous"]
    inp.append(f"{0x3ff^bits} {row['delta']} {row['frame_delta']} {boundary_tick} {tick} {int(tick==1412)}\n")
proc=subprocess.run([str(BIN),str(FIX),str(IW),str(ACTORS),str(CFG)],input="".join(inp),text=True,capture_output=True,cwd=ROOT.parent.parent,timeout=90,check=True)
lines=proc.stdout.splitlines(); sline=next(x.split() for x in lines if x.startswith("S ")); vline=next(x.split() for x in lines if x.startswith("V "))
actual=[]
for line in lines:
    if line.startswith("A "):
        v=list(map(int,line.split()[1:])); actual.append(dict(zip(("index","flags","type","countdown","x","y","z","heading","route","collision_timer","impulse_x","impulse_z","offset_x","offset_z","path_distance","path_segment","path_x","path_z","path_heading"),v)))
assert len(actual)==count,(len(actual),count,proc.stdout[-1000:])
expected_obj=[]
for i,a in enumerate(expected[:count]):
    expected_obj.append(dict(zip(("flags","type","countdown","x","y","z","heading","route","path_distance","path_segment","path_x","path_z","path_heading","impulse_x","impulse_z","offset_x","offset_z","collision_timer"),a)))
# Rebuild explicit indexed source expectation in the same field order.
expected_obj=[]
for i,a in enumerate(expected[:count]):
    keys=("flags","type","countdown","x","y","z","heading","route","path_distance","path_segment","path_x","path_z","path_heading","impulse_x","impulse_z","offset_x","offset_z","collision_timer")
    obj=dict(zip(keys,a));obj["index"]=i;expected_obj.append(obj)
field_names=("flags","type","countdown","x","y","z","heading","route","collision_timer","impulse_x","impulse_z","offset_x","offset_z","path_distance","path_segment","path_x","path_z","path_heading")
actor_mismatches=[]
for a,e in zip(actual,expected_obj):
    bad={k:{"native":a[k],"source":e[k]} for k in field_names if a[k]!=e[k]}
    if bad:actor_mismatches.append({"index":a["index"],"fields":bad})
# S row fields: tick, spawned, count, timer, counter, cooldown, near, voice, combo flag, combo whole, combo fraction, rng.
s=list(map(int,sline[1:])); source_g=after["traffic_globals"]
actual_g=dict(zip(("tick","spawned","count","timer","counter","cooldown","near","voice","combo_flag","combo_whole","combo_fraction","rng"),s))
expected_g={"tick":1412,"spawned":None,"count":count,"timer":source_g["timer_25d5"],"counter":source_g["selection_counter_4c98"],"cooldown":source_g["cooldown_e4"],"near":source_g["near_miss_25d3"],"voice":source_g["voice_25d6"],"combo_flag":source_g["combo_flag_9aa6"],"combo_whole":source_g["combo_9aa8"],"combo_fraction":source_g["combo_9aac"],"rng":source_g["rng_3b04"]}
actual_v=list(map(int,vline[1:])); expected_v={"tick":1412,"position":after["position"],"heading":after["heading"],"speed":after["speed"]}; got_v={"tick":actual_v[0],"position":actual_v[1:4],"heading":actual_v[4],"speed":actual_v[5]}
expected_g["spawned"]=actual_g["spawned"]
global_mismatches={k:{"native":actual_g[k],"source":v} for k,v in expected_g.items() if actual_g[k]!=v}
report={"status":"PASS_FULL_SOURCE_TRAFFIC_FRAME_TICK1412" if not actor_mismatches and not global_mismatches and got_v==expected_v else "MISMATCH_FULL_SOURCE_TRAFFIC_FRAME_TICK1412","rom_sha256":ROM,"trace_sha256":th,"tick":1412,"delta":after["delta"],"frame_delta":after["frame_delta"],"setting":setting,"active_actor_count":count,"vehicle_actual":got_v,"vehicle_expected":expected_v,"traffic_globals_actual":actual_g,"traffic_globals_expected":expected_g,"global_mismatches":global_mismatches,"actor_match_count":count-len(actor_mismatches),"actor_mismatches":actor_mismatches,"scope":"One full native taxi_traffic_frame call across all 27 active slots using the source pre-contact pool, exact captured vehicle/input replay through tick1412, source globals/RNG and source camera position. Other 23 allocated slots are inactive. This single trace-seeded frame does not establish that the actor pool and dispatcher stay aligned over preceding frames, passenger/fare integration, or a playable trip."}
OUT.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
print(json.dumps(report,indent=2))

