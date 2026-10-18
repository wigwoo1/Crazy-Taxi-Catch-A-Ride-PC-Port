#!/usr/bin/env python3
"""Replay the source slot-9 contact at tick1412 through native traffic follow."""
import gzip, hashlib, json, struct, subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
R=ROOT/"reports/original-core-reference"
TRACE=R/"route31nativeinputreplaytrace.actor-trace.json.gz"
CAPTURE=R/"route31nativeinputreplaytrace.route-follow.json"
CONTACT=R/"route31nativeinputreplaytrace-traffic-contact-audit.json"
BOUNDARY=R/"route31-controller-sample-boundaries.json"
FIXTURE=R/"route31pickupstate516.ewram-vblank-00516.bin"
IWRAM=R/"route31pickupstate516.03000000.bin"
BIN=ROOT/"bin/route31-source-contact-native-probe.exe"
OUT=R/"route31-source-contact-native-probe.json"
ROAD=ROOT/"assets/traffic/city1-road-133.ctrp"
ROM="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"

def actor_words(row,slot):
    raw=bytes.fromhex(row["traffic_actor_records"]["raw"])[slot*92:(slot+1)*92]
    assert len(raw)==92
    def u(o): return struct.unpack_from("<I",raw,o)[0]
    def s(o): return struct.unpack_from("<i",raw,o)[0]
    return {"flags":u(0),"type":u(4),"countdown":s(0x10),"x":s(0x14),"y":s(0x18),"z":s(0x1c),"heading":s(0x20),
      "collision_timer":s(0x58),"impulse_x":s(0x40),"impulse_z":s(0x48),"offset_x":s(0x4c),"offset_z":s(0x54),
      "path_distance":s(0x28),"path_segment":u(0x2c),"path_x":s(0x34),"path_z":s(0x38),"path_heading":u(0x3c),"route":133}

def globals(row):
    g=row["traffic_globals"]
    return {"cooldown":g["cooldown_e4"],"near":g["near_miss_25d3"],"voice":g["voice_25d6"],"combo_flag":g["combo_flag_9aa6"],
      "combo_whole":g["combo_9aa8"],"combo_fraction":g["combo_9aac"],"rng":g["rng_3b04"]}

data=TRACE.read_bytes(); trace_hash=hashlib.sha256(data).hexdigest()
cap=json.loads(CAPTURE.read_text()); audit=json.loads(CONTACT.read_text()); boundary=json.loads(BOUNDARY.read_text())
assert cap["rom_sha256"]==ROM and cap["route_source"]==31 and cap["frames"]==12000
assert audit["actor_trace_sha256"]==trace_hash and boundary["trace_sha256"]==trace_hash
contact=next(e for e in audit["traffic_contact_events"] if e["tick"]==1412 and e["slot"]==9)
assert contact["path_asset_index"]==133
rows=json.loads(gzip.decompress(data)); assert len(rows)==12000
before,after=rows[1411],rows[1412]
assert before["previous"]!=after["previous"] and after["trip_clock_state"]["trip_active"]==1
ab=actor_words(before,9); ag=globals(before); expected_actor=actor_words(after,9); expected_g=globals(after)
assert ab["route"]==133 and ab["path_distance"]==105751 and expected_actor["path_distance"]==105756
keys={0:1,6:5,7:4,8:0}; last=rows[516]["previous"]; lines=[]
for tick in range(517,1413):
    row=rows[tick]; bits=0
    for key in row["input_keys"]: bits|=1<<keys[key]
    boundary_tick=int(row["previous"]!=last); last=row["previous"]
    lines.append(f"{0x3ff^bits} {row['delta']} {row['frame_delta']} {boundary_tick} {tick} {int(tick==1412)}\n")
    if tick==1412:
        payload=[ab[k] for k in ("flags","type","countdown","x","y","z","heading","route","collision_timer","impulse_x","impulse_z","offset_x","offset_z","path_distance","path_segment","path_x","path_z","path_heading")]
        payload += [ag[k] for k in ("cooldown","near","voice","combo_flag","combo_whole","combo_fraction","rng")]+[1,1]
        lines.append(" ".join(map(str,payload))+"\n")
proc=subprocess.run([str(BIN),str(FIXTURE),str(IWRAM),str(ROAD)],input="".join(lines),text=True,capture_output=True,cwd=ROOT.parent.parent,timeout=60,check=True)
contact_line=next((line.split() for line in proc.stdout.splitlines() if line.startswith("C ")),None)
vehicle_line=next((line.split() for line in proc.stdout.splitlines() if line.startswith("V 1412 ")),None)
assert contact_line and vehicle_line, proc.stdout[-1000:]
assert contact_line[1]=="1412" and vehicle_line[1]=="1412"
# C line: tick, rc, collision, speed, gear, then all traffic actor and traffic-global fields.
vals=list(map(int,contact_line[1:])); names=("tick","rc","collision","speed","gear","flags","type","countdown","x","y","z","heading","route","collision_timer","impulse_x","impulse_z","offset_x","offset_z","path_distance","path_segment","path_x","path_z","path_heading","cooldown","near","voice","combo_flag","combo_whole","combo_fraction","rng")
actual=dict(zip(names,vals)); actual_actor={k:actual[k] for k in expected_actor}
actual_globals={k:actual[v] for k,v in {"cooldown":"cooldown","near":"near","voice":"voice","combo_flag":"combo_flag","combo_whole":"combo_whole","combo_fraction":"combo_fraction","rng":"rng"}.items()}
sv=rows[1412]
vehicle={"position":[int(vehicle_line[3]),int(vehicle_line[4]),int(vehicle_line[5])],"heading":int(vehicle_line[6]),"speed":int(vehicle_line[7])}
expected_vehicle={"position":sv["position"],"heading":sv["heading"],"speed":sv["speed"]}
report={"status":"PASS_SOURCE_SLOT9_CONTACT_NATIVE_REPLAY" if actual_actor==expected_actor and actual_globals==expected_g and vehicle==expected_vehicle and actual["collision"]==1 else "MISMATCH_SOURCE_SLOT9_CONTACT_NATIVE_REPLAY",
 "rom_sha256":ROM,"trace_sha256":trace_hash,"tick":1412,"source_actor_slot":9,"source_route_asset":133,"source_delta":after["delta"],"source_frame_delta":after["frame_delta"],
 "native_contact_triggered":bool(actual["collision"]),"vehicle_actual":vehicle,"vehicle_expected":expected_vehicle,
 "actor_actual":actual_actor,"actor_expected":expected_actor,"traffic_globals_actual":actual_globals,"traffic_globals_expected":expected_g,
 "component_scope":"Single captured contact after a source-aligned native driving replay from pickup state: taxi_traffic_actor_follow receives the source pre-contact actor/global state and source tick's variable delta. It does not replay the other 49 actors, dispatcher/owner cadence, passenger/fare updates, subsequent contacts, or prove a full route."}
OUT.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
print(json.dumps(report,indent=2))

