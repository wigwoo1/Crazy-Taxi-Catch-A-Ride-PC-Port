#!/usr/bin/env python3
"""Iteratively replay native driving and traffic from the source pickup state."""
import gzip,hashlib,json,struct,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]; R=ROOT/"reports/original-core-reference"
TRACE=R/"route31nativeinputreplaytrace.actor-trace.json.gz"; CAP=R/"route31nativeinputreplaytrace.route-follow.json"
AUDIT=R/"route31-controller-sample-boundaries.json"; ROADS=ROOT/"assets/traffic/city1-roads.json"
FIX=R/"route31pickupstate516.ewram-vblank-00516.bin"; IW=R/"route31pickupstate516.03000000.bin"
ACTORS=R/"route31-source-traffic-replay-initial.bin"; CFG=R/"route31-source-traffic-replay-initial.cfg"
BIN=ROOT/"bin/route31-source-traffic-replay-probe.exe"; OUT=R/"route31-source-traffic-replay-to-1412.json"
ROM="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"; KEY_BITS={0:1,6:5,7:4,8:0}
def u32(b,o):return struct.unpack_from("<I",b,o)[0]
def s32(b,o):return struct.unpack_from("<i",b,o)[0]
def actor_table(row,descmap):
    b=bytes.fromhex(row["traffic_actor_records"]["raw"]); assert len(b)==50*92; out=[]
    for i in range(50):
        p=i*92; d=u32(b,p+0x24); route=descmap.get(d,0); assert not d or route,(i,hex(d))
        out.append((u32(b,p),u32(b,p+4),s32(b,p+0x10),s32(b,p+0x14),s32(b,p+0x18),s32(b,p+0x1c),s32(b,p+0x20),route,s32(b,p+0x28),u32(b,p+0x2c),s32(b,p+0x34),s32(b,p+0x38),struct.unpack_from('<H',b,p+0x3c)[0],s32(b,p+0x40),s32(b,p+0x48),s32(b,p+0x4c),s32(b,p+0x54),s32(b,p+0x58)))
    return out
raw=TRACE.read_bytes(); th=hashlib.sha256(raw).hexdigest(); cap=json.loads(CAP.read_text()); aud=json.loads(AUDIT.read_text()); roads=json.loads(ROADS.read_text()); rows=json.loads(gzip.decompress(raw))
assert cap['rom_sha256']==ROM and cap['route_source']==31 and cap['frames']==12000 and aud['trace_sha256']==th
assert FIX.stat().st_size==0x40000 and IW.stat().st_size==0x8000
mem=FIX.read_bytes(); assert mem[0xf4:0xf4+4600]==bytes.fromhex(rows[516]['traffic_actor_records']['raw'])
setting=mem[0x1eb3]; density=(5,11,17,22,27,33,39,44,50,28,26,24,22,20,18,16)[setting>>4]
assert density==27 and setting==68
m={int(x['descriptor_address'],16):int(x['index']) for x in roads['paths']}; initial=actor_table(rows[516],m)
ACTORS.write_bytes(b''.join(struct.pack('<18i',*a) for a in initial))
g=rows[516]['traffic_globals'];ctx=rows[516]['traffic_frame_context'];cam=rows[516]['traffic_camera']['position']
config=[setting,density,g['timer_25d5'],g['selection_counter_4c98'],g['cooldown_e4'],g['near_miss_25d3'],g['voice_25d6'],g['combo_flag_9aa6'],g['combo_9aa8'],g['combo_9aac'],g['rng_3b04'],ctx['sound_7602'],ctx['trip_active_1694'],cam[0],cam[2]]
CFG.write_text(' '.join(map(str,config))+'\n',encoding='ascii')
lines=[]; expected=[]; last=rows[516]['previous']
for tick in range(517,1413):
    row=rows[tick]; bits=0
    for key in row['input_keys']: bits|=1<<KEY_BITS[key]
    boundary=int(row['previous']!=last); last=row['previous']
    cam=row['traffic_camera']['position']; c=row['traffic_frame_context']
    lines.append(f"{0x3ff^bits} {row['delta']} {row['frame_delta']} {boundary} {tick} {cam[0]} {cam[2]} {c['freeze_5f20']} {c['sound_7602']} {c['trip_active_1694']}\n")
    if boundary: expected.append(row)
proc=subprocess.run([str(BIN),str(FIX),str(IW),str(ACTORS),str(CFG)],input=''.join(lines),text=True,capture_output=True,cwd=ROOT.parent.parent,timeout=90,check=True)
actual_by_tick={}; actors_by_tick={}
for line in proc.stdout.splitlines():
    q=line.split()
    if q[0]=='S':
        v=list(map(int,q[1:])); actual_by_tick[v[0]]={'spawned':v[1],'count':v[2],'timer':v[3],'counter':v[4],'cooldown':v[5],'near':v[6],'voice':v[7],'combo_flag':v[8],'combo_whole':v[9],'combo_fraction':v[10],'rng':v[11]}
    elif q[0]=='V':
        v=list(map(int,q[1:])); actual_by_tick.setdefault(v[0],{})['vehicle']={'position':v[1:4],'heading':v[4],'speed':v[5]}
    elif q[0]=='A':
        v=list(map(int,q[1:])); actors_by_tick.setdefault(v[0],[]).append(dict(zip(('index','flags','type','countdown','x','y','z','heading','route','collision_timer','impulse_x','impulse_z','offset_x','offset_z','path_distance','path_segment','path_x','path_z','path_heading'),[v[1],*v[2:]])))
assert len(actual_by_tick)==len(expected),(len(actual_by_tick),len(expected),proc.stderr[-500:])
comparisons=[]; first=None
for row in expected:
    tick=row['tick']; a=actual_by_tick[tick]; source_g=row['traffic_globals']; source_c=row['traffic_frame_context']; srcactors=actor_table(row,m); got=actors_by_tick[tick]
    assert len(got)==50,(tick,len(got))
    expected_v={'position':row['position'],'heading':row['heading'],'speed':row['speed']}
    native_v=a.get('vehicle',{})
    expected_g={'timer':source_g['timer_25d5'],'counter':source_g['selection_counter_4c98'],'cooldown':source_g['cooldown_e4'],'near':source_g['near_miss_25d3'],'voice':source_g['voice_25d6'],'combo_flag':source_g['combo_flag_9aa6'],'combo_whole':source_g['combo_9aa8'],'combo_fraction':source_g['combo_9aac'],'rng':source_g['rng_3b04']}
    gm={k:{'native':a.get(k),'source':v} for k,v in expected_g.items() if a.get(k)!=v}
    vm={} if native_v==expected_v else {'native':native_v,'source':expected_v}
    actor_m=[]
    for i,(actual,tuple_expected) in enumerate(zip(got,srcactors)):
        names=('flags','type','countdown','x','y','z','heading','route','path_distance','path_segment','path_x','path_z','path_heading','impulse_x','impulse_z','offset_x','offset_z','collision_timer')
        exp=dict(zip(names,tuple_expected)); bad={k:{'native':actual[k],'source':exp[k]} for k in names if actual[k]!=exp[k]}
        if bad: actor_m.append({'index':i,'fields':bad})
    item={'tick':tick,'vehicle_mismatches':vm,'global_mismatches':gm,'actor_mismatch_count':len(actor_m),'actor_mismatches':actor_m[:4]}
    comparisons.append(item)
    if vm or gm or actor_m:
        first=item; break
passed=sum(not(x['vehicle_mismatches'] or x['global_mismatches'] or x['actor_mismatch_count']) for x in comparisons)
contact_event=next((e for e in json.loads((R/'route31nativeinputreplaytrace-traffic-contact-audit.json').read_text())['traffic_contact_events'] if e['tick']==1412),None)
report={'status':'PASS_ITERATIVE_NATIVE_TRAFFIC_REPLAY_THROUGH_1412' if first is None else 'FIRST_SOURCE_TRAFFIC_DIVERGENCE_FOUND','rom_sha256':ROM,'trace_sha256':th,'start_tick':516,'end_tick':1412,'active_slots':density,'source_dispatch_updates':len(expected),'exact_prefix_updates':passed,'first_mismatch':first,'source_contact_tick1412':contact_event,'comparisons':comparisons,'scope':'Native driver and complete traffic_frame run iteratively from the source pickup state. The initial 50-slot actor pool is verified byte-for-byte against the source checkpoint and is not reseeded during this replay. Per-VBlank input is sampled once; driving and traffic run at each observed source previous-counter boundary with source deltas. This remains a bounded scripted route diagnostic; passenger delivery/fare and trip completion are not implemented or verified by this run.'}
OUT.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({k:report[k] for k in ('status','start_tick','end_tick','active_slots','source_dispatch_updates','exact_prefix_updates','first_mismatch','scope')},indent=2))
