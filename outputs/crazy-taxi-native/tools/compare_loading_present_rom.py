"""Whole D764 caller, helper bodies explicit contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0xd764:0xd8a8],0x800d764):allowed_thumb[i.address]=bytes(i.bytes)
class LoadingProbe(ExitProbe):
 def external(self,target):
  used={0x802c9f0:4,0x8028fe4:1,0x800f7dc:9,0x8028288:7,0x8031ef8:5,0x8027fb8:5,0x8026318:0}
  assert target in used,hex(target);count=used[target]
  args=[self.reg[i] if i<4 else self.read(self.reg[13]+4*(i-4)) for i in range(count)]+[0]*(9-count)
  pos=[0]*3
  if target==0x8028288:pos=[self.read(args[2]+4*i) for i in range(3)];args[2]=0
  self.calls.append([target,*args,*pos]);self.reg[0]=0x8400000+args[0]*16 if target==0x8028fe4 else 0
cases=[(a,b,w,h,animation) for a,b in [(0,0),(0x60,0),(0x80,0),(0x9f,0),(0,1)] for w,h in [(240,160),(0,0),(65535,65535)] for animation in [0,127,128,255]]
cp=subprocess.run([str(root/'bin/loading-present-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (a,b,w,h,animation),want in zip(cases,rows):
 p=LoadingProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 for addr,value,size in [(0x2007542,w,2),(0x2007544,h,2),(0x2001eb0,a,1),(0x2001708,b,1),(0x2001684,animation,1)]:p.write(addr,value,size)
 p.run(entry=0x800d764,stop=0x10020000)
 actual=[len(p.calls),*[v for row in p.calls for v in row]]
 assert actual==want,((a,b,w,h,animation),actual,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='WholeD764 source caller: clear, bypass gates, two lookup/text calls all9args, three mutable-stack-position sprite requests, three64x64 image requests, signed animation sprite identity, final sprite position120/24/0 and26318. Helpers explicit returning contracts, no pixels/fade/timing/audio or whole attract lifecycle proof.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/loading_present.c','src/loading_present.h','tests/loading_present_rom_compare.c','tools/compare_loading_present_rom.py']})
(root/'reports/loading-present-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
