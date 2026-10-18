from pathlib import Path
root=Path('outputs/crazy-taxi-native')
source=(root/'tools/verify_bound_object_setup_actual_copy_rom.py').read_text()
insert=source.index("ns['ColdProbe']=ActualInstalledCopy")
extra="""
class InputObservedCopy(ActualInstalledCopy):
 def __init__(self,*args):self.observing=False;self.input_shadow=bytearray((165+i*13)&255 for i in range(66));super().__init__(*args)
 def read(self,a,size=4):
  if 0x20034dc<=a and a+size<=0x200351e:return int.from_bytes(self.input_shadow[a-0x20034dc:a-0x20034dc+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if self.observing:assert not(a<0x200351e and a+size>0x20034dc),(hex(a),size)
  super().write(a,v,size)
ActualInstalledCopy=InputObservedCopy
"""
source=source[:insert]+extra+source[insert:]
source=source.replace("exec(body,ns)","body=body.replace('p.run(entry=0x8028bd0','p.observing=True;before_input=bytes(p.input_shadow);p.run(entry=0x8028bd0').replace('assert p.reg[13]==','assert before_input==bytes(p.input_shadow)\\n assert p.reg[13]==')\nexec(body,ns)")
source=source.replace('bound-object-setup-actual-copy-rom-comparison.json','bound-object-setup-copy-input-rom-comparison.json')
source=source.replace("Diagnosticstack synthetic; DMA remains logical;", "All66 input shadow bytes retained; overlapping caller and copy stores rejected. Diagnosticstack synthetic; DMA remains logical;")
exec(compile(source,'joined-original-copy-input-observer','exec'),{'__file__':str(Path(__file__).resolve())})
