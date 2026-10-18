"""Complete actual2CA94 material override, valid signed-byte selections."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
for i in md.disasm(r[0x2ca94:0x2cada],0x0802ca94):allowed_thumb[i.address]=bytes(i.bytes)
assert int.from_bytes(r[0x2cae0:0x2cae4],'little')==0x02001790
class MaterialProbe(ExitProbe):
 def __init__(self,*args):self.table=bytearray(2560);self.lookup=bytearray([0xa5]*2560);super().__init__(*args)
 def read(self,a,size=4):
  if 0x02001790<=a<0x02002190:return int.from_bytes(self.table[a-0x02001790:a-0x02001790+size],'little')
  if 0x0202baec<=a<0x0202c4ec:return int.from_bytes(self.lookup[a-0x0202baec:a-0x0202baec+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x02001790<=a<0x02002190:self.table[a-0x02001790:a-0x02001790+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  elif 0x0202baec<=a<0x0202c4ec:self.lookup[a-0x0202baec:a-0x0202baec+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  else:super().write(a,v,size)
 def external(self,t):raise AssertionError(hex(t))
steps=0;cases=0
for index in [0,1,2,63,127,0x100,0x8000007f,0xffffff01]:
 for seed in [0,241]:
  probe=MaterialProbe((0,0,0,0,0,1,0,0,0,0));probe.table=bytearray((i*37+j+seed)&255 for i in range(128) for j in range(20))
  prior=bytes(probe.table);probe.reg[0]=index;probe.reg[13]=0x10000400;probe.reg[14]=0x10020001
  probe.run(entry=0x0802ca94,stop=0x10020000)
  native=subprocess.run([str(root/'bin/restart-material-rom-compare.exe'),f'{index:x}',f'{seed:x}'],capture_output=True,check=True).stdout
  assert native==probe.lookup and len(native)==2560 and probe.table==prior
  steps+=probe.steps;cases+=1
report={'status':'PASS','cases':cases,'actual_thumb_instructions':steps,'scope':'Complete original2CA94 no helpers, all2560lookup bytes exact, registered20-byte records retained, low-byte valid indices0..127 with upper-bit aliases. Source negative/out-of-table reads excluded; native guards reject invalid owners/indices. Full source table registration and host restart integration excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/restart-material-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
