"""Actual D764 clear/gates/return; ordinary content body explicitly excluded."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for start,end in [(0xd764,0xd796),(0xd89c,0xd8a8)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class LoadingProbe(ExitProbe):
 def external(self,target):
  assert target==0x802c9f0,hex(target)
  self.calls.append([target,*self.reg[:4]]);self.reg[0]=0
cases=[(a,b,w,h) for a in range(256) for b in [0,1,128,255] for w,h in [(240,160),(0,65535)]]
cp=subprocess.run([str(root/'bin/loading-present-bypass-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (a,b,w,h),want in zip(cases,rows):
 p=LoadingProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.reg[13]=0x10000400;p.reg[14]=0x800d8a9
 for addr,value,size in [(0x2007542,w,2),(0x2007544,h,2),(0x2001eb0,a,1),(0x2001708,b,1)]:p.write(addr,value,size)
 p.run(entry=0x800d764,stop={0x800d796,0x800d8a8})
 actual=[int(p.stopped_pc==0x800d8a8),len(p.calls),*p.calls[0]]
 assert actual==want,((a,b,w,h),actual,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual D764 entry clear arguments and post-clear gates, complete D89C epilogue for override or special mode. Ordinary D796..D89C content excluded; clear helper returning contract, no pixels/frame/fade/audio proof.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/loading_present_bypass.c','src/loading_present_bypass.h','tests/loading_present_bypass_rom_compare.c','tools/compare_loading_present_bypass_rom.py']})
(root/'reports/loading-present-bypass-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)

