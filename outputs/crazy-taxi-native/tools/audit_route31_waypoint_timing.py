#!/usr/bin/env python3
"""Audit route31 waypoint timing from a compact original-ROM trace."""
import gzip, hashlib, json, math
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
REPORTS=ROOT/'reports/original-core-reference'
label='route31baselineexact'
summary_path=REPORTS/f'{label}.route-follow.json'
trace_path=REPORTS/f'{label}.actor-trace.json.gz'
route_path=ROOT/'reports/live-delivery-route31.json'
summary=json.loads(summary_path.read_text())
with gzip.open(trace_path,'rt',encoding='utf-8') as f: trace=json.load(f)
route=json.loads(route_path.read_text()); points=route['points']
assert route['source']==summary['route_source']==31 and len(points)==summary['route_points']==27
assert (summary['route_waypoint_radius'],summary['route_lookahead'],summary['route_lookahead_speed_divisor'],summary['route_brake_speed'],summary['route_steer_during_brake'])==(4096,8192,16,200000,True)
def dist(a,b): return math.hypot(a[0]-b[0],a[1]-b[1])
def remaining(xz,waypoint):
 i=min(waypoint,len(points)-1)
 return round(dist(xz,points[i])+sum(dist(a,b) for a,b in zip(points[i:],points[i+1:])))
previous=None; milestones=[]
for row in trace:
 d=row.get('route_driver',{}); wp=d.get('waypoint')
 if wp is None: continue
 if previous is None or wp!=previous:
  pos=row['position']; xz=[pos[0],pos[2]]; target=points[min(wp,len(points)-1)]
  milestones.append({'tick':row['tick'],'waypoint':wp,'trip_active':bool(row.get('trip_clock_state',{}).get('trip_active')),'remaining_clock':row.get('remaining_clock'),'trip_time':row.get('trip_clock_state',{}).get('trip_time'),'position_xz':xz,'speed':row.get('speed'),'input_keys':row.get('input_keys'),'steering_difference':d.get('difference'),'brake':d.get('brake'),'distance_to_waypoint':round(dist(xz,target)),'remaining_route_length':remaining(xz,wp)})
  previous=wp
end=trace[-1]; d=end['route_driver']; xz=[end['position'][0],end['position'][2]]; dest=[-257694,274082]
assert summary['first_trip_tick']==516 and summary['final_waypoint']==21
assert summary['completed_count']==0 and summary['fare']==[0,0]
report={'status':'PASS_ROUTE31_WAYPOINT_TIMING_AUDIT_INCOMPLETE_DELIVERY','capture_label':label,'rom_sha256':summary['rom_sha256'],'core_sha256':summary['core_sha256'],'state_sha256':summary['state_sha256'],'capture_report_sha256':hashlib.sha256(summary_path.read_bytes()).hexdigest(),'actor_trace_sha256':hashlib.sha256(trace_path.read_bytes()).hexdigest(),'route_points_sha256':hashlib.sha256(route_path.read_bytes()).hexdigest(),'pickup_tick':summary['first_trip_tick'],'destination_xz':dest,'milestones_from_waypoint18':[m for m in milestones if m['waypoint']>=18],'timeout':{'tick':end['tick'],'active':bool(end.get('trip_clock_state',{}).get('trip_active')),'remaining_clock':end.get('remaining_clock'),'waypoint':d['waypoint'],'position_xz':xz,'distance_to_destination':round(dist(xz,dest)),'distance_to_current_waypoint':round(dist(xz,points[min(d['waypoint'],len(points)-1)])),'remaining_route_length':remaining(xz,d['waypoint']),'completed_count':end.get('completed_count'),'fare':[end.get('fare_whole'),end.get('fare_fraction')]},'scope':'Waypoint changes are explicitly retained by the compact trace. This is an original-ROM capture from one saved state using a diagnostic adaptive controller, not original-human input or native gameplay proof.'}
out=REPORTS/'route31-waypoint-timing-audit.json'; out.write_text(json.dumps(report,indent=2)+'\n')
print(report['status']); print('waypoint transitions:',[(m['tick'],m['waypoint'],m['remaining_clock']) for m in milestones]); print('timeout:',report['timeout'])
