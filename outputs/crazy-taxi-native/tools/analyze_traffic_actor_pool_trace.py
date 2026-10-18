"""Validate source traffic actor route pointers and align a driving schedule.
Reads emulator-captured EWRAM only; it does not execute or emulate the game.
"""
import collections, hashlib, json, pathlib, struct, sys
ROOT = pathlib.Path(__file__).resolve().parents[1]
trace_path = pathlib.Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "reports/original-core-reference/route31poolglobals.actor-trace.json"
roads_path = pathlib.Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT / "assets/traffic/city1-roads.json"
report_path = pathlib.Path(sys.argv[3]) if len(sys.argv) > 3 else ROOT / "reports/original-core-reference/route31poolglobals-pool-route-mapping.json"
schedule_path = pathlib.Path(sys.argv[4]) if len(sys.argv) > 4 else ROOT / "reports/original-core-reference/route31owners.observed.schedule.txt"
lifecycle_path = pathlib.Path(sys.argv[5]) if len(sys.argv) > 5 else report_path.with_name(report_path.stem + "-lifecycle-events.json")
trace = json.loads(trace_path.read_text())
roads = json.loads(roads_path.read_text())
schedule_lines = schedule_path.read_text().splitlines()
assert schedule_lines and schedule_lines[0].startswith("SOURCE_DRIVING_REFERENCE_V3 ")
schedule_count = int(schedule_lines[0].split()[1])
schedule = [[int(v) for v in line.split()] for line in schedule_lines[1:] if line.strip()]
assert len(schedule) == schedule_count and all(len(row) == 14 for row in schedule)
assert roads["rom_sha256"] == "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
by_descriptor = {int(item["descriptor_address"], 16): item for item in roads["paths"]}
changes=[0]*50; first=[None]*50; last=[None]*50; previous=[0]*50
unknown=collections.Counter(); bad_points=[]; transition_refreshes=[]; bad_segments=[]
null_descriptor_with_point=[]; descriptor_without_point=[]; descriptor_without_point_count=0
nonzero_descriptor_records=0; max_live=max_registered=max_pending=0
spawn_events=route_transitions=despawn_events=0
actor_lifecycle_events=[]; lifecycle_counts=collections.Counter(); lifecycle_by_slot=collections.Counter()
previous_actor=[None]*50
previous_frame_globals=None
for tick, frame in enumerate(trace):
    block=frame["traffic_actor_records"]
    assert block["address"]=="0x020000F4" and block["stride"]==0x5C and block["count"]==50
    raw=bytes.fromhex(block["raw"]); assert len(raw)==50*0x5C
    frame_globals=frame.get("traffic_globals")
    live=registered=pending=0
    for slot in range(50):
        base=slot*0x5C; u=lambda off:struct.unpack_from("<I",raw,base+off)[0]; s=lambda off:struct.unpack_from("<i",raw,base+off)[0]
        flags,countdown,desc=u(0),s(0x10),u(0x24); distance,segment,point=s(0x28),u(0x2C),u(0x30)
        current={"descriptor":desc,"flags":flags,"type":u(4),"countdown":countdown,
            "x":s(0x14),"y":s(0x18),"z":s(0x1C),"heading":s(0x20),
            "distance":distance,"segment":segment,"point":point}
        prior=previous_actor[slot]
        if prior is not None:
            kinds=[]
            if prior["descriptor"]!=desc:
                if not prior["descriptor"] and desc:kinds.append("descriptor_created")
                elif prior["descriptor"] and not desc:kinds.append("descriptor_cleared")
                else:kinds.append("route_descriptor_changed")
            if prior["descriptor"] and desc:
                if prior["countdown"]>0 and countdown<=0:kinds.append("pending_to_active")
                elif prior["countdown"]<=0 and countdown>0:kinds.append("active_to_pending")
                if (prior["flags"]&1)!=(flags&1):
                    kinds.append("active_flag_set" if flags&1 else "active_flag_cleared")
            if kinds:
                route=by_descriptor.get(desc) if desc else None
                prior_route=by_descriptor.get(prior["descriptor"]) if prior["descriptor"] else None
                event={"tick":tick,"counter":frame.get("counter"),"frame_delta":frame.get("frame_delta"),
                    "slot":slot,"events":kinds,
                    "before":{**prior,"route":prior_route["index"] if prior_route else None,
                        "descriptor_address":hex(prior["descriptor"]) if prior["descriptor"] else None},
                    "after":{**current,"route":route["index"] if route else None,
                        "descriptor_address":hex(desc) if desc else None},
                    "traffic_globals_before":previous_frame_globals,
                    "traffic_globals_after":frame_globals,
                    "global_deltas":({
                        "rng_changed":previous_frame_globals.get("rng_3b04")!=frame_globals.get("rng_3b04"),
                        "selection_counter_delta":(frame_globals.get("selection_counter_4c98",0)-previous_frame_globals.get("selection_counter_4c98",0))&0xFFFFFFFF,
                        "cooldown_before":previous_frame_globals.get("cooldown_e4"),
                        "cooldown_after":frame_globals.get("cooldown_e4"),
                        "voice_before":previous_frame_globals.get("voice_25d6"),
                        "voice_after":frame_globals.get("voice_25d6")}
                        if previous_frame_globals is not None and frame_globals is not None else None)}
                actor_lifecycle_events.append(event)
                for kind in kinds:lifecycle_counts[kind]+=1;lifecycle_by_slot[(slot,kind)]+=1
        previous_actor[slot]=current
        if desc:
            registered+=1
            if countdown>0:pending+=1
            else:live+=1
        if not desc:
            if point and len(null_descriptor_with_point)<32:null_descriptor_with_point.append({"tick":tick,"slot":slot,"point":hex(point)})
            if previous[slot]:changes[slot]+=1;despawn_events+=1
            previous[slot]=0;continue
        nonzero_descriptor_records+=1;route=by_descriptor.get(desc)
        if route is None:unknown[hex(desc)]+=1;continue
        rid=route["index"];prior_descriptor=previous[slot]
        descriptor_changed=prior_descriptor!=0 and prior_descriptor!=desc
        if first[slot] is None:first[slot]=rid
        if prior_descriptor!=desc:
            changes[slot]+=1
            if prior_descriptor:route_transitions+=1
            else:spawn_events+=1
        previous[slot]=desc;last[slot]=rid
        if segment>=route["count"]:
            if len(bad_segments)<32:bad_segments.append({"tick":tick,"slot":slot,"route":rid,"segment":segment,"count":route["count"],"distance":distance})
            continue
        expected=int(route["point_address"],16)+segment*16
        if not point:
            descriptor_without_point_count+=1
            if len(descriptor_without_point)<32:descriptor_without_point.append({"tick":tick,"slot":slot,"route":rid,"flags":flags,"countdown":countdown,"segment":segment,"distance":distance})
        elif point!=expected:
            detail={"tick":tick,"slot":slot,"route":rid,"segment":segment,"actual":hex(point),"expected":hex(expected)}
            next_matches=False
            if descriptor_changed and tick+1<len(trace):
                next_raw=bytes.fromhex(trace[tick+1]["traffic_actor_records"]["raw"]);nb=slot*0x5C
                nd=struct.unpack_from("<I",next_raw,nb+0x24)[0];ns=struct.unpack_from("<I",next_raw,nb+0x2C)[0];np=struct.unpack_from("<I",next_raw,nb+0x30)[0]
                next_matches=nd==desc and ns<route["count"] and np==int(route["point_address"],16)+ns*16
            if next_matches:
                if len(transition_refreshes)<32:transition_refreshes.append(detail)
            elif len(bad_points)<32:bad_points.append(detail)
    max_live=max(max_live,live);max_registered=max(max_registered,registered);max_pending=max(max_pending,pending)
    previous_frame_globals=frame_globals

schedule_indices=[];cursor=0;unmatched=None;matched_actor_hash=hashlib.sha256();global_change_counts=collections.Counter();prior_globals=None;camera_records=[];camera_missing_rows=[]
for row_index,row in enumerate(schedule):
    found=None
    while cursor<len(trace):
        frame=trace[cursor];owners=frame.get("driver_owners",{})
        exact=(frame["keyinput"]==row[0] and frame["delta"]==row[2] and frame["frame_delta"]==row[3]
            and frame["position"]==row[4:7] and frame["heading"]==row[7] and frame["speed"]==row[8]
            and owners.get("flag_1697")==row[10] and owners.get("flag_153c")==row[11]
            and owners.get("freeze_2620")==row[12] and owners.get("freeze_177b")==row[13])
        stamp_exact=(not schedule_indices or frame["counter"]-trace[schedule_indices[0]]["counter"]==row[9]-schedule[0][9])
        if exact and stamp_exact:
            found=cursor;break
        cursor+=1
    if found is None:
        unmatched={"schedule_row":row_index,"values":row};break
    if schedule_indices:
        expected_elapsed=row[9]-schedule[0][9]
        actual_elapsed=trace[found]["counter"]-trace[schedule_indices[0]]["counter"]
        if actual_elapsed!=expected_elapsed:
            unmatched={"schedule_row":row_index,"trace_index":found,"expected_counter_delta":expected_elapsed,"actual_counter_delta":actual_elapsed};break
    schedule_indices.append(found)
    matched_actor_hash.update(bytes.fromhex(trace[found]["traffic_actor_records"]["raw"]))
    camera=trace[found].get("traffic_camera")
    if camera is None or camera.get("position") is None:
        camera_missing_rows.append({"schedule_row":row_index,"trace_index":found})
    else:
        camera_records.append(camera)
    globals_now=trace[found].get("traffic_globals")
    if globals_now is None:
        unmatched={"schedule_row":row_index,"trace_index":found,"missing":"traffic_globals"};break
    if prior_globals is not None:
        for key,value in globals_now.items():
            if value!=prior_globals[key]:global_change_counts[key]+=1
    prior_globals=globals_now
    cursor=found+1
aligned=len(schedule_indices)==len(schedule) and unmatched is None
report={
 "rom_sha256":roads["rom_sha256"],"trace_sha256":hashlib.sha256(trace_path.read_bytes()).hexdigest(),
 "roads_sha256":hashlib.sha256(roads_path.read_bytes()).hexdigest(),"schedule_sha256":hashlib.sha256(schedule_path.read_bytes()).hexdigest(),
 "trace":str(trace_path),"schedule":str(schedule_path),"city":roads["city"],"ticks":len(trace),"slots":50,
 "nonzero_route_descriptor_records":nonzero_descriptor_records,"known_route_descriptor_count":len(by_descriptor),
 "unique_route_ids_observed":sorted({v for v in first+last if v is not None}),"unknown_route_descriptors":dict(unknown),
 "point_pointer_mismatch_examples":bad_points,"one_update_route_transition_pointer_refresh_count":len(transition_refreshes),
 "one_update_route_transition_pointer_refresh_examples":transition_refreshes,"segment_out_of_range_examples":bad_segments,
 "route_descriptor_without_point_pointer_count":descriptor_without_point_count,"route_descriptor_without_point_pointer_examples":descriptor_without_point,
 "null_descriptor_with_point_examples":null_descriptor_with_point,"spawn_events":spawn_events,"route_transition_events":route_transitions,
 "despawn_events":despawn_events,"max_live_descriptor_slots":max_live,"max_registered_descriptor_slots":max_registered,
 "max_pending_descriptor_slots":max_pending,"route_changes_by_slot":changes,"first_route_by_slot":first,"last_route_by_slot":last,
 "schedule_rows":len(schedule),"schedule_actor_pool_alignment_pass":aligned,"schedule_trace_indices":schedule_indices,
 "schedule_unmatched_example":unmatched,"aligned_actor_pool_sha256":matched_actor_hash.hexdigest(),
 "aligned_camera_observations":len(camera_records),"aligned_camera_coverage_complete":len(camera_records)==len(schedule_indices),
 "camera_missing_examples":camera_missing_rows[:16],
 "traffic_global_change_counts_aligned_schedule":dict(global_change_counts),
 "aligned_camera_pointer_values":sorted({item["pointer"] for item in camera_records}),
 "first_aligned_camera":camera_records[0] if camera_records else None,"last_aligned_camera":camera_records[-1] if camera_records else None,
 "first_aligned_traffic_globals":trace[schedule_indices[0]]["traffic_globals"] if schedule_indices else None,
 "last_aligned_traffic_globals":trace[schedule_indices[-1]]["traffic_globals"] if schedule_indices else None,
 "scope":"Read-only source trace analysis. Validates actor route/point pointers for all captured frames and aligns each source driving row by input, timing, pose and owner flags. It proves actor/global snapshots are available at the selected schedule points; does not execute native 50-slot dispatch, prove per-update traffic routine parity or establish a successful fare route."
}
assert not unknown,unknown;assert not bad_points,bad_points[:1];assert not bad_segments,bad_segments[:1]
assert aligned,(len(schedule_indices),unmatched)
life={"status":"PASS_SOURCE_ACTOR_POOL_LIFECYCLE_EXTRACTION",
 "rom_sha256":roads["rom_sha256"],"trace_sha256":hashlib.sha256(trace_path.read_bytes()).hexdigest(),
 "roads_sha256":hashlib.sha256(roads_path.read_bytes()).hexdigest(),
 "source_trace":str(trace_path),"ticks":len(trace),"slots":50,
 "record_layout":{"stride":92,"flags":0,"type":4,"countdown":16,"x":20,"y":24,"z":28,
  "heading":32,"route_descriptor":36,"route_distance":40,"route_segment":44,"route_point_pointer":48},
  "event_counts":dict(lifecycle_counts),
  "event_counts_by_slot":{str(slot):{kind:n for (s,kind),n in lifecycle_by_slot.items() if s==slot}
   for slot in range(50) if any(s==slot for s,_ in lifecycle_by_slot)},
  "transition_signatures":{},
  "events":actor_lifecycle_events,
  "scope":"Transition-only extraction from original-ROM actor-pool snapshots. Event globals are frame-level before/after observations and cannot be assigned to an individual actor when several transitions share a VBlank. Events are VBlank observations, not proof of exact routine entry/return order or a general native dispatch schedule. No state was synthesized or changed."}
events_per_tick=collections.Counter(event["tick"] for event in actor_lifecycle_events)
candidate_rejections=[event for event in actor_lifecycle_events
    if "route_descriptor_changed" in event["events"]
    and event["after"]["countdown"]==180
    and event["global_deltas"] is not None
    and event["global_deltas"]["selection_counter_delta"]==0
    and not event["global_deltas"]["rng_changed"]]
life["transition_signatures"]["candidate_path_write_then_180_retry_without_rng_or_selection_advance"]={
    "observations":len(candidate_rejections),
    "unique_vblank_ticks":len({event["tick"] for event in candidate_rejections}),
    "single_lifecycle_event_ticks":sum(events_per_tick[event["tick"]]==1 for event in candidate_rejections),
    "examples":[{"tick":event["tick"],"counter":event["counter"],"slot":event["slot"],
        "old_route":event["before"]["route"],"candidate_route":event["after"]["route"],
        "countdown_before":event["before"]["countdown"],"countdown_after":event["after"]["countdown"]}
        for event in candidate_rejections[:8]],
    "interpretation":"Observed signature is consistent with a route candidate/path write followed by a 180-tick retry, without frame-level RNG or selection-counter advance. It is a hypothesis about spawn rejection; snapshots do not prove the exact caller or intra-VBlank event order."}
lifecycle_path.parent.mkdir(parents=True,exist_ok=True)
lifecycle_path.write_text(json.dumps(life,indent=2)+"\n")
report["actor_lifecycle_report"]=str(lifecycle_path)
report["actor_lifecycle_report_sha256"]=hashlib.sha256(lifecycle_path.read_bytes()).hexdigest()
report["actor_lifecycle_event_counts"]=dict(lifecycle_counts)
report["analyzer_sha256"]=hashlib.sha256(pathlib.Path(__file__).read_bytes()).hexdigest()
report_path.parent.mkdir(parents=True,exist_ok=True);report_path.write_text(json.dumps(report,indent=2)+"\n")
print("PASS",json.dumps({"ticks":len(trace),"slots":50,"routes":report["unique_route_ids_observed"],"descriptor_records":nonzero_descriptor_records,"unknown_descriptors":len(unknown),"point_mismatches":len(bad_points),"transition_refreshes":len(transition_refreshes),"schedule_rows":len(schedule),"schedule_aligned":aligned,"globals":dict(global_change_counts),"lifecycle_events":dict(lifecycle_counts),"lifecycle_report":str(lifecycle_path),"report":str(report_path)}))


