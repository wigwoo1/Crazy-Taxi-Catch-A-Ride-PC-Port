#!/usr/bin/env python3
"""Audit source input callback and in-game input-latch boundaries."""
from __future__ import annotations
import hashlib, json, pathlib
ROOT = pathlib.Path(__file__).resolve().parents[1]
REF = ROOT / "reports/original-core-reference"
ROM_SHA = "39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
CORE_SHA = "73411273b6b9bb99e22f2278f8da79f92feb3b80a007a21e18262a915a0f84d4"
STATE_SHA = "a20a48db7c4514d28c072de7ad80ed1576d77076a06d5e7b6c038ce8b0404a01"
# RetroPad ID -> original active-low GBA KEYINPUT bit.
BIT = {8:0, 0:1, 2:2, 3:3, 7:4, 6:5, 4:6, 5:7, 11:8, 10:9}
def read(name): return json.loads((REF/name).read_text(encoding="utf-8"))
def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def canon(v):
    if isinstance(v, dict): return {k:canon(x) for k,x in v.items() if k != "raw_40"}
    if isinstance(v, list): return [canon(x) for x in v]
    return v
def mask(keys):
    out=0x3ff
    for key in keys:
        assert key in BIT, ("unknown RetroPad id", key)
        out &= ~(1 << BIT[key])
    return out

def main():
    tag="r5294input60"
    rows=read(tag+".actor-trace.json")
    run=read(tag+".json")
    route=read(tag+".route-follow.json")
    trace=read(tag+".input-callback-trace.json")
    reference=read("r5294cpu60pool.actor-trace.json")
    reference_run=read("r5294cpu60pool.json")
    polls=trace["input_poll_first_by_tick"]
    first=trace["input_state_first_by_tick"]
    calls=trace["input_state_calls_by_tick"]
    assert len(rows)==len(polls)==len(first)==60
    assert trace["status"]=="PASS_INPUT_CALLBACK_BOUNDARY_TRACE"
    assert set(calls.values())=={16} and set(calls)=={str(i) for i in range(60)}
    assert run["status"]==reference_run["status"]=="PASS"
    assert run["rom_sha256"]==route["rom_sha256"]==trace["rom_sha256"]==ROM_SHA
    assert run["core_sha256"]==route["core_sha256"]==trace["core_sha256"]==CORE_SHA
    assert run["instrumentation"] is None and reference_run["instrumentation"] is None
    assert route["state_sha256"]==STATE_SHA
    # Read-only callback telemetry must leave every recorded game-state field intact.
    assert len(reference)==60
    for i,(a,b) in enumerate(zip(reference,rows)):
        assert canon(a)==canon(b), ("callback telemetry changed source trace",i)
        assert polls[i]["tick"]==first[i]["tick"]==rows[i]["tick"]==i
        assert polls[i]["keys"]==rows[i]["input_keys"]
        if i:
            # first input_state() observes the register before this frame's core run.
            assert first[i]["keyinput"]==rows[i-1]["keyinput"], ("pre-run KEYINPUT boundary",i)
            assert first[i]["raw_input_history"]==rows[i-1]["raw_input_history"], ("pre-run input history",i)

    boundaries=[]
    for i,row in enumerate(rows):
        if row["previous"]==row["counter"] and (i==0 or rows[i-1]["previous"]!=row["previous"]):
            boundaries.append(i)
    transitions=[]
    prior=None
    for i,poll in enumerate(polls):
        keys=poll["keys"]
        if prior is None or keys!=prior:
            expected=mask(keys)
            if i==0:
                commit=0
            else:
                commit=next((j for j in range(i,len(rows))
                    if j in boundaries and rows[j]["raw_input_history"]==expected and rows[j]["keyinput"]==expected),None)
                assert commit is not None, ("input mask never committed at source boundary",i,keys,expected)
                assert rows[i]["raw_input_history"]!=expected or rows[i]["keyinput"]!=expected, (
                    "changed mask already visible before its next source boundary",i)
            transitions.append({"input_change_tick":i,"keys":keys,"expected_active_low_KEYINPUT":expected,
                "first_source_commit_tick":commit,"delay_in_VBlanks":commit-i,
                "boundary_source_counter":rows[commit]["counter"],
                "post_commit_previous":rows[commit]["previous"]})
            prior=keys
    assert boundaries==[0,5,11,17,23,28,33,38,43,48,53,58], boundaries
    assert [r["delay_in_VBlanks"] for r in transitions]==[0,5,5,5,4,4], transitions
    assert route["completed_count"]==0 and route["fare"]==[0,0]
    report={"status":"PASS_SOURCE_INPUT_LATCH_BOUNDARY_60_FRAME_TRACE",
      "source_rom_sha256":ROM_SHA,"core_sha256":CORE_SHA,"input_state_sha256":STATE_SHA,
      "frames":60,"rom_instrumentation":None,"input_poll_rows":len(polls),
      "input_state_query_count_per_vblank":16,"callback_run_matches_uninstrumented_game_state":True,
      "matching_reference_trace":"r5294cpu60pool.actor-trace.json",
      "matching_reference_sha256":sha(REF/"r5294cpu60pool.actor-trace.json"),
      "source_update_boundary_ticks":boundaries,"input_transitions":transitions,
      "interpretation":"Each of the 60 VBlanks produced one poll and 16 button queries. Read-only callback telemetry exactly matches the paired uninstrumented gameplay trace (optional raw camera bytes omitted). At each first input_state query, the active-low key register and input history match the previous VBlank's post-run values. Five scripted key-mask changes become visible in both source KEYINPUT and the game's 0x34DE input history at the next observed previous==counter source-update boundary, four or five VBlanks later in this capture. This identifies the source-facing latch boundary for this fixed route/state and installed core; it does not by itself prove all hardware input callers, every game mode, or full original scheduling parity.",
      "scope":"Read-only mGBA/libretro callback telemetry on the supplied ROM from a saved pickup state. Scripted route remains incomplete at waypoint %d with zero fare; not physical Xbox behavior or a playable-port result."%route["final_waypoint"]}
    out=REF/"counter5294-input-latch-boundary-audit.json"
    out.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps({"status":report["status"],"query_count_per_vblank":16,
      "source_update_boundary_ticks":boundaries,"input_transitions":transitions,"report":str(out)},indent=2))
if __name__=="__main__":main()


