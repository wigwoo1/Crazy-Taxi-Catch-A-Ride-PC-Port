from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');base=(root/'tools/compare_bound_actual_startup_owner_rom.py').read_text();insert=base.index('rows=')
extra="""
class InputShadowProbe(ns['SceneProbe']):
 def __init__(self,*args):
  self.observing=False;self.input_shadow=bytearray(66);super().__init__(*args)
  self.input_shadow=bytearray((input_seed+off*13)&255 for off in range(66))
 def read(self,a,size=4):
  if 0x20034dc<=a and a+size<=0x200351e:return int.from_bytes(self.input_shadow[a-0x20034dc:a-0x20034dc+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if self.observing:assert not (a<0x200351e and a+size>0x20034dc),('startup input write',hex(a),size)
  if 0x20034dc<=a and a+size<=0x200351e:self.input_shadow[a-0x20034dc:a-0x20034dc+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little');return
  super().write(a,v,size)
ns['SceneProbe']=InputShadowProbe
"""
source=base[:insert]+extra+base[insert:]
source=source.replace('p.run(entry=0x08014a58','p.observing=True;before_input=bytes(p.input_shadow);p.run(entry=0x08014a58')
source=source.replace('actual=bytes(p.regions[a]);','assert bytes(p.input_shadow)==before_input;actual=bytes(p.regions[a]);')
results=[]
for name in ['preview','retry','failure']:
 for seed in [0,165,255]:
  code=source.replace('bound-whole-startup-preview-owner.txt',f'bound-whole-startup-{name}-owner.txt').replace('bound-whole-startup-owner-rom-comparison.json',f'bound-startup-input-shadow-{name}-{seed}.json')
  env={'input_seed':seed,'__file__':str(Path(__file__).resolve())};exec(compile(code,'whole-vehicle-input-shadow-qualification','exec'),env);results.append({'case':name,'input_seed':seed,'calls':len(env['rows'])})
report={'status':'PASS_ACTUAL_BOUND_WHOLE_VEHICLE_START_INPUT_RETAINED','cases':results,'calls':sum(x['calls'] for x in results),'scope':'Wholeoriginal14A58 withcomplete11804/11540/1E7F0/1E784 andrealBoundmarker/groundcapture; all3744actorbytes match native and66inputbytes retained/nooverlappingstores on12cold/warmfixturecalls. String/RNG/arithmetic/containment contracts remainexplicit; entirebootstrapothercallees/scheduling unqualified.'}
(root/'reports/bound-startup-input-shadow-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS input shadow12actualcalls')
