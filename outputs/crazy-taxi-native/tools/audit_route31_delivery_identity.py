#!/usr/bin/env python3
"""Join ROM-derived passenger, route, destination and source-run timeout evidence."""
from __future__ import annotations
import hashlib, json, math, pathlib
ROOT=pathlib.Path(__file__).resolve().parents[1]
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()
def signed32(v):
    v &= 0xffffffff
    return v-0x100000000 if v&0x80000000 else v
def native_route_length(points):
    total=0
    for (ax,az),(bx,bz) in zip(points,points[1:]):
        dx=signed32(bx-ax)>>8; dz=signed32(bz-az)>>8
        root=math.isqrt((dx*dx+dz*dz)&0xffffffff)&0xffff
        total=(total+root)&0xffffffff
    return signed32((total<<8)&0xffffffff)
def main():
    passenger_path=ROOT/'reports/passengers/city1-passengers.json'
    route_path=ROOT/'reports/live-delivery-route31.json'
    trace_path=ROOT/'reports/original-core-reference/route31radius4k.actor-trace.json'
    passengers=json.loads(passenger_path.read_text(encoding='utf-8'))
    route_json=json.loads(route_path.read_text(encoding='utf-8'))
    trace=json.loads(trace_path.read_text(encoding='utf-8'))
    route_id=31
    routes=[r for r in passengers['routes'] if r['index']==route_id]
    matches=[p for p in passengers['pedestrians'] if route_id in p['matching_route_indices']]
    if len(routes)!=1 or len(matches)!=1: raise SystemExit(f'Expected unique route/passenger; got {len(routes)}/{len(matches)}')
    route=routes[0];ped=matches[0];dest=passengers['destinations'][ped['destination_index']]
    points=[row[1:3] for row in route['points_raw']]
    recorded=[list(p) for p in route_json['points']]
    if points!=recorded: raise SystemExit('route JSON differs from extracted CTP passenger route')
    if not trace: raise SystemExit('empty actor trace')
    active=[e for e in trace if e.get('route_driver',{}).get('trip_active')]
    if not active: raise SystemExit('no active trip in original-ROM capture')
    first,last=active[0],active[-1]
    peak=max(trace,key=lambda e:e.get('remaining_clock',-1))
    driver=last.get('route_driver',{})
    endpoint=points[-1];target=dest['target_xz']
    destination_distance=math.isqrt((endpoint[0]-target[0])**2+(endpoint[1]-target[1])**2)
    endpos=driver.get('position',[0,0,0]);remaining_to_dest=math.isqrt((endpos[0]-target[0])**2+(endpos[2]-target[1])**2)
    route_length=native_route_length(points)
    exact=sum(math.dist(a,b) for a,b in zip(points,points[1:]))
    expiry=next((e for e in trace if e.get('remaining_clock')==0 and e.get('route_driver',{}).get('trip_active')),None)
    report={
      'status':'PASS_SOURCE_ROUTE31_IDENTITY_TIMEOUT_AUDIT',
      'source_route_index':route_id,'source_route_name':route['name'],'passenger_index':ped['index'],
      'passenger_kind':ped['kind'],'customer_candidate':ped['customer_candidate'],
      'destination_index':ped['destination_index'],'destination_target_xz':target,
      'destination_accepted_material_slots':dest['accepted_material_slots'],
      'waypoint_count':len(points),'route_start_xz':points[0],'route_end_xz':endpoint,
      'route_end_to_destination_distance_floor':destination_distance,
      'native_source_route_length':route_length,'euclidean_route_length':exact,
      'first_active_trip':{'tick':first['tick'],'clock':first.get('remaining_clock'),'waypoint_target_index':first['route_driver'].get('waypoint')},
      'peak_clock':{'tick':peak['tick'],'clock':peak.get('remaining_clock')},
      'last_active_sample':{'tick':last['tick'],'clock':last.get('remaining_clock'),'waypoint_target_index':driver.get('waypoint'),'position':endpos,'distance_to_destination_floor':remaining_to_dest},
      'first_active_sample_at_clock_zero':None if expiry is None else {'tick':expiry['tick'],'waypoint_target_index':expiry['route_driver'].get('waypoint'),'position':expiry['route_driver'].get('position')},
      'completed_count_at_capture_end':trace[-1].get('completed_count'),'fare_at_capture_end':[trace[-1].get('fare_whole'),trace[-1].get('fare_fraction')],
      'sha256':{'passenger_source_report':sha(passenger_path),'route31_projection':sha(route_path),'original_rom_actor_trace':sha(trace_path)},
      'scope':'Confirms the saved original-ROM route-follow trace corresponds to source passenger record and destination data, and records timeout/progress. It does not establish successful source pickup, arrival, material acceptance, delivery, or native game parity.'
    }
    out=ROOT/'reports/original-core-reference/route31-source-identity-timeout-audit.json'
    out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:report[k] for k in ('status','source_route_name','passenger_index','passenger_kind','destination_index','destination_target_xz','waypoint_count','route_end_to_destination_distance_floor','native_source_route_length','first_active_trip','peak_clock','last_active_sample','first_active_sample_at_clock_zero','completed_count_at_capture_end','fare_at_capture_end')},separators=(',',':')))
if __name__=='__main__': main()


