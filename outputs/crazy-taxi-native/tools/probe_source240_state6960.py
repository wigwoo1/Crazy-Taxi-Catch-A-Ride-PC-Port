"""Observe shared6960 use in the recovered baseline background/scenery consumer."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_clean_render_memory.py').read_text().split('\np=OneFrameblockScene();')[0])
class StateProbe(OneFrameblockScene):
 def __init__(self,value):
  self.observe=False;self.state_reads=0;self.state_writes=[];super().__init__();self.write(0x03006960,value);self.observe=True
 def read(self,a,size=4):
  if self.observe and a<=0x03006960<a+size:self.state_reads+=1
  return super().read(a,size)
 def write(self,a,value,size=4):
  if self.observe and a<=0x03006960<a+size:self.state_writes.append({'value':value,'size':size})
  return super().write(a,value,size)
results=[]
for value in [0,0xa5a5a5a5,0xffffffff]:
 p=StateProbe(value)
 try:
  p.run(0x0802b6a0);fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert len(p.requests)==37 and p.counts==[8660,8006,73,2121] and fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
  results.append({'initial_value':f'{value:08x}','status':'PASS','direct_state_reads':p.state_reads,'state_writes':p.state_writes})
 except AssertionError as error:results.append({'initial_value':f'{value:08x}','status':'DEPENDENCY_FOUND','assertion':str(error),'direct_state_reads':p.state_reads,'state_writes':p.state_writes})
report={'status':'PASS' if all(x['status']=='PASS' for x in results) else 'DEPENDENCY_FOUND','cases':results,'scope':'Background/scenerybaseline actualcopiedrenderer withthree6960values andsharedmemorycalltrace, all37intermediates comparednative. Pointerword inframeblock mayberead withoutdereferencingstate; no semantics/generalactor/shadedraster owner inferred.'}
(root/'reports/source240-state6960-observation.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
