#!/usr/bin/env python3
"""Compare a reduced dispatcher hook with the 1,200-frame untouched replay."""
import argparse,json
from pathlib import Path
ROM_SHA="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
def read(p):return json.loads(p.read_text(encoding="utf-8"))
def main():
 p=argparse.ArgumentParser();p.add_argument('--report-dir',type=Path,required=True);p.add_argument('--baseline',default='callbackdispatchbaseline1200');p.add_argument('--minimal',default='callbackdispatchminimal1200');p.add_argument('--full',default='callbackdispatchonly1200');p.add_argument('--output',type=Path,required=True);a=p.parse_args();root=a.report_dir
 names=(a.baseline,a.minimal,a.full);docs={n:read(root/f'{n}.json') for n in names};routes={n:read(root/f'{n}.route-follow.json') for n in names};traces={n:read(root/f'{n}.actor-trace.json') for n in names}
 if any(d.get('status')!='PASS' or d.get('rom_sha256')!=ROM_SHA for d in docs.values()):raise SystemExit('capture status or ROM hash invalid')
 if len({d.get('core_sha256') for d in docs.values()})!=1 or len({routes[n].get('state_sha256') for n in names})!=1 or len({routes[n].get('input_schedule_scope') for n in names})!=1:raise SystemExit('source state/core/input mismatch')
 if any(len(traces[n])!=1200 for n in names):raise SystemExit('expected 1200 rows per capture')
 baseline=traces[a.baseline];minimal=traces[a.minimal];nontraffic=tuple(k for k in baseline[0] if k not in ('traffic_actor_records','traffic_pool_changed_from_previous_vblank'));other_diff=[];pool_diff=[]
 for i,(b,m) in enumerate(zip(baseline,minimal)):
  if any(b.get(k)!=m.get(k) for k in nontraffic):other_diff.append(i)
  if b['traffic_actor_records']['raw']!=m['traffic_actor_records']['raw']:pool_diff.append(i)
 minimal_doc=read(root/f'{a.minimal}.traffic-dispatch-entry-trace.json');full_doc=read(root/f'{a.full}.traffic-dispatch-entry-trace.json');me=minimal_doc['call_events'];fe=full_doc['call_events']
 key=lambda e:(e['source_counter'],e['frame_delta'],e['observed_after_core_tick'])
 if [key(e) for e in me]!=[key(e) for e in fe]:raise SystemExit('minimal and full dispatcher event tuples differ')
 if pool_diff!=[961,1183] or other_diff or len(me)!=236:raise SystemExit(f'unexpected comparison boundary pool={pool_diff} other={other_diff[:3]} events={len(me)}')
 event_alignment=[]
 for tick in (961,1183):
  row=baseline[tick];matches=[e for e in me if e.get("observed_after_core_tick")==tick]
  if len(matches)!=1:raise SystemExit(f"expected one dispatcher event at divergence row {tick}")
  event=matches[0]
  if event.get("source_counter")!=row.get("counter") or event.get("frame_delta")!=row.get("frame_delta"):raise SystemExit(f"dispatcher boundary does not align with player frame at row {tick}")
  event_alignment.append({"row":tick,"source_counter":event["source_counter"],"frame_delta":event["frame_delta"],"observed_after_core_tick":event["observed_after_core_tick"],"matches_untouched_player_counter_and_delta":True})
 differences=[]
 for i in pool_diff:
  b=bytes.fromhex(baseline[i]['traffic_actor_records']['raw']);m=bytes.fromhex(minimal[i]['traffic_actor_records']['raw'])
  for slot in range(50):
   x,y=b[slot*92:(slot+1)*92],m[slot*92:(slot+1)*92]
   if x!=y:differences.append({'tick':i,'slot':slot,'differing_offsets':[j for j in range(92) if x[j]!=y[j]],'differing_fields':[name for name,start,end in (("flags",0,4),("type",4,8),("unknown_08_0f",8,16),("countdown",16,20),("x",20,24),("y",24,28),("z",28,32),("heading",32,36)) if any(start<=j<end for j in range(92) if x[j]!=y[j])],'baseline_hex':x.hex(),'minimal_hex':y.hex()})
 def actor_value(name,row,slot,offset):
  raw=bytes.fromhex(traces[name][row]["traffic_actor_records"]["raw"]);return int.from_bytes(raw[slot*92+offset:slot*92+offset+4],"little",signed=True)
 temporal={}
 for slot in (18,19):
  rows=[{"tick":i,"baseline":actor_value(a.baseline,i,slot,16),"minimal":actor_value(a.minimal,i,slot,16),"full":actor_value(a.full,i,slot,16)} for i in (960,961,962)]
  if rows[0]["baseline"]!=rows[0]["minimal"] or rows[2]["baseline"]!=rows[2]["minimal"]:raise SystemExit(f"slot{slot} countdown did not reconverge around tick961")
  temporal[f"slot{slot}_countdown"]={"field":"countdown","raw_record_offset":"0x10","rows":rows,"equal_before_and_after":True}
 rows=[{"tick":i,"baseline":actor_value(a.baseline,i,11,20),"minimal":actor_value(a.minimal,i,11,20),"full":actor_value(a.full,i,11,20)} for i in (1182,1183,1184)]
 if rows[0]["baseline"]!=rows[0]["minimal"] or rows[2]["baseline"]!=rows[2]["minimal"]:raise SystemExit("slot11 x position did not reconverge around tick1183")
 temporal["slot11_x"]={"field":"x_position","raw_record_offset":"0x14","rows":rows,"equal_before_and_after":True}
 report={'status':'PASS_MINIMAL_PROBE_ASSESSMENT_RETAINS_PARITY_FAILURE','rom_sha256':ROM_SHA,'source_state_sha256':routes[a.minimal]['state_sha256'],'frames':1200,'stub_bytes':60,'previous_stub_bytes':136,'record_bytes':8,'previous_record_bytes':11,'dispatcher_event_tuples_match_previous_probe':len(me),'player_camera_input_route_and_global_rows_equal':1200,'actor_pool_equal_rows':1198,'actor_pool_divergence_ticks':pool_diff,'nontraffic_divergence_ticks':other_diff,'actor_differences':differences,'divergence_dispatch_alignment':event_alignment,'route_outcome':{'waypoint':routes[a.minimal]['final_waypoint'],'fares':routes[a.minimal]['completed_count'],'fare':routes[a.minimal]['fare']},'finding':'Reducing the entry hook from136 to60 bytes and each log record from11 to8 bytes does not remove the first full actor-table divergence: ticks961 and1183 remain identical to the prior dispatcher probe, while all player/camera/input/route/global fields match. Thus smaller code alone is insufficient to treat the later 48 events as timing-neutral. No replay acceptance was extended; the boundary remains188 events through tick956. The raw-field mapping identifies the transients as countdown at offset0x10 and x position at0x14; each actor record reconverges with the untouched baseline on the next sampled row. Each mismatch coincides with the logged dispatcher event at the same counter and frame delta in all hooked traces, which aligns the drift with a dispatcher/frame boundary but does not prove causation.','limitations':['Instrumented source route reaches waypoint2 and completes zero fares.','This temporary hook is an offline diagnostic, not native gameplay parity.','Supplied ROM was only read and its hash is unchanged.']}
 report["temporal_reconvergence"]=temporal
 a.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8');print(json.dumps({k:v for k,v in report.items() if k not in ('finding','limitations','actor_differences')},indent=2))
if __name__=='__main__':main()
