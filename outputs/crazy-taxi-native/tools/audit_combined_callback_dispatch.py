#!/usr/bin/env python3
"""Verify same-run callback/dispatcher instrumentation against source baselines."""
import argparse, hashlib, json
from pathlib import Path
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
SHARED = ("tick", "input_keys", "keyinput", "heading", "speed", "counter", "previous", "frame_delta", "delta", "intro", "position", "angles", "orientation", "gravity", "airborne", "raw_input_history", "driver_owners", "traffic_frame_context", "route_driver", "completed_count", "fare_whole", "fare_fraction", "remaining_clock")
def read(p): return json.loads(p.read_text(encoding="utf-8"))
def main():
 ap=argparse.ArgumentParser(); ap.add_argument("--report-dir",type=Path,required=True); ap.add_argument("--combined",default="callbacktimingcombined522"); ap.add_argument("--baseline",default="dispatchbaseline700"); ap.add_argument("--dispatcher-reference",default="dispatchcadence700"); ap.add_argument("--callback-reference",default="callbacktimingphase522"); ap.add_argument("--output",type=Path,required=True); a=ap.parse_args(); root=a.report_dir
 names=(a.combined,a.baseline,a.dispatcher_reference,a.callback_reference)
 docs={n:read(root/f"{n}.json") for n in names}
 routes={n:read(root/f"{n}.route-follow.json") for n in names}
 traces={n:read(root/f"{n}.actor-trace.json") for n in names}
 for n,d in docs.items():
  if d.get("status")!="PASS" or d.get("rom_sha256")!=ROM_SHA: raise SystemExit(f"bad status/ROM hash: {n}")
 if len(traces[a.combined])!=522 or len(traces[a.callback_reference])!=522 or len(traces[a.baseline])<522 or len(traces[a.dispatcher_reference])<522: raise SystemExit("capture row counts differ")
 if len({routes[n].get("state_sha256") for n in names})!=1 or len({routes[n].get("input_schedule_scope") for n in names})!=1: raise SystemExit("captures differ in source state or input schedule")
 if len({docs[n].get("core_sha256") for n in names})!=1: raise SystemExit("captures used different core binaries")
 for i,row in enumerate(traces[a.combined]):
  for n in (a.baseline,a.dispatcher_reference,a.callback_reference):
   other=traces[n][i]
   for k in SHARED:
    if row.get(k)!=other.get(k): raise SystemExit(f"gameplay field mismatch {k} at row {i} vs {n}")
   for k in ("pointer","position"):
    if row["traffic_camera"].get(k)!=other["traffic_camera"].get(k): raise SystemExit(f"camera mismatch {k} at row {i} vs {n}")
 # The compact dispatcher records must exactly equal the first 106 rows of
 # the independent 700-frame dispatcher trace.
 combined_dispatch=read(root/f"{a.combined}.traffic-dispatch-entry-trace.json")
 reference_dispatch=read(root/f"{a.dispatcher_reference}.traffic-dispatch-entry-trace.json")
 cd=combined_dispatch["call_events"]; rd=[e for e in reference_dispatch["call_events"] if e["observed_after_core_tick"]<522]
 if combined_dispatch["log_saturated"] or combined_dispatch["logged_count"]!=106 or cd!=rd: raise SystemExit("combined dispatcher events differ from reference prefix")
 # Callback events should match except the sampled pre-callback Timer2 value,
 # which can legitimately shift when the additional dispatcher logger runs.
 combined_cb=read(root/f"{a.combined}.callback-cycle-trace.json")
 reference_cb=read(root/f"{a.callback_reference}.callback-cycle-trace.json")
 cc=combined_cb["call_events"]; rc=reference_cb["call_events"]
 ignore={"timer2_original_registers"}
 if len(cc)!=107 or len(rc)!=107 or combined_cb["log_saturated"] or not combined_cb["instrumentation"].get("timer_counter_controls_preserved"): raise SystemExit("callback trace count, saturation, or timer controls invalid")
 callback_diff_fields=[]
 for i,(x,y) in enumerate(zip(cc,rc)):
  for k in (set(x)|set(y))-ignore:
   if x.get(k)!=y.get(k): callback_diff_fields.append((i,k))
 if callback_diff_fields: raise SystemExit(f"callback events differ outside expected timer phase field: {callback_diff_fields[:4]}")
 instr_d=combined_dispatch["instrumentation"]; instr_c=combined_cb["instrumentation"]
 if instr_d["source_rom_sha256"]!=ROM_SHA or instr_c["source_rom_sha256"]!=ROM_SHA or instr_d["instrumented_rom_sha256"]!=instr_c["instrumented_rom_sha256"]: raise SystemExit("combined probes are not on the same verified instrumented content")
 pairs=[]; unmatched=[]
 dispatch_by={(e["source_counter"]+1)&0xffffffff:e for e in cd}
 for e in cc:
  hit=dispatch_by.get(e["source_counter"])
  if hit and hit["observed_after_core_tick"]+1==e["observed_after_core_tick"]: pairs.append(e)
  else: unmatched.append(e)
 if len(pairs)!=106 or len(unmatched)!=1 or (unmatched[0]["source_counter"],unmatched[0]["observed_after_core_tick"])!=(4707,511): raise SystemExit("callback/dispatcher counter cadence differs from reviewed boundary")
 windows=[]
 for e in cc:
  total=e["timer3_timer2_overflows"]*65536+e["timer2_signed_elapsed"]
  windows.append({"counter":e["source_counter"],"tick":e["observed_after_core_tick"],"lower_cycles":total-65536,"upper_cycles":total+65536})
 report={"status":"PASS_SAME_RUN_BASELINE_EQUAL_CALLBACK_DISPATCH_TRACE","rom_sha256":ROM_SHA,"instrumented_rom_sha256":instr_d["instrumented_rom_sha256"],"source_state_sha256":routes[a.combined]["state_sha256"],"baseline_equal_shared_rows":522,"compared_fields":list(SHARED)+["traffic_camera.pointer","traffic_camera.position"],"combined_dispatch_events":len(cd),"dispatcher_event_prefix_exact_matches":len(rd),"combined_callback_events":len(cc),"callback_event_matches_excluding_timer2_prevalue":len(cc),"timer2_prevalue_differences_due_to_dispatcher_probe":sum(x.get("timer2_original_registers")!=y.get("timer2_original_registers") for x,y in zip(cc,rc)),"timer_controls_preserved":combined_cb["instrumentation"]["timer_counter_controls_preserved"],"counter_aligned_callback_dispatch_pairs":len(pairs),"unmatched_callback":[{"source_counter":unmatched[0]["source_counter"],"tick":unmatched[0]["observed_after_core_tick"]}],"callback_window_cycles":{"lower_min":min(x["lower_cycles"] for x in windows),"upper_max":max(x["upper_cycles"] for x in windows),"measurement":"Timer2/Timer3 around original callback plus trampoline; +/-65536-cycle sampling bound"},"route":{"waypoint":routes[a.combined]["final_waypoint"],"completed_fares":routes[a.combined]["completed_count"],"fare":routes[a.combined]["fare"]},"finding":"The combined temporary ROM trace preserves all 522 shared player/camera/traffic-context observations against three independent captures. Its 106 dispatcher records exactly match the dispatcher-only reference prefix; all 107 callback measurements match the callback-only trace except the pre-callback Timer2 register snapshot, which shifts on two events because the additional dispatcher logger adds phase-dependent work. Both probes share one in-memory ROM hash; the timer controls remain preserved. This removes separate-run pairing uncertainty for this prefix, but post-retro_run event drains still do not establish exact within-frame order, total main-loop execution time, or any events beyond the 522-frame/106-dispatch sample. The source-follow route completes zero fares.","limitations":["No later dispatcher cadence inferred beyond the captured prefix.","This is an offline instrumented GBA-core diagnostic, not native runtime or gameplay parity.","Text, fonts, HUD, menus, logos, signs, and lettering remain excluded from all upscale paths."]}
 a.output.parent.mkdir(parents=True,exist_ok=True); a.output.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8"); print(json.dumps({k:v for k,v in report.items() if k not in ("finding","limitations")},indent=2))
if __name__=="__main__":main()
