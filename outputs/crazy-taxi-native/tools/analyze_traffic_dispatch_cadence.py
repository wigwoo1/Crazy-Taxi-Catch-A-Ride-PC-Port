"""Stream an original-ROM traffic trace and summarize observable actor-change cadence.
This reports post-VBlank sample differences; it does not prove routine entry or calls on unchanged rows.
"""
import collections, hashlib, json, pathlib, struct, sys
root=pathlib.Path(__file__).resolve().parents[1]
trace=pathlib.Path(sys.argv[1]) if len(sys.argv)>1 else root/'reports/original-core-reference/route31poolcontext6.actor-trace.json'
out=pathlib.Path(sys.argv[2]) if len(sys.argv)>2 else root/'reports/original-core-reference/route31poolcontext6-dispatch-cadence.json'
class StreamArray:
    def __init__(self,path): self.f=path.open('r',encoding='utf-8'); self.buf=''; self.pos=0; self.done=False; self.dec=json.JSONDecoder()
    def fill(self):
        chunk=self.f.read(1<<20)
        if not chunk: self.done=True; return False
        self.buf+=chunk; return True
    def whitespace(self):
        while True:
            while self.pos<len(self.buf) and self.buf[self.pos].isspace(): self.pos+=1
            if self.pos<len(self.buf) or self.done:return
            self.fill()
    def __iter__(self):
        self.whitespace()
        if self.pos>=len(self.buf) or self.buf[self.pos]!='[': raise ValueError('expected JSON array')
        self.pos+=1
        while True:
            self.whitespace()
            if self.pos>=len(self.buf): raise ValueError('truncated JSON array')
            if self.buf[self.pos]==']': return
            try: value,end=self.dec.raw_decode(self.buf,self.pos)
            except json.JSONDecodeError:
                if self.done: raise
                self.fill(); continue
            self.pos=end
            yield value
            self.whitespace()
            if self.pos>=len(self.buf): raise ValueError('truncated after array value')
            if self.buf[self.pos]==',': self.pos+=1
            elif self.buf[self.pos]==']': return
            else: raise ValueError('expected comma or closing bracket')
            if self.pos>(1<<20): self.buf=self.buf[self.pos:]; self.pos=0

def distribution(values):
    c=collections.Counter(values)
    return {str(k):c[k] for k in sorted(c)}
changed=[]; global_changes=collections.Counter(); previous_globals=None; frames=0; keyinput_counts=collections.Counter(); deltas=collections.Counter(); frame_deltas=collections.Counter(); frozen=collections.Counter(); trip=collections.Counter(); changed_slot_counts=collections.Counter(); event_by_delta=collections.Counter(); live_by_delta_event=collections.Counter(); sample=[]
for row in StreamArray(trace):
    tick=row.get('tick',frames); frames+=1
    gates=row.get('traffic_frame_context',{})
    frozen[str(gates.get('freeze_5f20','?'))]+=1; trip[str(gates.get('trip_active_1694','?'))]+=1
    deltas[str(row.get('delta'))]+=1; frame_deltas[str(row.get('frame_delta'))]+=1; keyinput_counts[str(row.get('keyinput'))]+=1
    slots=row.get('traffic_pool_changed_from_previous_vblank',[])
    delta_key=str(row.get('delta')); is_event=bool(slots); event_by_delta[(delta_key,'changed' if is_event else 'unchanged')]+=1
    raw=bytes.fromhex(row.get('traffic_actor_records',{}).get('raw',''))
    live=sum(1 for slot in range(50) if len(raw)==50*0x5c and struct.unpack_from('<I',raw,slot*0x5c+0x24)[0] and struct.unpack_from('<i',raw,slot*0x5c+0x10)[0]<=0)
    live_by_delta_event[(delta_key,str(live),'changed' if is_event else 'unchanged')]+=1
    if slots:
        event={'tick':tick,'changed_slots':slots,'delta':row.get('delta'),'frame_delta':row.get('frame_delta'),'counter':row.get('counter'),'keyinput':row.get('keyinput'),'freeze':gates.get('freeze_5f20'),'trip_active':gates.get('trip_active_1694')}
        changed.append(event); changed_slot_counts[str(len(slots))]+=1
        if len(sample)<40: sample.append(event)
    now=row.get('traffic_globals',{})
    if previous_globals is not None:
        for key,value in now.items():
            if value!=previous_globals.get(key): global_changes[key]+=1
    previous_globals=now
intervals=[b['tick']-a['tick'] for a,b in zip(changed,changed[1:])]
hashv=hashlib.sha256()
with trace.open('rb') as f:
    for chunk in iter(lambda:f.read(1<<20),b''): hashv.update(chunk)
report={'scope':'Post-VBlank actor-table sample differences only; an unchanged sample does not prove the original dispatcher was not called. This report narrows candidate source cadence but is not routine-entry tracing or native gameplay parity.','trace':str(trace),'trace_bytes':trace.stat().st_size,'trace_sha256':hashv.hexdigest(),'frames':frames,'actor_change_samples':len(changed),'first_actor_change_samples':sample,'last_actor_change_samples':changed[-20:],'changed_slots_per_sample':distribution([len(x['changed_slots']) for x in changed]),'interval_between_actor_change_samples':distribution(intervals),'max_interval_between_actor_change_samples':max(intervals) if intervals else None,'frame_delta_values':dict(sorted(frame_deltas.items(),key=lambda item:int(item[0]) if item[0]!='None' else -1)),'delta_values':dict(sorted(deltas.items(),key=lambda item:int(item[0]) if item[0]!='None' else -1)),'actor_change_frame_delta_values':distribution([x['frame_delta'] for x in changed]),'actor_change_delta_values':distribution([x['delta'] for x in changed]),'sample_change_by_delta':{f'{d}_{kind}':count for (d,kind),count in sorted(event_by_delta.items())},'live_actor_change_by_delta':{f'{d}_live{n}_{kind}':count for (d,n,kind),count in sorted(live_by_delta_event.items())},'traffic_globals_change_counts':dict(global_changes),'freeze_values':dict(frozen),'trip_active_values':dict(trip),'keyinput_values':dict(keyinput_counts),'interpretation':'Actor changes cluster at observed source checkpoints. These are upper bounds on state-change timing, not proof that dispatch only ran at those VBlanks; use them to select dynamic trace windows around transitions.'}
out.parent.mkdir(parents=True,exist_ok=True); out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({k:report[k] for k in ['frames','actor_change_samples','changed_slots_per_sample','interval_between_actor_change_samples','max_interval_between_actor_change_samples','traffic_globals_change_counts','freeze_values','trip_active_values','trace_sha256']},indent=2))



