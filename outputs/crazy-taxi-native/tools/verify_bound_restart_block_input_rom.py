from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_restart_block_reset_rom.py').read_text().split('\nseeds=')[0],ns)
class Probe(ns['BlockProbe']):
 def __init__(self,*args):
  self.observing=False;self.input_shadow=bytearray(66);super().__init__(*args)
 def read(self,a,size=4):
  if 0x20034dc<=a and a+size<=0x200351e:return int.from_bytes(self.input_shadow[a-0x20034dc:a-0x20034dc+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if self.observing:assert not(a<0x200351e and a+size>0x20034dc),(hex(a),size)
  if 0x20034dc<=a and a+size<=0x200351e:self.input_shadow[a-0x20034dc:a-0x20034dc+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little');return
  super().write(a,v,size)
rows=[]
for seed in [0,165,255]:
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.input_shadow=bytearray((seed+off*13)&255 for off in range(66));before=bytes(p.input_shadow);p.block=bytearray((seed+i*37)&255 for i in range(528));p.reset_words={0x2001768:seed,0x200176c:seed^0xabcdef01}
 p.observing=True;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800df8c,stop=0x10020000)
 assert before==bytes(p.input_shadow);assert p.reset_words=={0x2001768:0xffffffff,0x200176c:0};assert p.block[:512]==bytes(512)
 rows.append({'seed':seed,'instructions':p.steps})
report={'status':'PASS_COMPLETE_RESTART_BLOCK_FILL_INPUT_RETAINED','cases':rows,'rom_sha256':hashlib.sha256(ns['r']).hexdigest(),'scope':'WholeDF8C->DF9C(-1)->33F2C completeoriginal512bytefill, all66inputbytes retained/nooverlappingstores. NonresetDF9C path excluded; noexternalno-statecalleecontracts.'}
(root/'reports/bound-restart-block-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS original nested fill input retention',len(rows))
