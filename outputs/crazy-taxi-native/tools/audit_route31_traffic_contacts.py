#!/usr/bin/env python3
"""Verify route31 traffic paths and collision-gate math at sampled contacts."""
import gzip,hashlib,json,math,struct,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]; R=ROOT/'reports/original-core-reference'
label=sys.argv[1] if len(sys.argv)>1 else 'route31radius4k'; rp=R/f'{label}.route-follow.json'; tp=R/f'{label}.actor-trace.json'
if not tp.exists() and tp.with_suffix(tp.suffix+'.gz').exists():tp=tp.with_suffix(tp.suffix+'.gz')
roads_path=ROOT/'assets/traffic/city1-roads.json'
meta=json.loads(rp.read_text());trace=json.loads(gzip.open(tp,'rt',encoding='utf-8').read()) if tp.suffix=='.gz' else json.loads(tp.read_text());roads=json.loads(roads_path.read_text())
assert len(trace)==meta['frames'] and len(trace)>1 and meta['traffic_actor_trace'] is True
assert meta['rom_sha256']==roads['rom_sha256']=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
descriptors={int(item['descriptor_address'],16):item for item in roads['paths']};cache={}
def signed32(value):
 value &= 0xffffffff
 return value-0x100000000 if value&0x80000000 else value
def load_path(desc):
 if desc in cache:return cache[desc]
 item=descriptors[desc];path=ROOT/'assets/traffic'/f"city1-road-{item['index']}.ctrp";raw=path.read_bytes()
 assert hashlib.sha256(raw[8:]).hexdigest()==item['sha256']
 assert raw[:4]==b'CTP1';count=struct.unpack_from('<I',raw,4)[0];assert count==item['count'] and len(raw)==8+count*16
 pts=[struct.unpack_from('<IiiI',raw,8+i*16) for i in range(count)]
 assert pts[0][0]==0 and pts[-1][0]==item['length']
 cache[desc]=(item,pts,path);return cache[desc]
def path_at(desc,distance):
 item,pts,_=load_path(desc);d=distance%item['length'];i=0
 while i+1<len(pts) and pts[i+1][0]<=d:i+=1
 assert i+1<len(pts)
 d0,x0,z0,_=pts[i];d1,x1,z1,_=pts[i+1];span=d1-d0;offset=d-d0
 xx=signed32(x0+int(signed32((x1-x0)*offset)/span));zz=signed32(z0+int(signed32((z1-z0)*offset)/span))
 return xx,zz
def collision_gate(path_x,path_z,player_x,player_z,speed):
 radius=min(speed//0x472,200)*192//200+384
 dx=signed32(path_x-player_x);dz=signed32(path_z-player_z);squared=signed32(dx*dx+dz*dz);limit=signed32(radius*radius)
 return abs(dx)<2*radius and abs(dz)<2*radius and squared//256<=limit//256,radius,dx,dz

window=(3657,6323) if label=='route31radius4k' else None
events=[];prior=bytes.fromhex(trace[0]['traffic_actor_records']['raw'])
for tick,row in enumerate(trace[1:],1):
 raw=bytes.fromhex(row['traffic_actor_records']['raw'])
 if (window and window[0]<=tick<=window[1]) or (window is None and row['route_driver']['trip_active']):
  bp=trace[tick-1];px,pz=row['position'][0],row['position'][2]
  for slot in range(50):
   off=slot*0x5c;flags0=struct.unpack_from('<I',prior,off)[0];flags1=struct.unpack_from('<I',raw,off)[0]
   if not(flags0&1 and not flags1&1):continue
   desc0=struct.unpack_from('<I',prior,off+0x24)[0];desc1=struct.unpack_from('<I',raw,off+0x24)[0]
   if not desc0 or desc0!=desc1:continue
   d0=struct.unpack_from('<i',prior,off+0x28)[0];d1=struct.unpack_from('<i',raw,off+0x28)[0];delta=row['frame_delta']
   item,_,_=load_path(desc1);assert d1%item['length']==(d0+delta)%item['length'],(tick,slot,d0,d1,delta)
   path_x,path_z=struct.unpack_from('<ii',raw,off+0x34);assert path_at(desc1,d1)==(path_x,path_z),(tick,slot)
   hit_pre,rpre,dx,dz=collision_gate(path_x,path_z,px,pz,bp['speed']);hit_post,rpost,_,_=collision_gate(path_x,path_z,px,pz,row['speed'])
   impulse=(struct.unpack_from('<i',raw,off+0x40)[0],struct.unpack_from('<i',raw,off+0x48)[0]);timer=struct.unpack_from('<i',raw,off+0x58)[0]
   assert hit_pre and timer==120 and (impulse[0] or impulse[1]) and row['speed']<bp['speed'],(tick,slot,hit_pre,timer,impulse)
   events.append({'tick':tick,'slot':slot,'descriptor':hex(desc1),'path_asset_index':item['index'],'frame_delta':delta,'route_distance_before':d0,'route_distance_after':d1,'predicted_path_xz':[path_x,path_z],'taxi_after_xz':[px,pz],'collision_gate_dx_dz':[dx,dz],'gate_radius_at_preimpact_speed':rpre,'gate_at_preimpact_speed':hit_pre,'gate_radius_at_postimpact_speed':rpost,'gate_at_postimpact_speed':hit_post,'actor_bit0':[1,0],'impulse_after':impulse,'collision_timer_after':timer,'player_speed_before':bp['speed'],'player_speed_after':row['speed'],'input_keys':row.get('input_keys'),'route_waypoint':row['route_driver']['waypoint']})
 prior=raw
assert len(events)==6 if window else len(events)>0,len(events)
report={'status':'PASS_ROUTE31_TRAFFIC_PATH_AND_COLLISION_GATE_REPLAY','rom_sha256':meta['rom_sha256'],'core_sha256':meta['core_sha256'],'state_sha256':meta['state_sha256'],'capture_report_sha256':hashlib.sha256(rp.read_bytes()).hexdigest(),'actor_trace_sha256':hashlib.sha256(tp.read_bytes()).hexdigest(),'traffic_asset_manifest_sha256':hashlib.sha256(roads_path.read_bytes()).hexdigest(),'interval_ticks':list(window) if window else 'active_trip','traffic_contact_events':events,'path_advance_match_count':len(events),'collision_gate_match_count_at_preimpact_speed':sum(e['gate_at_preimpact_speed'] for e in events),'scope':'All detected events advance the source road path by the captured frame_delta, reproduce exact path x/z at the saved route distance, and satisfy recovered collision-gate arithmetic against the post-movement taxi using preimpact speed. The actor clears active bit0, sets impulse and timer120, and taxi speed drops on the same sampled transition. This verifies sampled state consistency with recovered path/collision routines; it does not prove subframe dispatcher order or a completed fare.'}
out=R/('route31-traffic-contact-audit.json' if label=='route31radius4k' else f'{label}-traffic-contact-audit.json');out.write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],'path matches',report['path_advance_match_count'],'gate matches',report['collision_gate_match_count_at_preimpact_speed'])
for e in events:print(e['tick'],e['slot'],'dist',round(math.hypot(*e['collision_gate_dx_dz'])),'radius',e['gate_radius_at_preimpact_speed'],'speed',e['player_speed_before'],'->',e['player_speed_after'])
