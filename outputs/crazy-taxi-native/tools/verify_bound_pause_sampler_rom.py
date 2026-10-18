from pathlib import Path
import os,subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');rep=root/'reports'
ns={};exec((root/'tools/compare_input_sampler_rom.py').read_text().split('\ncases=')[0],ns)
def read_state(p):
 values=[p.read(0x20034de,2),p.read(0x20034dc,2),p.read(0x20034e0,2)]
 for i in range(10):
  values.extend([p.read(a+i,n) for a,n in [(0x20034e2,1),(0x20034ec,1),(0x20034f6,1),(0x2003500,1)]]+[p.read(0x200350a+i*2,2)])
 return values
cases=[]
for name,ticks in [('held',350),('resume',450),('restart',450),('quit',450)]:
 keys=[0]*ticks;keys[299]=8
 if name=='resume':keys[359]=8
 if name in ['restart','quit']:
  for n,key in [(305,128),(310,1),(315,64),(320,1)]:keys[n-1]=key
  if name=='quit':keys[306]=128
 key=rep/f'bound-pause-sampler-{name}-keys.txt';step=rep/f'bound-pause-sampler-{name}-ticks.txt';key.write_text('\n'.join(map(str,keys))+'\n');step.write_text('initial_delta 1\n'+'1\n'*ticks)
 env=os.environ.copy();env['TAXI_BOUND_PAUSE_TRACE']='1';env['TAXI_BOUND_SAMPLER_TRACE']='1'
 run=subprocess.run([str(root/'bin/scene-viewer-bound-pause-sampler-capture.exe'),'--challenge-bound-drive-preview','--gameplay-key-replay',str(key),'--gameplay-step-replay',str(step),'--ticks',str(ticks),'--snapshot',str(rep/f'bound-pause-sampler-{name}.bmp')],env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 (rep/f'bound-pause-sampler-{name}.log').write_text(run.stdout);(rep/f'bound-pause-sampler-{name}.err').write_text(run.stderr);assert run.returncode==0 and not run.stderr,(name,run.stderr)
 p=None;samples=0;checks=0;reset_count=0;discontinuities=[];steps=0;last=None
 for line in run.stdout.splitlines():
  if line.startswith('BOUND SAMPLER RESET:'):
   if p is not None:
    steps+=p.steps;discontinuities.append({'after_sample':samples,'previous_state':read_state(p),'next_reset':'native whole-input zero; original restart reset NOT yet qualified'})
   p=ns['InputProbe']((0,0,0,0,0,1),0)
   for a in range(0x20034dc,0x2003520,4):p.arm.mem[a]=0
   reset_count+=1;last=None
  elif line.startswith('BOUND SAMPLER:'):
   row=list(map(int,line.split(':',1)[1].split()));assert len(row)==109
   assert p is not None;raw,delta,flag=row[:3];before=row[3:56];after=row[56:]
   assert read_state(p)==before,(name,samples,'before',read_state(p),before)
   if last is not None:checks+=53
   p.arm.mem[0x4000130]=raw;p.arm.mem[0x2001ea8]=0;p.arm.mem[0x2001708]=0;p.arm.mem[0x200170c]=0;p.arm.mem[0x2003b00]=delta;p.reg[14]=0x10020001
   p.run(entry=0x8015c3c,stop=0x10020000)
   assert read_state(p)==after,(name,samples,'after',read_state(p),after)
   last=after;samples+=1
 steps+=p.steps
 assert reset_count==(2 if name=='restart' else 1),(name,reset_count)
 assert samples==(450 if name=='resume' else 350 if name=='held' else 418 if name=='restart' else 320),(name,samples)
 if name=='restart':assert len(discontinuities)==1 and any(discontinuities[0]['previous_state'])
 cases.append({'case':name,'samples':samples,'reset_count':reset_count,'retained_fields':checks,'instructions':steps,'native_reset_discontinuities':discontinuities});print('PASS',name,samples,'resets',reset_count,flush=True)
report={'status':'PASS_ORIGINAL_INPUT_SAMPLER_WITH_EXPLICIT_NATIVE_RESET_BOUNDARIES','cases':cases,'scope':'Original15C3C continuous input state53fields at every captured native sampler call; no reseeding inside segments. Native whole-input reset splits Restart into2segments, NOT evidence that original Restart clears input; parent/callee ownership unresolved. Recorded rawkey/delta contracts, not hardware or original complete cadence.'}
(rep/'bound-pause-sampler-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
