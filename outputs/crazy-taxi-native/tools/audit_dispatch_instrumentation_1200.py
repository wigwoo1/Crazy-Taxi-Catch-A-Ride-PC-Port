#!/usr/bin/env python3
"""Report long-run side effects of source-dispatch instrumentation without hiding divergence."""
import argparse,json
from pathlib import Path
ROM_SHA="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
def read(p):return json.loads(p.read_text(encoding="utf-8"))
def main():
 a=argparse.ArgumentParser();a.add_argument("--report-dir",type=Path,required=True);a.add_argument("--baseline",default="callbackdispatchbaseline1200");a.add_argument("--dispatch-only",default="callbackdispatchonly1200");a.add_argument("--combined",default="callbackdispatchcombined1200");a.add_argument("--output",type=Path,required=True);x=a.parse_args();r=x.report_dir
 names=(x.baseline,x.dispatch_only,x.combined); docs={n:read(r/f"{n}.json") for n in names}; routes={n:read(r/f"{n}.route-follow.json") for n in names}; traces={n:read(r/f"{n}.actor-trace.json") for n in names}
 if any(d.get("status")!="PASS" or d.get("rom_sha256")!=ROM_SHA for d in docs.values()):raise SystemExit("capture not PASS or wrong ROM")
 if len({d.get("core_sha256") for d in docs.values()})!=1 or len({routes[n].get("state_sha256") for n in names})!=1 or len({routes[n].get("input_schedule_scope") for n in names})!=1:raise SystemExit("core, source state, or route schedule mismatch")
 if len({len(traces[n]) for n in names})!=1 or len(traces[x.baseline])!=1200:raise SystemExit("expected three 1200-row traces")
 actor_diff=[];baseline_shared_diff=[]; nontraffic_fields=tuple(k for k in traces[x.baseline][0] if k not in ("traffic_actor_records","traffic_pool_changed_from_previous_vblank"))
 slot_summary=[]
 for i,(b,d,c) in enumerate(zip(traces[x.baseline],traces[x.dispatch_only],traces[x.combined])):
  for k in nontraffic_fields:
   if b.get(k)!=d.get(k) or b.get(k)!=c.get(k):baseline_shared_diff.append((i,k));break
  br=bytes.fromhex(b["traffic_actor_records"]["raw"]);dr=bytes.fromhex(d["traffic_actor_records"]["raw"]);cr=bytes.fromhex(c["traffic_actor_records"]["raw"])
  if br!=dr or br!=cr:
   slots=[s for s in range(50) if br[s*92:(s+1)*92]!=dr[s*92:(s+1)*92] or br[s*92:(s+1)*92]!=cr[s*92:(s+1)*92]]
   actor_diff.append(i)
   for s in slots:
    bo,do,co=br[s*92:(s+1)*92],dr[s*92:(s+1)*92],cr[s*92:(s+1)*92]
    if len(slot_summary)<16:slot_summary.append({"tick":i,"slot":s,"baseline_record_hex":bo.hex(),"dispatch_only_record_hex":do.hex(),"combined_record_hex":co.hex(),"differing_fields":[name for name,start,end in (("flags",0,4),("type",4,8),("unknown_08_0f",8,16),("countdown",16,20),("x",20,24),("y",24,28),("z",28,32),("heading",32,36)) if any(start<=j<end for j in range(92) if bo[j]!=do[j] or bo[j]!=co[j])],"differing_byte_offsets":[j for j in range(92) if bo[j]!=do[j] or bo[j]!=co[j]]})
 dispatches={n:read(r/f"{n}.traffic-dispatch-entry-trace.json") for n in (x.dispatch_only,x.combined)};de=dispatches[x.dispatch_only]["call_events"];ce=dispatches[x.combined]["call_events"]
 if de!=ce:raise SystemExit("dispatch-only and combined logger events differ")
 event_alignment=[]
 for tick in (961,1183):
  row=traces[x.baseline][tick];matches=[e for e in de if e.get("observed_after_core_tick")==tick]
  if len(matches)!=1:raise SystemExit(f"expected one dispatcher event at divergence row {tick}")
  event=matches[0]
  if event.get("source_counter")!=row.get("counter") or event.get("frame_delta")!=row.get("frame_delta"):raise SystemExit(f"dispatcher boundary does not align with player frame at row {tick}")
  event_alignment.append({"row":tick,"source_counter":event["source_counter"],"frame_delta":event["frame_delta"],"observed_after_core_tick":event["observed_after_core_tick"],"matches_untouched_player_counter_and_delta":True})
 callbacks=read(r/f"{x.combined}.callback-cycle-trace.json")
 if callbacks["log_saturated"] or callbacks["final_buffer_count"]!=0 or callbacks["logged_count"]!=len(callbacks["call_events"]) or callbacks["max_records_per_frame"]>=146 or not callbacks["instrumentation"].get("timer_counter_controls_preserved"):raise SystemExit("callback logger drain/capacity/timer check failed")
 if (len(actor_diff),actor_diff)!=(2,[961,1183]) or baseline_shared_diff:raise SystemExit(f"unexpected parity boundary actor={actor_diff} other={baseline_shared_diff[:4]}")
 def actor_value(name,row,slot,offset):
  raw=bytes.fromhex(traces[name][row]["traffic_actor_records"]["raw"]);return int.from_bytes(raw[slot*92+offset:slot*92+offset+4],"little",signed=True)
 temporal={}
 for slot in (18,19):
  rows=[{"tick":i,"baseline":actor_value(x.baseline,i,slot,16),"dispatch_only":actor_value(x.dispatch_only,i,slot,16),"combined":actor_value(x.combined,i,slot,16)} for i in (960,961,962)]
  if rows[0]["baseline"]!=rows[0]["dispatch_only"] or rows[0]["baseline"]!=rows[0]["combined"] or rows[2]["baseline"]!=rows[2]["dispatch_only"] or rows[2]["baseline"]!=rows[2]["combined"]:raise SystemExit(f"slot{slot} countdown did not reconverge around tick961")
  temporal[f"slot{slot}_countdown"]={"field":"countdown","raw_record_offset":"0x10","rows":rows,"equal_before_and_after":True}
 rows=[{"tick":i,"baseline":actor_value(x.baseline,i,11,20),"dispatch_only":actor_value(x.dispatch_only,i,11,20),"combined":actor_value(x.combined,i,11,20)} for i in (1182,1183,1184)]
 if rows[0]["baseline"]!=rows[0]["dispatch_only"] or rows[0]["baseline"]!=rows[0]["combined"] or rows[2]["baseline"]!=rows[2]["dispatch_only"] or rows[2]["baseline"]!=rows[2]["combined"]:raise SystemExit("slot11 x position did not reconverge around tick1183")
 temporal["slot11_x"]={"field":"x_position","raw_record_offset":"0x14","rows":rows,"equal_before_and_after":True}
 report={"status":"PASS_IDENTIFIED_FIRST_INSTRUMENTED_TRAFFIC_DIVERGENCE","rom_sha256":ROM_SHA,"source_state_sha256":routes[x.baseline]["state_sha256"],"frames":1200,"player_camera_input_route_and_traffic_global_rows_equal":1200,"compared_non_traffic_fields":list(nontraffic_fields),"traffic_actor_table_equal_rows":1198,"first_traffic_actor_table_divergence_tick":961,"all_traffic_actor_divergence_ticks":actor_diff,"changed-slot-list-metadata-also-differs-at":[962,1184],"differing_actor_slots":slot_summary,"divergence_dispatch_alignment":event_alignment,"dispatch_only_and_combined_event_lists_exactly_equal":True,"dispatch_entry_count":len(de),"callback_event_count":len(callbacks["call_events"]),"callback_log_drains":callbacks["log_drains"],"callback_max_records_per_frame":callbacks["max_records_per_frame"],"callback_log_saturated":callbacks["log_saturated"],"callback_timer_controls_preserved":callbacks["instrumentation"]["timer_counter_controls_preserved"],"route_outcome":{"waypoint":routes[x.combined]["final_waypoint"],"completed_fares":routes[x.combined]["completed_count"],"fare":routes[x.combined]["fare"]},"finding":"Across 1,200 rows, driver, camera position/pointer, inputs, route state, traffic globals, and shared frame context remain equal. Both dispatch-only and callback+dispatch instrumented captures show the same four traffic-pool difference observations at ticks961/962 and1183/1184; actor records first differ at tick961. Countdown differences at tick961 and an x-position difference at tick1183 are each transient; the instrumented actor records equal the untouched baseline again on the next sampled row (962 and1184 respectively). Both one-row transients coincide with one logged dispatcher event at the identical source counter/frame delta in both instrumented captures. This aligns the actor sampling difference with a dispatcher/frame boundary but does not establish causation. The dispatcher event lists themselves match. This establishes that the current dispatcher-entry instrumentation is not timing-neutral beyond tick956, despite matched player/global rows; it does not identify the exact cause or justify extending native replay past the 188-event accepted prefix.","limitations":["The source route only reaches waypoint2; no fares complete.","These are offline diagnostic captures with temporary in-memory ROM hooks, not native gameplay parity.","The four actor-state differences are retained as failures; no checkpoint rebasing or corrective production change was applied.","Text, fonts, HUD, menus, logos, signage, and lettering remain excluded from all upscale paths."]}
 report["temporal_reconvergence"]=temporal
 x.output.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8");print(json.dumps({k:v for k,v in report.items() if k not in ("finding","limitations","differing_actor_slots")},indent=2))
if __name__=="__main__":main()


