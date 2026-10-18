#!/usr/bin/env python3
"""Long-window validation of source update and input-latch boundaries."""
from __future__ import annotations
import collections, hashlib, json, pathlib, statistics
ROOT=pathlib.Path(__file__).resolve().parents[1]
REF=ROOT/"reports/original-core-reference"
ROM="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
CORE="73411273b6b9bb99e22f2278f8da79f92feb3b80a007a21e18262a915a0f84d4"
STATE="a20a48db7c4514d28c072de7ad80ed1576d77076a06d5e7b6c038ce8b0404a01"
BIT={8:0,0:1,2:2,3:3,7:4,6:5,4:6,5:7,11:8,10:9}
def read(n):return json.loads((REF/n).read_text(encoding="utf-8"))
def digest(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def active_low(keys):
    v=0x3ff
    for k in keys:
        assert k in BIT
        v &= ~(1<<BIT[k])
    return v

def main():
    tag="r5294input600"
    run=read(tag+".json"); route=read(tag+".route-follow.json")
    rows=read(tag+".actor-trace.json"); cb=read(tag+".input-callback-trace.json")
    cpu_path=REF/(tag+".cpu-timing-trace.json")
    cpu=json.loads(cpu_path.read_text(encoding="utf-8"))
    polls=cb["input_poll_first_by_tick"]; queries=cb["input_state_first_by_tick"]
    calls=cb["input_state_calls_by_tick"]
    assert run["status"]=="PASS" and run["frames"]==len(rows)==600
    assert run["rom_sha256"]==route["rom_sha256"]==cb["rom_sha256"]==ROM
    assert run["core_sha256"]==route["core_sha256"]==cb["core_sha256"]==CORE
    assert run["instrumentation"] is None and run["traffic_dispatch_instrumentation"] is None
    assert route["state_sha256"]==STATE and cb["status"]=="PASS_INPUT_CALLBACK_BOUNDARY_TRACE"
    assert len(polls)==len(queries)==len(calls)==600 and set(calls.values())=={16}
    assert [x["tick"] for x in cpu["records"]]==list(range(-1,600))
    # Raw actor-pool integrity and exact actor-delta table correspondence.
    stride,count=0x5c,50; prev_pool=None
    for i,row in enumerate(rows):
        raw=bytes.fromhex(row["traffic_actor_records"]["raw"])
        assert len(raw)==stride*count
        changed=row.get("traffic_pool_changed_from_previous_vblank") or []
        if prev_pool is not None:
            actual=[s for s in range(count) if prev_pool[s*stride:(s+1)*stride]!=raw[s*stride:(s+1)*stride]]
            assert actual==changed,(i,actual,changed)
        prev_pool=raw
        assert polls[i]["tick"]==queries[i]["tick"]==i
        assert polls[i]["keys"]==row["input_keys"]
        if i:
            assert queries[i]["keyinput"]==rows[i-1]["keyinput"]
            assert queries[i]["raw_input_history"]==rows[i-1]["raw_input_history"]
    boundaries=[]
    for i,row in enumerate(rows):
        if row["previous"]==row["counter"] and (i==0 or rows[i-1]["previous"]!=row["previous"]):
            boundaries.append(i)
    intervals=[b-a for a,b in zip(boundaries,boundaries[1:])]
    assert len(boundaries)==118 and set(intervals)<={5,6}
    for ix,(a,b) in enumerate(zip(boundaries,boundaries[1:])):
        assert rows[b]["frame_delta"]==b-a
    transition_rows=[]; prior=None
    for i,p in enumerate(polls):
        keys=tuple(p["keys"])
        if prior is None or keys!=prior:
            expected=active_low(keys)
            if i==0:
                commit=0
            else:
                commit=next((j for j in boundaries if j>=i and
                    rows[j]["keyinput"]==expected and rows[j]["raw_input_history"]==expected),None)
                assert commit is not None,(i,keys,expected)
                assert 0<commit-i<=6,(i,commit)
            transition_rows.append({"input_change_tick":i,"keys":list(keys),
                "expected_active_low_mask":expected,"source_commit_tick":commit,
                "delay_vblanks":commit-i,"source_counter":rows[commit]["counter"],
                "frame_delta":rows[commit]["frame_delta"]})
            prior=keys
    assert len(transition_rows)==31
    assert route["completed_count"]==0 and route["fare"]==[0,0]
    report={"status":"PASS_UNINSTRUMENTED_600_FRAME_SOURCE_INPUT_AND_DELTA_BOUNDARIES",
      "rom_sha256":ROM,"core_sha256":CORE,"state_sha256":STATE,"frames":600,
      "rom_instrumentation":None,"input_state_queries_per_vblank":16,
      "source_update_boundary_count":len(boundaries),"source_update_boundary_ticks":boundaries,
      "source_boundary_interval_histogram":dict(sorted(collections.Counter(intervals).items())),
      "source_boundary_interval_min_median_max":[min(intervals),statistics.median(intervals),max(intervals)],
      "frame_delta_equals_elapsed_vblanks_at_each_boundary":True,
      "adjacent_raw_actor_pool_deltas_verified":599,
      "input_transition_count":len(transition_rows),"input_commit_delay_histogram":dict(sorted(collections.Counter(x["delay_vblanks"] for x in transition_rows).items())),
      "input_transitions":transition_rows,"cpu_vblank_snapshots":len(cpu["records"]),
      "cpu_trace_sha256":digest(cpu_path),"actor_trace_sha256":digest(REF/(tag+".actor-trace.json")),
      "route_result":{"final_waypoint":route["final_waypoint"],"completed_count":route["completed_count"],"fare":route["fare"]},
      "interpretation":"Across 600 uninstrumented VBlanks from the fixed pickup state, 118 source update boundaries are observed. Each consecutive source boundary is 5 or 6 VBlanks apart, and its stored frame_delta exactly equals the elapsed VBlank count. All 599 post-initial 50-actor-pool deltas match adjacent raw records. Thirty-one scripted input masks are observed; after each noninitial change, the in-game KEYINPUT/history reflect that mask at the next matching source update boundary 4-5 VBlanks later in this route. Read-only frontend callback telemetry leaves the source state trace intact. These results constrain source scheduling for this state/core but do not prove every game-mode caller or the native 60 Hz interpolation design.",
      "scope":"Offline source-ROM capture using the supplied ROM, installed core, saved pickup state, and scripted route follower. No fare completion; no physical Xbox or native gameplay claim."}
    out=REF/"counter5294-input-latch-600frame-audit.json"
    out.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps({"status":report["status"],"frames":600,"boundaries":len(boundaries),"interval_histogram":report["source_boundary_interval_histogram"],"input_changes":len(transition_rows),"input_delay_histogram":report["input_commit_delay_histogram"],"report":str(out)},indent=2))
if __name__=="__main__":main()
