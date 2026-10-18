from __future__ import annotations
import hashlib, json, pathlib
root=pathlib.Path(r"./")
ref=root/"reports/original-core-reference"
clean=json.loads((ref/"route31clean30b.actor-trace.json").read_text(encoding="utf-8"))[:8]
inputs=json.loads((ref/"route31selectorprobe8211.inputs.json").read_text(encoding="utf-8"))
assert len(clean)==len(inputs)==8
assert all(row["input_keys"]==[8] for row in inputs)
fields=("tick","input_keys","keyinput","heading","speed","counter","previous","frame_delta","delta","intro","position","angles","orientation","gravity","airborne","raw_input_history","traffic_camera","driver_owners","traffic_globals","traffic_frame_context","trip_clock_state","trip_start_inputs")
def sha(name): return hashlib.sha256((ref/name).read_bytes()).hexdigest()
def load_trace(label): return json.loads((ref/f"{label}.actor-trace.json").read_text(encoding="utf-8"))
def compare(label):
    trace=load_trace(label)
    assert len(trace)==8
    out=[]
    for i,(base,probe) in enumerate(zip(clean,trace)):
        assert base["tick"]==probe["tick"] and base["counter"]==probe["counter"]
        diffs=[]
        for field in fields:
            if base.get(field)!=probe.get(field): diffs.append({"field":field})
        a=bytes.fromhex(base["traffic_actor_records"]["raw"])
        b=bytes.fromhex(probe["traffic_actor_records"]["raw"])
        assert len(a)==len(b)==50*92
        actors=[]
        for slot in range(50):
            start=slot*92
            offsets=[j for j in range(92) if a[start+j]!=b[start+j]]
            if offsets:
                actors.append({"slot":slot,"offsets":[f"0x{x:02X}" for x in offsets],"values":[{"offset":f"0x{x:02X}","clean":a[start+x],"instrumented":b[start+x]} for x in offsets]})
        out.append({"tick":base["tick"],"counter":base["counter"],"non_actor_field_mismatches":diffs,"actor_mismatches":actors,"actor_records_exact":50-len(actors)})
    return out
probe_report=json.loads((ref/"route31selectorfull8211.json").read_text(encoding="utf-8"))
dispatch=json.loads((ref/"route31selectorfull8211.traffic-dispatch-entry-trace.json").read_text(encoding="utf-8"))
dispatch_cmp=compare("route31selectorfull8211")
assert probe_report["rom_sha256"]=="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
assert [e["source_counter"] for e in dispatch["call_events"]]==[8213,8218]
follow_report=json.loads((ref/"route31followstream8211.json").read_text(encoding="utf-8"))
follow=json.loads((ref/"route31followstream8211.traffic-follow-entry-trace.json").read_text(encoding="utf-8"))
follow_cmp=compare("route31followstream8211")
assert follow_report["rom_sha256"]==probe_report["rom_sha256"]
def slot(e):
    delta=int(e["actor_pointer"],16)-0x020000F4
    assert delta>=0 and delta%0x5C==0 and delta//0x5C<50
    return delta//0x5C
calls=[{"counter":e["source_counter"],"slot":slot(e),"actor_pointer":e["actor_pointer"],"argument_r1":e["argument_r1"],"entry_tick":e["observed_entry_tick"],"return_tick":e["observed_return_tick"],"actor_changed_offsets":[i for i,(a,b) in enumerate(zip(bytes.fromhex(e["actor_raw"]),bytes.fromhex(e["actor_after_raw"]))) if a!=b]} for e in follow["call_events"]]
assert len(calls)==13
assert [c["slot"] for c in calls if c["counter"]==8213]==[1,2,3,4,6,8]
assert [c["slot"] for c in calls if c["counter"]==8214]==[22]
assert [c["slot"] for c in calls if c["counter"]==8218]==[1,2,4,6,8]
assert [c["slot"] for c in calls if c["counter"]==8219]==[22]
selector_rows=[r for r in clean if 8212<=r["counter"]<=8221]
assert all(r["traffic_globals"]["selection_counter_4c98"]==111 for r in selector_rows)
assert all(not r["non_actor_field_mismatches"] for r in dispatch_cmp+follow_cmp)
assert dispatch_cmp[6]["actor_records_exact"]==48 and not dispatch_cmp[7]["actor_mismatches"]
assert follow_cmp[6]["actor_records_exact"]==45 and not follow_cmp[7]["actor_mismatches"]
assert all(x["actor_records_exact"]==50 for x in follow_cmp[:6]) and follow_cmp[7]["actor_records_exact"]==50
result={"status":"PASS_HELPER_AND_DISPATCH_PHASE_CAPTURE_WITH_ONE_ROW_TIMING_PERTURBATION","rom_sha256":probe_report["rom_sha256"],"installed_core_sha256":probe_report["core_sha256"],"clean_trace_sha256":sha("route31clean30b.actor-trace.json"),"input_schedule_sha256":sha("route31selectorprobe8211.inputs.json"),"direct_dispatch_trace_sha256":sha("route31selectorfull8211.traffic-dispatch-entry-trace.json"),"helper_trace_sha256":sha("route31followstream8211.traffic-follow-entry-trace.json"),"frames":8,"input_keys":[x["input_keys"] for x in clean],"compared_non_actor_fields":list(fields),"dispatch_events":dispatch["call_events"],"dispatch_framewise_comparison":dispatch_cmp,"helper_calls":calls,"helper_stream_drains":follow["buffer_drains"],"helper_max_records_in_one_tick":follow["max_records_per_vblank"],"helper_calls_spanning_vblank":follow["vblank_spanning_return_frames"],"helper_framewise_comparison":follow_cmp,"clean_selector_rows":{"counters":[r["counter"] for r in selector_rows],"values":[r["traffic_globals"]["selection_counter_4c98"] for r in selector_rows],"limitation":"Captured on clean uninstrumented VBlank rows, not directly at the helper entry instruction."},"interpretation":"The dispatch hook yields events at counters8213 and8218 (delta5). The helper hook records 13 calls and exact actor pointers, entry/return VBlank observations and per-record input/output bytes; the slot8 call crosses the VBlank boundary. Non-actor player/camera/shared-global fields match the clean capture on all eight frames. The dispatch hook differs from clean only in actor countdown byte +0x10 for slots12 and13 at counter8218, then catches up by8219. The more invasive helper-entry/return hook differs at counter8218 only in countdown byte +0x10 for slots9-13, then catches up by8219. Therefore neither hook is a frame-perfect clean parity capture at that one row; use helper call order/pointers as diagnostic phase evidence only, not as a clean state oracle. The selector value111 is still paired clean-source evidence rather than a direct instruction-time sample.","scope":"Eight-frame source-ROM diagnostic around counters8212-8219; not a native game loop, not full scheduler parity, not a playable port."}
out=ref/"route31selector-helper-phase-audit.json"
out.write_text(json.dumps(result,indent=2)+"\n",encoding="utf-8")
print(json.dumps({"status":result["status"],"dispatch_mismatches":[(x["counter"],x["actor_records_exact"],x["actor_mismatches"]) for x in dispatch_cmp if x["actor_mismatches"]],"helper_mismatches":[(x["counter"],x["actor_records_exact"],x["actor_mismatches"]) for x in follow_cmp if x["actor_mismatches"]],"calls":len(calls),"selector":111,"output":str(out)},separators=(",",":")))
