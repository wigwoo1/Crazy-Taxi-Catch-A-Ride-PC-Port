from pathlib import Path
import json,struct,hashlib
root=Path('outputs/crazy-taxi-native')
base=(root/'tools/compare_startup_obj_setup_rom.py').read_text();prefix,body=base.split('\ncases=',1)
ns={'__file__':str(Path(__file__).resolve())};exec(prefix,ns)
r=ns['r'];expected=[0xe92d0ff0]+[w for _ in range(32) for w in (0xe8b00ff0,0xe8a10ff0)]+[0xe8bd0ff0,0xe12fff1e]
assert list(struct.unpack('<67I',r[0x5f4:0x700]))==expected
Parent=ns['ColdProbe'];copies=[]
class ActualInstalledCopy(Parent):
 def external(self,t):
  if t!=0x8032454:return super().external(t)
  src,dst=self.reg[:2];assert (src,dst) in ((0x20081ec,0x7000000),(0x2009508,0x20081ec))
  sp=self.reg[13];saved=self.reg[4:12].copy();before=bytes(self.read(src+i,1) for i in range(1024));stack={};writes=[]
  regs=self.reg.copy();pc=0x80005f4;steps=0
  while pc!=0x8000700:
   w=struct.unpack_from('<I',r,pc-0x8000000)[0];steps+=1
   if w==0xe92d0ff0:
    regs[13]-=32
    for j in range(8):stack[regs[13]+4*j]=regs[j+4]
   elif w==0xe8b00ff0:
    regs[4:12]=[self.read(regs[0]+4*j) for j in range(8)];regs[0]+=32
   elif w==0xe8a10ff0:
    for j in range(8):
     a=regs[1]+4*j;assert dst<=a<dst+1024 and not(a<0x200351e and a+4>0x20034dc);self.write(a,regs[j+4]);writes.append(a)
    regs[1]+=32
   elif w==0xe8bd0ff0:regs[4:12]=[stack[regs[13]+4*j] for j in range(8)];regs[13]+=32
   elif w==0xe12fff1e:assert pc==0x80006fc
   else:raise AssertionError(hex(w))
   pc+=4
  assert regs[0]==src+1024 and regs[1]==dst+1024 and regs[13]==sp and regs[4:12]==saved and len(writes)==256
  assert not(sp-32<0x200351e and sp>0x20034dc)
  assert bytes(self.read(dst+i,1) for i in range(1024))==before
  self.reg[:]=regs;self.copies.append((src,dst));copies.append(dict(source=hex(src),destination=hex(dst),instructions=steps,diagnostic_stack=hex(sp)))
ns['ColdProbe']=ActualInstalledCopy
body='cases='+body
body=body.replace('startup-obj-setup-rom-comparison.json','bound-object-setup-actual-copy-rom-comparison.json')
exec(body,ns)
report=ns['report'];report['installed_copy_calls']=len(copies);report['installed_copy_instructions']=sum(c['instructions'] for c in copies);report['copy_arguments']=list({(c['source'],c['destination'],c['diagnostic_stack']) for c in copies});report['scope']='Whole28BD0/27CD0 cold and active with actual280C4/28E88. All5504 native owned bytes/counters exact across144 fixtures. All installed copy requests execute exact ROM05F4 fixed1024-byte ARM body,67 instructions, bounded destination plus32-byte scratch stack and callee saved registers verified. Installed bytes checked against original ROM; execute ROM identity offline, not relocated gameplay address or CPU in runtime. Diagnosticstack synthetic; DMA remains logical; not complete bootstrap/liveframe/hardware.'
(root/'reports/bound-object-setup-actual-copy-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(copies),'original installed-copy calls;',report['cases'],'native objectsetup cases')
