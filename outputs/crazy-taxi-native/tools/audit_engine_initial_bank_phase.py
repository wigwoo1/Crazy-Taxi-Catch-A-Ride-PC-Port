"""Independent phase-only reconstruction; no native/source state is overwritten."""
from pathlib import Path
import json,struct,hashlib
P=Path('./');B=P/'outputs/crazy-taxi-native';R=B/'reports/original-core-reference';W=B/'work/llvm-build-20261014'
log=(W/'full-audio-cooldown-236-endpoint.log').read_bytes()
services=[list(map(int,l.split()[1:])) for l in log.decode().splitlines() if l.startswith('A ') and len(l.split())==11]
assert len(services)==236 and all(s[6]==1 for s in services),'Low engine is not continuously owned by physical slot0'
boot=(R/'boot4200.ewram').read_bytes();output_rate=struct.unpack_from('<I',boot,0x7564)[0]
seed_phase=struct.unpack_from('<I',boot,0x7574+20)[0];seed_step=struct.unpack_from('<I',boot,0x7574+16)[0]
meta=json.loads((B/'assets/audio/manifest.json').read_bytes())['records'][9]
assert meta['selector_prefix']==0 and boot[0x7574+4]==1
assert struct.unpack_from('<I',boot,0x7574+24)[0]==int(meta['source_pointer'],16)+8
limit=meta['decoded_samples'];events=json.loads((R/'callbackdispatchminimal1200.traffic-dispatch-entry-trace.json').read_bytes())['call_events']
assert len(events)==236
endpoint=json.loads((R/'full-audio-endpoint-20261014.json').read_bytes());native=endpoint['native_channels'][0]['phase'];source=endpoint['source_channels'][0]['phase']
def advance(phase,step,banks):
    for _ in range(banks*352):
        if phase>>8>=limit:phase=0
        phase=(phase+step)&0xffffffff
    return phase
def run(mode):
    phase=seed_phase;step=seed_step;clock=0;records=[]
    for s,e in zip(services,events):
        index,stamp,speed,rpm,angle,requests,low,high,context,driver=s
        target=stamp if mode=='native' else stamp-4 if mode=='first_bank_only' else e['observed_after_core_tick']+1
        banks=target-clock;assert banks>=0
        phase=advance(phase,step,banks)
        rate=(10512+rpm)&65535 # Explicit scale4096 hypothesis, checked against native final reconstruction.
        step=rate*256//output_rate;clock=target
        if index in (0,101,102,103,104,105,106,235):records.append({'update':index,'banks':banks,'clock':clock,'rpm':rpm,'step':step,'phase_after_prior_banks':phase})
    return {'phase':advance(phase,step,1),'banks_total':clock+1,'selected_boundaries':records}
variants={m:run(m) for m in ['native','first_bank_only','source_observation_clock']}
assert variants['native']['phase']==native,'Scale4096 reconstruction does not reproduce actual native endpoint'
preview=[s[3] for s in services[103:106]];assert preview==[500,500,500]
report={'status':'PASS_NATIVE_PHASE_RECONSTRUCTION_AND_SOURCE_INITIAL_BANK_HYPOTHESIS' if variants['source_observation_clock']['phase']==source else 'PARTIAL_NATIVE_PHASE_RECONSTRUCTION','source_phase':source,'native_phase':native,'source_minus_native':source-native,'boot_phase':seed_phase,'boot_step':seed_step,'raw_loop_samples':limit,'output_rate':output_rate,'variants':variants,'preview_rpm':preview,'native_log_sha256':hashlib.sha256(log).hexdigest(),'limits':['Phase-only independent model assumes recorded RPM uses scale4096; native endpoint equality confirms reconstruction, not every intermediate rate independently.','Corrected clocks derive from source observation tick plus one completed core frame, not expected channel phase.','Source clock variant is not yet executed in full native controller replay.','Intermediate source snapshots and full controller/gameplay regression still required before production integration.']}
(R/'engine-initial-bank-phase-20261014.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({'status':report['status'],'native_phase':native,'source_phase':source,'predicted_phases':{k:v['phase'] for k,v in variants.items()},'bank_totals':{k:v['banks_total'] for k,v in variants.items()},'preview_rpm':preview}))