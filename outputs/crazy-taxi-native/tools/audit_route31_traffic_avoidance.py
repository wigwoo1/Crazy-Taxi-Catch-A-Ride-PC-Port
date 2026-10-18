#!/usr/bin/env python3
"""Compare rejected traffic-steer test schedules against the matched route baseline."""
import gzip,hashlib,json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1];R=ROOT/'reports/original-core-reference'
labels={'baseline':'route31baselineexact','baseline_after_probe':'route31postavoidbase','broad_avoid':'route31avoidtraffic','near_avoid':'route31avoidnear'}
common=None;runs={}
for name,label in labels.items():
 mp=R/f'{label}.route-follow.json';tp=R/f'{label}.actor-trace.json.gz';m=json.loads(mp.read_text())
 signature=(m['rom_sha256'],m['core_sha256'],m['state_sha256'])
 if common is None:common=signature
 assert signature==common,(name,signature,common)
 with gzip.open(tp,'rt',encoding='utf-8') as f:trace=json.load(f)
 assert m['frames']==16000 and m['first_trip_tick']==516 and m['completed_count']==0 and m['fare']==[0,0]
 d=trace[-1]['route_driver']
 runs[name]={'label':label,'report_sha256':hashlib.sha256(mp.read_bytes()).hexdigest(),'trace_sha256':hashlib.sha256(tp.read_bytes()).hexdigest(),'frames':m['frames'],'route_waypoint_radius':m['route_waypoint_radius'],'route_lookahead':m['route_lookahead'],'route_lookahead_speed_divisor':m['route_lookahead_speed_divisor'],'route_brake_speed':m['route_brake_speed'],'route_steer_during_brake':m['route_steer_during_brake'],'avoid_enabled':bool(m.get('route_avoid_traffic',False)),'avoid_distance':m.get('route_avoid_distance',18000 if name=='broad_avoid' else 5000),'avoid_width':m.get('route_avoid_width',5000 if name=='broad_avoid' else 1600),'avoid_hold':m.get('route_avoid_hold',60 if name=='broad_avoid' else 30),'avoid_events':d.get('traffic_avoid_events',0),'final_waypoint':m['final_waypoint'],'completed_count':m['completed_count'],'fare':m['fare'],'remaining_clock':m['remaining_clock']}
assert runs['baseline']['final_waypoint']==21
assert runs['baseline_after_probe']['final_waypoint']==21 and runs['baseline_after_probe']['avoid_events']==0
assert runs['broad_avoid']['final_waypoint']==6 and runs['broad_avoid']['avoid_events']>0
assert runs['near_avoid']['final_waypoint']==10 and runs['near_avoid']['avoid_events']>0
report={'status':'PASS_TRAFFIC_AVOIDANCE_VARIANTS_REJECTED_AS_NONDELIVERY','common_rom_sha256':common[0],'common_core_sha256':common[1],'common_saved_state_sha256':common[2],'shared_vehicle_controller':{'route_source':31,'route_start_waypoint':0,'route_waypoint_radius':4096,'route_lookahead':8192,'route_lookahead_speed_divisor':16,'route_brake_speed':200000,'route_steer_during_brake':True},'runs':runs,'finding':'The fixed-duration lateral steering heuristic is counterproductive in these runs: broad 18k/5k/60 steering reached waypoint6; near 5k/1.6k/30 steering reached waypoint10; both baseline runs reached waypoint21. The no-avoid baseline after adding this optional probe matches the earlier baseline endpoint, so default-off behavior remains intact. All source trips timed out with zero deliveries/fare. Reject these policies for the port; this is controller-probe evidence only, not a source-human input sequence or physics change.','scope':'All captures replay the same supplied ROM, libretro core and serialized start state. Avoidance only changes diagnostic input keys; source movement, collision, timers and traffic remain authoritative.'}
out=R/'route31-traffic-avoidance-audit.json';out.write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],[(k,v['final_waypoint'],v['avoid_events']) for k,v in runs.items()])
