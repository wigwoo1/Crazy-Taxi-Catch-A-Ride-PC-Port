from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');base=(root/'tools/compare_startup_obj_setup_rom.py').read_text();insert=base.index('\ncases=')
extra="""
class ObservedColdProbe(ColdProbe):
 def __init__(self,*args):self.observing=False;self.input_shadow=bytearray((165+i*13)&255 for i in range(66));super().__init__(*args)
 def read(self,a,size=4):
  if 0x20034dc<=a and a+size<=0x200351e:return int.from_bytes(self.input_shadow[a-0x20034dc:a-0x20034dc+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if self.observing:assert not(a<0x200351e and a+size>0x20034dc),(hex(a),size)
  if 0x20034dc<=a and a+size<=0x200351e:self.input_shadow[a-0x20034dc:a-0x20034dc+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little');return
  super().write(a,v,size)
ColdProbe=ObservedColdProbe
"""
code=base[:insert]+extra+base[insert:]
code=code.replace('p.run(entry=0x8028bd0','p.observing=True;before_input=bytes(p.input_shadow);p.run(entry=0x8028bd0')
code=code.replace('assert p.reg[13]==', 'assert before_input==bytes(p.input_shadow)\n assert p.reg[13]==')
code=code.replace('startup-obj-setup-rom-comparison.json','bound-object-setup-input-rom-comparison.json')
exec(compile(code,'whole-object-setup-input-observer','exec'),{'__file__':str(Path(__file__).resolve())})
p=root/'reports/bound-object-setup-input-rom-comparison.json';report=json.loads(p.read_text());report['scope']+=' Input shadow66 bytes retained withnooverlappingCPU/DMAstores inallcases. Logicalinstalled1024copy source/destinationoriginalknown81EC/OAM/9508 remaincontracts; completebootstrap/hardware excluded.';p.write_text(json.dumps(report,indent=2)+'\n')
