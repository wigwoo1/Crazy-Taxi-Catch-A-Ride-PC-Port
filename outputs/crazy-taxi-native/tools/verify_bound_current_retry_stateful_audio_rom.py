"""Original10B54 versus actual Bound controller states; explicit captured callback contracts."""
from pathlib import Path
import json,hashlib,sys
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_game_audio_controller_rom.py').read_text().split('\nrng=random.Random')[0],ns)
label='bound-audio-effects-exit-restart-driver'+str(int(sys.argv[1]))
rows=[];resets={}
for line in (root/'reports'/(label+'.log')).read_text().splitlines():
 if line.startswith('BOUND AUDIO RESET:'):resets[len(rows)]=list(map(int,line.split(':',1)[1].split()))
 if line.startswith('BOUND AUDIO STATE:'):rows.append(list(map(int,line.split(':',1)[1].split())))
assert len(resets)==2
(root/'reports'/(label+'-states.txt')).write_text('\n'.join(' '.join(map(str,row)) for row in rows)+'\n')
for i in ns['Cs'](ns['CS_ARCH_ARM'],ns['CS_MODE_THUMB']).disasm(ns['r'][0x11490:0x114f0],0x8011490):ns['allowed_thumb'][i.address]=bytes(i.bytes)

sf,inf=ns['sf'],ns['inf'];assert len(sf)==24 and len(inf)==43 and len(rows)>0
class CapturedProbe(ns['ControllerProbe']):
 def external(self,target):
  if target in [0x8032488,0x8032520]:return super().external(target)
  index=len(self.calls);assert index<len(self.expected),(index,hex(target))
  event=self.expected[index];ret=event[3]
  # External results are recorded actual native callbacks, not fabricated audio handles.
  self.inputs['random']=ret
  for j in range(9):self.inputs[f'starts[{j}]']=ret;self.inputs[f'busy[{j}]']=ret
  if target==0x8015d98:self.inputs['action'+str(self.reg[0])]=ret
  super().external(target)
  assert self.calls[-1]==event[:3],dict(frame=self.frame,call=index,original=self.calls[-1],native=event[:3])
  self.reg[0]=ret&0xffffffff
  if event[0]==7:self.write(0x20025d6,event[4],1)
def controller_values(p):
 values=[]
 for k in sf:
  a,n,sgn=ns['smap'][k];v=p.read(a,n);values.append(v-(1<<(n*8)) if sgn and v>=(1<<(n*8-1)) else v)
 return values
steps=events=ended=outer=0
p=None
for frame,row in enumerate(rows):
 before=dict(zip(sf,row[:24]));inputs=dict(zip(inf,row[24:67]));after=row[67:91];count=row[91]
 assert len(row)==92+count*5
 calls=[row[92+5*j:97+5*j] for j in range(count)]
 if frame==0:
  p=CapturedProbe((0,0,0,0,0,1,0,0,0,0))
  for k,(a,n,sgn) in ns['smap'].items():p.write(a,before[k],n)
 if frame in resets and frame:
  reset=resets[frame];prior=controller_values(p)
  assert all(prior[j]==reset[j] for j in range(24) if j not in [21,22])
  assert [(prior[j],reset[j]) for j in [21,22]]==[(-1,0),(-1,0)]
  # Original reset overwrites both selection bytes: retain original prior values.
  p.write(0x20025e0,reset[24]);p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.steps=0
  p.run(entry=0x8011490,stop=0x10020000)
  assert controller_values(p)==reset[25:49]
  assert p.read(0x20025e0)==reset[49]
 assert controller_values(p)==row[:24],dict(frame=frame,phase='retained-before',original=controller_values(p),native=row[:24])
 p.calls=[];p.expected=calls;p.frame=frame;p.inputs=inputs.copy();p.steps=0;prior_steps=0 # per-call instruction budget, no state reset
 # Restore only the explicit offline call frame; controller memory stays retained.
 p.reg[13]=0x10000400
 for k,(a,n) in ns['imap'].items():p.write(a,inputs[k],n)
 for k,a,base in [('current_class',0x2001688,0x2009500),('alternate_class',0x200168c,0x2009520)]:
  p.write(a,0 if inputs[k]==-1 else base);p.write(base+1,1 if inputs[k]==0 else 0,1)
 try:p.run(entry=0x8010b54,stop=0x801146a)
 except Exception:
  print('stateful original probe stopped at frame',frame,'steps',p.steps);raise
 actual=[]
 for k in sf:
  a,n,sgn=ns['smap'][k];v=p.read(a,n);actual.append(v-(1<<(n*8)) if sgn and v>=(1<<(n*8-1)) else v)
 assert actual==after,dict(frame=frame,original=actual,native=after)
 assert len(p.calls)==count,(frame,len(p.calls),count)
 steps+=p.steps-prior_steps;events+=count;ended+=bool(inputs['ended']);outer+=bool(inputs['outer'])
report=dict(status='PASS_STATEFUL_ACTUAL_BOUND_AUDIO_CONTROLLER_ROM',frames=len(rows),ended_frames=ended,outer_frames=outer,ordered_requests=events,controller_fields=24,thumb_instructions=steps,rom_sha256=hashlib.sha256(ns['r']).hexdigest(),retained_field_checks=(len(rows)-1)*24,scope='Original11490 reset executed at retry on retained controller memory; selector pre-import discrepancy excluded because reset overwrites both, auxiliary owner captured contract. Original controller state initialized once, then retained across all captured calls without bridge/state reseeding; captured native prestate checked everycall. Actual native Bound pre/post controller state and ordered requests versus original10B54; all24 outputs exact per call. Real native callback results supplied explicitly for channel allocation/busy/actions/RNG; reward callback selector copied as explicit side-effect contract. Original external callback bodies, input bridge/driver ownership, original full-game/global initialization trajectory, mixer cadence, physical audio and full game not proved.')
from collections import Counter
ops=Counter();starts=Counter()
for row in rows:
 for j in range(row[91]):
  op,handle,value,result,selection=row[92+5*j:97+5*j];ops[op]+=1
  if op==1:starts[handle]+=1
report['request_op_counts']=dict(ops);report['start_sound_indices']=dict(starts)
report['driver_voice_starts_131_202']=sum(n for ix,n in starts.items() if 131<=ix<=202)
report['capture_sha256']=hashlib.sha256((root/('reports/'+label+'-states.txt')).read_bytes()).hexdigest()
(root/('reports/'+label+'-stateful-rom-comparison.json')).write_text(json.dumps(report,indent=2));print('PASS',report)
