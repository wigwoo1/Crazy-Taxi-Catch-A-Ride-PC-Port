#!/usr/bin/env python3
"""Audit two consecutive native traffic scene updates against clean route31 rows."""
import hashlib,json,struct
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]; R=ROOT/"reports/original-core-reference"
TRACE=R/"route31clean30b.actor-trace.json"; CAP=R/"route31clean30b.capture.json"
MANIFEST=R/"route31-postimpact-frame-fixtures.json"; SELECTOR_TRACE=R/"route31selectorentry8211.traffic-follow-entry-trace.json"; TEST=ROOT/"tests/route31_postimpact_traffic_frame_probe.c"
BIN=ROOT/"bin/route31-postimpact-traffic-frame-probe.exe"; OUT=R/"route31-postimpact-frame-native-audit.json"
FIELDS=("flags","type","countdown","x","y","z","heading","route","collision_timer","impulse_x","impulse_z","offset_x","offset_z","path_distance","path_segment","path_x","path_z","path_heading")
ROM="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def record(row,i,descriptors):
    b=bytes.fromhex(row["traffic_actor_records"]["raw"]); p=i*92
    return {"flags":struct.unpack_from("<I",b,p)[0],"type":struct.unpack_from("<I",b,p+4)[0],"countdown":struct.unpack_from("<i",b,p+0x10)[0],"x":struct.unpack_from("<i",b,p+0x14)[0],"y":struct.unpack_from("<i",b,p+0x18)[0],"z":struct.unpack_from("<i",b,p+0x1c)[0],"heading":struct.unpack_from("<i",b,p+0x20)[0],"route":descriptors.get(struct.unpack_from("<I",b,p+0x24)[0],0),"collision_timer":struct.unpack_from("<i",b,p+0x58)[0],"impulse_x":struct.unpack_from("<i",b,p+0x40)[0],"impulse_z":struct.unpack_from("<i",b,p+0x48)[0],"offset_x":struct.unpack_from("<i",b,p+0x4c)[0],"offset_z":struct.unpack_from("<i",b,p+0x54)[0],"path_distance":struct.unpack_from("<i",b,p+0x28)[0],"path_segment":struct.unpack_from("<I",b,p+0x2c)[0],"path_x":struct.unpack_from("<i",b,p+0x34)[0],"path_z":struct.unpack_from("<i",b,p+0x38)[0],"path_heading":struct.unpack_from("<H",b,p+0x3c)[0]}
cap=json.loads(CAP.read_text()); manifest=json.loads(MANIFEST.read_text()); roads=json.loads((ROOT/"assets/traffic/city1-roads.json").read_text()); desc={int(x["descriptor_address"],16):int(x["index"]) for x in roads["paths"]}; rows=json.loads(TRACE.read_text()); by={x["counter"]:x for x in rows}; cases=manifest["cases"]
assert cap["status"]=="PASS" and cap["rom_sha256"]==ROM and cap["frames"]==30
assert sha(TRACE)=="6572930b33452bb4f027983b2c1c7271a72f90dd83df7a50e066b204073dccf5" and len(cases)==2
selector_trace=json.loads(SELECTOR_TRACE.read_text()); selector_events=selector_trace["call_events"]
assert manifest["selector_trace_sha256"]==sha(SELECTOR_TRACE) and len(selector_events)==13
assert {e["selector_counter_4c98_entry"] for e in selector_events}=={111}
assert manifest["direct_helper_selector_values"]==[111]
log=R/manifest["sequence_log"]; lines=log.read_text(encoding="utf-8-sig").splitlines(); frames={}; current=None
for line in lines:
    v=line.split()
    if v[0]=="F":
        n=list(map(int,v[1:])); assert len(n)==13; current=n[0]; frames[current]={"summary":n[1:],"actors":{}}
    elif v[0]=="A":
        assert current is not None
        n=list(map(int,v[1:])); assert len(n)==19; frames[current]["actors"][n[0]]=dict(zip(FIELDS,n[1:]))
assert set(frames)=={0,1} and all(set(frames[i]["actors"])==set(range(50)) for i in frames)
case_reports=[]
for ix,case in enumerate(cases):
    pool_row,driver_row,result_row=(case[k] for k in ("actor_pool_row","driver_state_row","traffic_result_row"))
    assert all(c in by for c in (pool_row,driver_row,result_row))
    assert by[driver_row]["position"]==by[result_row]["position"] and by[driver_row]["heading"]==by[result_row]["heading"] and by[driver_row]["speed"]==by[result_row]["speed"]
    pool=R/case["pool_fixture"]; expected_fixture=R/case["expected_fixture"]; cfg=R/case["config"]
    assert len(pool.read_bytes())==50*72 and len(expected_fixture.read_bytes())==50*72
    n=frames[ix]["summary"]; assert len(n)==12
    actual=dict(zip(("spawned","count","timer","counter","cooldown","near","voice","combo_flag","combo_whole","combo_fraction","rng","speed"),n))
    source=by[result_row]; driver=by[driver_row]; g=source["traffic_globals"]
    expected_shared={"count":case["active_count_assumption"],"timer":g["timer_25d5"],"counter":g["selection_counter_4c98"],"cooldown":g["cooldown_e4"],"near":g["near_miss_25d3"],"voice":g["voice_25d6"],"combo_flag":g["combo_flag_9aa6"],"combo_whole":g["combo_9aa8"],"combo_fraction":g["combo_9aac"],"rng":g["rng_3b04"],"speed":source["speed"]}
    shared_bad={k:{"native":actual[k],"source":v} for k,v in expected_shared.items() if actual[k]!=v}
    actor_bad=[]; phase_bad=[]
    for i in range(50):
        expected=record(source,i,desc); phase=record(driver,i,desc)
        bad={k:{"native":frames[ix]["actors"][i][k],"source":expected[k]} for k in FIELDS if frames[ix]["actors"][i][k]!=expected[k]}
        if bad:actor_bad.append({"slot":i,"fields":bad})
        if any(frames[ix]["actors"][i][k]!=phase[k] for k in FIELDS):phase_bad.append(i)
    case_reports.append({"native_update":ix,"actor_pool_row":pool_row,"driver_state_row":driver_row,"traffic_result_row":result_row,"delta":case["delta"],"frame_delta":case["frame_delta"],"active_count_used":case["active_count_assumption"],"active_count_basis":case["active_count_basis"],"actor_match_count":50-len(actor_bad),"actor_mismatches":actor_bad,"comparison_to_driver_state_row":{"actor_match_count":50-len(phase_bad),"mismatched_slots":phase_bad},"shared_actual":actual,"shared_source_expected":expected_shared,"shared_mismatches":shared_bad,"fixture_pool_sha256":sha(pool),"expected_fixture_sha256":sha(expected_fixture),"config_sha256":sha(cfg)})
# Verify the carried native state at update0 is the clean pre-state for update1.
carry_actor_mismatches=[]
for i in range(50):
    first=record(by[cases[0]["traffic_result_row"]],i,desc); second_pool=record(by[cases[1]["actor_pool_row"]],i,desc)
    if first!=second_pool:carry_actor_mismatches.append(i)
first_after=by[cases[0]["traffic_result_row"]]["traffic_globals"]; second_before=by[cases[1]["actor_pool_row"]]["traffic_globals"]
carry_globals={k:(first_after.get(k),second_before.get(k)) for k in ("timer_25d5","selection_counter_4c98","cooldown_e4","near_miss_25d3","voice_25d6","combo_flag_9aa6","combo_9aa8","combo_9aac","rng_3b04") if first_after.get(k)!=second_before.get(k)}
all_pass=all(c["actor_match_count"]==50 and not c["shared_mismatches"] for c in case_reports) and not carry_actor_mismatches and not carry_globals
report={"status":"PASS_TWO_CONSECUTIVE_NATIVE_SCENE_UPDATES_SOURCE_MATCH" if all_pass else "MISMATCH_SOURCE_TRAFFIC_FRAME_SEQUENCE","rom_sha256":ROM,"capture_core_sha256":cap["core_sha256"],"capture_report_sha256":sha(CAP),"source_trace_sha256":sha(TRACE),"fixture_manifest_sha256":sha(MANIFEST),"selector_trace_sha256":sha(SELECTOR_TRACE),"direct_helper_selector_values":sorted({e["selector_counter_4c98_entry"] for e in selector_events}),"test_source_sha256":sha(TEST),"native_executable_sha256":sha(BIN),"native_sequence_log_sha256":sha(log),"cases":case_reports,"carried_actor_state_matches_next_source_seed":not carry_actor_mismatches,"carried_actor_mismatch_slots":carry_actor_mismatches,"carried_globals_match_next_source_seed":not carry_globals,"carried_global_mismatches":carry_globals,"scope":"Two native taxi_traffic_scene_update calls run in one process. The second call carries the first call’s actor array, timer, selection counter, cooldown, combo state and RNG; the C probe checks those carried globals against the second source pre-state without reloading them. Player pose/speed is supplied from each source driver-state row rather than driven continuously by native gameplay. Each resulting 50-actor state matches the later source row across all18 fields represented by TaxiTrafficActor; shared fields also match. This is repeated source-seeded stage validation, not a continuous input-to-trip replay or complete playable traffic/game loop.","next_dependency":"Resolve or explicitly bound the one-row hook timing effect; then drive continuous native driver/input updates between traffic calls and compare every intervening source row. The selector and helper callset are now directly logged at entry; driver poses remain source-injected."}
OUT.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8"); print(json.dumps({"status":report["status"],"cases":[{"rows":[c["actor_pool_row"],c["driver_state_row"],c["traffic_result_row"]],"actor_match_count":c["actor_match_count"],"phase_comparison":c["comparison_to_driver_state_row"],"shared_mismatches":c["shared_mismatches"]} for c in case_reports],"carried_actor_state_matches_next_source_seed":report["carried_actor_state_matches_next_source_seed"],"carried_globals_match_next_source_seed":report["carried_globals_match_next_source_seed"],"next_dependency":report["next_dependency"]},indent=2))
