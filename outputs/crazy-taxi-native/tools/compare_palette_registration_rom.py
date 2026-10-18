"""Complete original E154 registration, DMA register request boundary explicit."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
todo=[0x0800e154];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x0800e154<=a<=0x0800e380,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class RegistrationProbe(ExitProbe):
 def __init__(self,*args):self.records=bytearray([0xa5]*2560);self.palette=bytearray(512);self.meta={};self.dma={};super().__init__(*args)
 def read(self,a,size=4):
  if a in self.meta:return self.meta[a]
  if 0x02001790<=a<0x02002190:return int.from_bytes(self.records[a-0x02001790:a-0x02001790+size],'little')
  if 0x084a5a34<=a<0x084a5c34:return int.from_bytes(self.palette[a-0x084a5a34:a-0x084a5a34+size],'little')
  if a in self.dma:return self.dma[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  if a in [0x02001770,0x02001780,0x02001782,0x02001784]:self.meta[a]=v&((1<<(size*8))-1)
  elif 0x02001790<=a<0x02002190:self.records[a-0x02001790:a-0x02001790+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  elif a in [0x040000d4,0x040000d8,0x040000dc]:self.dma[a]=v&MASK
  else:super().write(a,v,size)
 def external(self,t):raise AssertionError(hex(t))
cases=[(flags,mode,cap,count,0,0,131) for flags in range(8) for mode in [0,1] for cap,count in [(0,0),(32,1),(255,127)]]
cases += [(4,mode,cap,0,0,0,0) for mode in [255,256,257] for cap in [1,33,256]]
cases += [(4,mode,32,1,bg,obj,0) for mode in [0,1] for bg,obj in [(1,0),(0,1),(65535,65535),(512,512)]]
run=subprocess.run([str(root/'bin/palette-registration-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=run.stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 flags,mode,cap,count,bg,obj,seed=case;parts=row.split();expected=[int(v) for v in parts[:4]]+[int(v,16) for v in parts[4:7]];table=bytes(int(v,16) for v in parts[7:])
 probe=RegistrationProbe((0,0,0,0,0,1,0,0,0,0));probe.records=bytearray([0xa5]*2560)
 for i in range(256):probe.palette[i*2:i*2+2]=((i*131+seed)&65535).to_bytes(2,'little')
 probe.meta={0x02001770:flags,0x02001780:count,0x02001782:bg,0x02001784:obj};probe.dma={}
 probe.reg[:3]=[0x084a5a34,cap,mode];probe.reg[13]=0x10000400;probe.reg[14]=0x10020001
 probe.run(entry=0x0800e154,stop=0x10020000)
 transfers=[probe.dma.get(0x040000d4,0x11111111),probe.dma.get(0x040000d8,0x22222222),probe.dma.get(0x040000dc,0x33333333)]
 actual=[signed(probe.reg[0]),probe.meta[0x02001780],probe.meta[0x02001782],probe.meta[0x02001784]]+transfers
 assert actual==expected and bytes(probe.records)==table,(case,actual,expected)
 steps+=probe.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Complete E154 metadata/extrema/count/cursor/return/DMA-request stores, all2560record bytes compared with untouchedpadding and nonselected records retained. Eight flags, low-byte modes/caps, valid record count0/1/127, palette-space rejection. DMA transfer execution/hardware palette contents excluded; invalid native owners/capacity guard outside source valid domain. Palette values explicit deterministic fixtures.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/palette-registration-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
