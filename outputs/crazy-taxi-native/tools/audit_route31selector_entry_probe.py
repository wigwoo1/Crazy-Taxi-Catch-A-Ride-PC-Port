from __future__ import annotations
import hashlib,json,pathlib
root=pathlib.Path(r"./")
ref=root/"reports/original-core-reference"
ROM_SHA="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
def read(name): return json.loads((ref/name).read_text(encoding="utf-8"))
def sha(name): return hashlib.sha256((ref/name).read_bytes()).hexdigest()
clean=read("route31clean30b.actor-trace.json")[:8]
probe=read("route31selectorentry8211.actor-trace.json")
cap=read("route31selectorentry8211.json")
stream=read("route31selectorentry8211.traffic-follow-entry-trace.json")
assert cap["status"]=="PASS" and cap["rom_sha256"]==ROM_SHA
assert stream["instrumentation"]["source_rom_sha256"]==ROM_SHA
assert stream["instrumentation"]["selector_read_address"]=="0x03004C98"
assert len(clean)==len(probe)==8 and stream["record_count"]==13
fields=("tick","input_keys","keyinput","heading","speed","counter","previous","frame_delta","delta","intro","position","angles","orientation","gravity","airborne","raw_input_history","traffic_camera","driver_owners","traffic_globals","traffic_frame_context","trip_clock_state","trip_start_inputs")
frames=[]
for i,(a,b) in enumerate(zip(clean,probe)):
    assert a["counter"]==b["counter"] and a["tick"]==b["tick"]
    nonactor=[f for f in fields if a.get(f)!=b.get(f)]
    base=bytes.fromhex(a["traffic_actor_records"]["raw"]);test=bytes.fromhex(b["traffic_actor_records"]["raw"])
    assert len(base)==len(test)==50*92
    actor_diffs=[]
    for slot in range(50):
        off=slot*92
        changed=[j for j in range(92) if base[off+j]!=test[off+j]]
        if changed:
            actor_diffs.append({"slot":slot,"offsets":[f"0x{j:02X}" for j in changed],"values":[{"offset":f"0x{j:02X}","clean":base[off+j],"probe":test[off+j]} for j in changed]})
    frames.append({"tick":i,"counter":a["counter"],"nonactor_fields_exact":not nonactor,"nonactor_field_differences":nonactor,"actor_records_exact":50-len(actor_diffs),"actor_differences":actor_diffs})
assert all(f["nonactor_fields_exact"] for f in frames)
assert all(not f["actor_differences"] for f in frames[:6]) and not frames[7]["actor_differences"]
assert frames[6]["counter"]==8218 and frames[6]["actor_records_exact"]==45
for d in frames[6]["actor_differences"]:
    assert d["slot"] in (9,10,11,12,13) and d["offsets"]==["0x10"]
source_selector={r["counter"]:r["traffic_globals"]["selection_counter_4c98"] for r in clean}
calls=[]
for e in stream["call_events"]:
    delta=int(e["actor_pointer"],16)-0x020000F4
    assert delta>=0 and delta%0x5C==0 and delta//0x5C<50
    slot=delta//0x5C
    expected=source_selector[e["source_counter"]]
    assert e["selector_counter_4c98_entry"]==expected
    calls.append({"source_counter":e["source_counter"],"slot":slot,"actor_pointer":e["actor_pointer"],"selector_counter_4c98_entry":e["selector_counter_4c98_entry"],"entry_tick":e["observed_entry_tick"],"return_tick":e["observed_return_tick"],"selector_matches_clean_vblank_value":True})
expected={8213:[1,2,3,4,6,8],8214:[22],8218:[1,2,4,6,8],8219:[22]}
observed={c:[x["slot"] for x in calls if x["source_counter"]==c] for c in expected}
assert observed==expected,observed
result={"status":"PASS_DIRECT_SELECTOR_CAPTURE_WITH_QUANTIFIED_HELPER_TIMING_LAG","rom_sha256":ROM_SHA,"installed_core_sha256":cap["core_sha256"],"clean_trace_sha256":hashlib.sha256((ref/"route31clean30b.actor-trace.json").read_bytes()).hexdigest(),"instrumented_trace_sha256":sha("route31selectorentry8211.actor-trace.json"),"selector_stub_sha256":stream["instrumentation"]["entry_stub_sha256"],"source_input_sha256":sha("route31selectorprobe8211.inputs.json"),"frames":8,"frames_compared":frames,"helper_calls":calls,"helper_callset":{str(k):v for k,v in observed.items()},"selector_capture":"Full 32-bit IWRAM value read at 0x03004C98 in each 0x08006908 helper-entry probe; all13 calls report111, matching the clean trace's VBlank selector values at their source counters.","timing_limit":"Player/camera/shared fields are exact on all eight VBlanks. Hooked actor pool differs only at counter8218: slots9–13 raw +0x10 are each five source counts behind, then all50 records match again at8219. This is a timing perturbation; helper ordering and entry selectors are diagnostic observations, not clean frame-state parity.","scope":"Eight-frame source-ROM probe with an in-memory hook; not a native game scheduler, continuous driver match, or playable port."}
out=ref/"route31selector-entry-audit.json"
out.write_text(json.dumps(result,indent=2)+"\n",encoding="utf-8")
print(json.dumps({"status":result["status"],"selector_values":sorted(set(x["selector_counter_4c98_entry"] for x in calls)),"helper_calls":len(calls),"actor_records_at_8218":frames[6]["actor_records_exact"],"actor_records_at_8219":frames[7]["actor_records_exact"],"report":str(out)},separators=(",",":")))
