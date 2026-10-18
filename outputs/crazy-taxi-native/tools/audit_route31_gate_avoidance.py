#!/usr/bin/env python3
"""Compare exact-path traffic-gate test-driver settings on the same source state."""
import gzip,hashlib,json,math,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];R=ROOT/'reports/original-core-reference'
CASES={'baseline':'route31radius4k','margin2':'route31gateavoid2','margin3_hold12':'route31gateavoid','margin3_hold6':'route31gate3hold6','margin3_hold18':'route31gate3hold18','margin3_25_hold18':'route31gate325','margin3_5_hold18':'route31gate35','margin4':'route31gateavoid4'}
common=None;results={}
for key,label in CASES.items():
 mp=R/f'{label}.route-follow.json';tp=R/(f'{label}.actor-trace.json' if (R/f'{label}.actor-trace.json').exists() else f'{label}.actor-trace.json.gz')
 meta=json.loads(mp.read_text());trace=json.loads(tp.read_text()) if tp.suffix=='.json' else json.loads(gzip.open(tp,'rt',encoding='utf-8').read())
 signature=(meta['rom_sha256'],meta['core_sha256'],meta['state_sha256'])
 if common is None:common=signature
 assert signature==common,(key,signature,common)
 assert meta['frames']==16000 and meta['first_trip_tick']==516 and meta['completed_count']==0 and meta['fare']==[0,0]
 assert all('traffic_actor_records' in row for row in trace)
 waypoints={};prev_wp=None;active=[];contacts=[];prior=bytes.fromhex(trace[0]['traffic_actor_records']['raw'])
 for tick,row in enumerate(trace):
  d=row['route_driver'];wp=d['waypoint']
  if wp!=prev_wp:waypoints.setdefault(wp,tick);prev_wp=wp
  if d['trip_active']:active.append(row)
  if tick:
   raw=bytes.fromhex(row['traffic_actor_records']['raw'])
   if d['trip_active'] and row['speed']<trace[tick-1]['speed']:
    for slot in range(50):
     off=slot*0x5c;f0=struct.unpack_from('<I',prior,off)[0];f1=struct.unpack_from('<I',raw,off)[0]
     if f0&1 and not f1&1 and struct.unpack_from('<i',raw,off+0x58)[0]==120 and (struct.unpack_from('<i',raw,off+0x40)[0] or struct.unpack_from('<i',raw,off+0x48)[0]):contacts.append((tick,slot))
   prior=raw
 assert active
 last=active[-1];distance=round(math.dist([last['position'][0],last['position'][2]],[-257694,274082]))
 gate_events=trace[-1]['route_driver'].get('traffic_gate_events',0)
 seg_start=waypoints.get(10);seg_end=waypoints.get(11)
 results[key]={'label':label,'capture_report_sha256':hashlib.sha256(mp.read_bytes()).hexdigest(),'actor_trace_sha256':hashlib.sha256(tp.read_bytes()).hexdigest(),'gate_enabled':bool(meta.get('route_avoid_collision_gate',False)),'gate_margin':meta.get('route_gate_margin'),'gate_hold':meta.get('route_gate_hold'),'final_waypoint':meta['final_waypoint'],'completed_count':meta['completed_count'],'fare':meta['fare'],'gate_steering_events':gate_events,'traffic_contact_signatures_during_trip':len(contacts),'last_active_tick':last['tick'],'clock_at_last_active':last.get('remaining_clock'),'distance_to_destination_at_expiry':distance,'waypoint10_tick':seg_start,'waypoint11_tick':seg_end,'waypoint10_to_11_ticks':seg_end-seg_start if seg_start is not None and seg_end is not None else None,'contacts_on_waypoint10_to_11':sum(seg_start<=t<=seg_end for t,_ in contacts) if seg_start is not None and seg_end is not None else None}
assert results['baseline']['final_waypoint']==21
assert results['margin3_hold12']['final_waypoint']==25 and results['margin3_hold18']['final_waypoint']==25
assert results['margin3_hold6']['final_waypoint']==20 and results['margin2']['final_waypoint']==21
assert results['margin3_25_hold18']['final_waypoint']==16 and results['margin3_5_hold18']['final_waypoint']==20 and results['margin4']['final_waypoint']==15
assert results['margin3_hold18']['gate_steering_events']<results['margin3_hold12']['gate_steering_events']
report={'status':'PASS_EXACT_PATH_GATE_POLICY_SWEEP_NO_FARE','common_rom_sha256':common[0],'common_core_sha256':common[1],'common_saved_state_sha256':common[2],'shared_route_policy':{'route_source':31,'route_start_waypoint':0,'radius':4096,'lookahead':8192,'speed_divisor':16,'brake_threshold':200000,'steer_during_brake':True},'results':results,'finding':'A path-aware steering probe at margin3 reached waypoint25 and reduced waypoint10→11 traversal from the baseline 2666 to1718 frames, with contact signatures on that segment dropping from6 to2. Margin2/4 and fractional thresholds3.25/3.5 regressed to waypoints21/15/16/20; margin3 hold6 also regressed to20. Hold18 preserved waypoint25 and reduced steering events from70 to53. The best run still expired 46,293 world units from destination27 with completed_count0/fare0. This is evidence that a narrow source-path-informed input schedule can improve route timing, not proof of original-human control or native gameplay parity.', 'scope':'Same supplied ROM, mGBA core, serialized state, route path and vehicle controller; only diagnostic input-policy parameters varied. Source movement, traffic, collision response and clocks remained unchanged. No run completed a fare.'}
(R/'route31-gate-aware-avoidance-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['status'])
for k,v in results.items():print(k,'wp',v['final_waypoint'],'contacts',v['traffic_contact_signatures_during_trip'],'wp10-11',v['waypoint10_to_11_ticks'],'gate events',v['gate_steering_events'],'dest distance',v['distance_to_destination_at_expiry'])
