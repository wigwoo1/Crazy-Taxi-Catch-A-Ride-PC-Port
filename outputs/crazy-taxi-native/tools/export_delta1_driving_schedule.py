"""Expand a source-batched driving trace into one-tick native updates."""
from __future__ import annotations
import hashlib,json,pathlib,struct,sys
ROOT=pathlib.Path(__file__).resolve().parents[1]
REF=ROOT/"reports/original-core-reference"
ROM="39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5"
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
    if len(sys.argv) not in (5,6) or (len(sys.argv)==6 and sys.argv[5]!="--pose-heading-only"):raise SystemExit("usage: export_delta1_driving_schedule.py TRACE RUN_JSON BASELINE_EWRAM SCHEDULE [--pose-heading-only]")
    verify_mask=1 if len(sys.argv)==6 else 3
    trace_path,run_path,baseline_path,schedule_path=map(pathlib.Path,sys.argv[1:5])
    rows=json.loads(trace_path.read_text(encoding="utf-8"));run=json.loads(run_path.read_text(encoding="utf-8"));baseline=baseline_path.read_bytes()
    assert len(baseline)==0x40000 and run["status"]=="PASS" and run["rom_sha256"]==ROM
    assert run.get("instrumentation") is None and run.get("traffic_dispatch_instrumentation") is None
    previous=struct.unpack_from("<i",baseline,0x3524)[0]
    total=0;events=[];steps=[];previous_tick=0
    for index,row in enumerate(rows):
        current=int(row["previous"])
        if current==previous:continue
        elapsed=(current-previous)&0xffffffff
        if elapsed>=0x80000000:elapsed-=0x100000000
        expected=max(1,min(60,elapsed))
        delta=int(row["delta"]);frame=int(row["frame_delta"])
        assert frame==expected and delta==frame,(index,delta,frame,expected)
        raw=int(row["raw_input_history"])
        owner=row.get("driver_owners")
        assert isinstance(owner,dict)
        flags=[int(owner[k]) for k in ("flag_1697","flag_153c","freeze_2620","freeze_177b")]
        assert all(v in (0,1) for v in flags)
        start=total
        for sub in range(frame):
            total+=1
            verify=verify_mask if sub==frame-1 else 0
            point=row["position"] if verify else (0,0,0)
            heading=int(row["heading"]) if verify else 0
            speed=int(row["speed"]) if verify else 0
            steps.append((raw,1,1,1,*point,heading,speed,total,*flags,verify))
        events.append({"source_trace_row":index,"source_counter":int(row["counter"]),
            "source_previous":current,"source_frame_delta":frame,"native_substeps":frame,
            "native_tick_start":start+1,"native_tick_end":total,"checkpoint":verify,
            "input_mask":raw,"expected_position":row["position"],"expected_heading":row["heading"],"expected_speed":row["speed"]})
        previous=current;previous_tick=int(row["counter"])
    assert len(events)>10 and len(steps)==total
    schedule_path.parent.mkdir(parents=True,exist_ok=True)
    schedule_path.write_text(f"SOURCE_DRIVING_REFERENCE_V4 {len(steps)}\n"+"".join(" ".join(map(str,s))+"\n" for s in steps),encoding="ascii")
    report={"status":"PASS_SOURCE_BATCH_TO_DELTA1_SCHEDULE_EXPORTED","source_rom_sha256":ROM,
      "source_trace":str(trace_path),"source_trace_sha256":sha(trace_path),"source_run":str(run_path),
      "source_run_sha256":sha(run_path),"baseline_ewram_sha256":sha(baseline_path),
      "schedule":str(schedule_path),"schedule_sha256":sha(schedule_path),"source_update_events":len(events),
      "native_delta1_steps":len(steps),"source_elapsed_units_total":total,"verification_mask":verify_mask,
      "source_update_checkpoints":events,"scope":"Diagnostic-only schedule expansion. Every original source update batch is replaced by the same number of delta-1 native reference-driving steps while holding the source input mask and dynamic owner fields through that batch. Each batch endpoint compares position and heading only when verification_mask is 1, or additionally speed when it is 3. This does not establish full dispatcher, passenger, traffic, or playable-port parity."}
    out=schedule_path.with_suffix(".audit.json");out.write_text(json.dumps(report,indent=2)+"\n",encoding="utf-8")
    print(json.dumps({"status":report["status"],"source_updates":len(events),"native_delta1_steps":len(steps),"last_source_counter":previous_tick,"schedule":str(schedule_path),"audit":str(out)},indent=2))
if __name__=="__main__":main()
