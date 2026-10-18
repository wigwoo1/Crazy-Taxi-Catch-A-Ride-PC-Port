"""Execute original1FB7C startup draw block with explicit draw callback boundary."""
from pathlib import Path
import itertools,subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_customer_dispatch_rom.py').read_text().split('\ncases=')[0],ns)
for a,b in [(0x1fb7c,0x1fc54),(0x1fc74,0x1fd1a),(0x1fdcc,0x1fdd4)]:
 for i in ns['md'].disasm(ns['r'][a:b],0x8000000+a):ns['allowed_thumb'][i.address]=bytes(i.bytes)
class ArtProbe(ns['DispatchProbe']):
 def external(self,t):
  assert t in [0x8028288,0x8028658],hex(t)
  frame,group,pos,flags=self.reg[:4];assert group==7 and flags==0
  sp=self.reg[13];assert [self.read(sp+j*4) for j in range(3)]==[0,4,0]
  x=ns['signed'](self.read(pos));y=ns['signed'](self.read(pos+4));assert self.read(pos+8)==0
  scale=ns['signed'](self.read(sp+12)) if t==0x8028658 else 0
  if t==0x8028658:assert self.read(sp+16)==0
  self.calls.extend([frame,int(t==0x8028658),x,y,scale])
cases=list(itertools.product([0,1],[-32768,-1,0,1,2,3,32767],[0,240,65535],[-32768,-1,0,120,32767],[-32768,-7,0,255,256,512,513,32767]))
cp=subprocess.run([str(root/'bin/start-art-submission-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 active,phase,width,offset,scale=case;p=ArtProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
 for a,v,n in [(0x2001696,active,1),(0x20016b4,phase,2),(0x2007542,width,2),(0x20016b8,offset,2),(0x20016ba,scale,2)]:p.write(a,v,n)
 p.reg[13]=0x10000400;p.run(entry=0x801fb7c,stop=0x801fd24 if not active else 0x80202b6)
 assert [len(p.calls)//5,*p.calls]==want,(case,p.calls,want);steps+=p.steps
report=dict(status='PASS_ORIGINAL_START_ART_SUBMISSION',cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(ns['r']).hexdigest(),scope='Original1FB7C block sliding/affine/suppressed paths matches compiled native ordered frame/group/position/scale requests.28288/28658 are callback boundaries; stack frame supplied explicitly. Original full1F0E8 ownership, OAM/pixels/shader and4K60 excluded.')
(root/'reports/start-art-submission-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
