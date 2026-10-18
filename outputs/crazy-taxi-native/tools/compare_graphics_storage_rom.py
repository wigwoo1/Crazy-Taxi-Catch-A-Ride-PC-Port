"""Actual original bootstrap storage loops, excluding relocation and callees."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
for i in md.disasm(r[0x2ae5a:0x2aef4],0x0802ae5a):allowed_thumb[i.address]=bytes(i.bytes)
class StorageProbe(ExitProbe):
 def __init__(self,*args):self.graphics=bytearray([0xa5]*256);self.pool=bytearray([0xa5]*66048);super().__init__(*args)
 def read(self,a,size=4):
  if 0x03006810<=a<0x03006910:return int.from_bytes(self.graphics[a-0x03006810:a-0x03006810+size],'little')
  if 0x0201b6ec<=a<0x0202b8ec:return int.from_bytes(self.pool[a-0x0201b6ec:a-0x0201b6ec+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x03006810<=a<0x03006910:self.graphics[a-0x03006810:a-0x03006810+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  elif 0x0201b6ec<=a<0x0202b8ec:self.pool[a-0x0201b6ec:a-0x0201b6ec+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  else:super().write(a,v,size)
 def external(self,t):raise AssertionError(hex(t))
steps=0
for seed in [0,1,0x80000000,0xffffffff]:
 p=StorageProbe((0,0,0,0,0,1,0,0,0,0))
 for offset in [0x34,0x40,0xf4]:p.write(0x03006810+offset,seed)
 before=bytes(p.graphics);p.run(entry=0x0802ae5a,stop=0x0802aef4)
 expected=bytearray(before)
 for offset in [0x34,0x40,0xf4]:expected[offset:offset+4]=bytes(4)
 assert p.graphics==expected
 native=subprocess.run([str(root/'bin/graphics-storage-rom-compare.exe'),f'{seed:08x}'],capture_output=True,check=True).stdout
 assert native==bytes(12)+p.pool and len(native)==66060
 steps+=p.steps
report={'status':'PASS','cases':4,'actual_thumb_instructions':steps,'pool_words':[12288,4096,128],'scope':'Complete original AE5A..AEF2 three global zero stores and all pool/head loops; every pool byte compared to native, all unrelated graphics globals preserved. Allocation/relocation/callbacks/callees/GPU consumer layout excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/graphics-storage-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
