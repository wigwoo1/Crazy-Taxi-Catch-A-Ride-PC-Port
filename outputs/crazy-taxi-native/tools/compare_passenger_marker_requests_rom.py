exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
import itertools,subprocess
for i in md.disasm(r[0x23304:0x23390],0x8023304):allowed_thumb[i.address]=bytes(i.bytes)
class MarkerProbe(IdleProbe):
 def external(self,target):
  assert target==0x8031a50,hex(target)
  a=self.reg[1];self.requests.append([self.reg[0],signed(self.read(a)),signed(self.read(a+4)),signed(self.read(a+8)),self.reg[2],self.reg[3],self.read(self.reg[13])])
cases=[(*c,*xyz) for c in itertools.product([0,1,3,5,65,255],[0,4,255],[0,1,3,127,128,255],[0,1],[0,1]) for xyz in [(123,-456,789),(2147483647,2147483600,-2147483648)]]
cp=subprocess.run([str(root/'bin/passenger-marker-requests-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0;requests=0
for c,expected in zip(cases,rows):
 flags,kind,phase,pending,active,x,y,z=c;p=MarkerProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];actor=0x2005f2c
 for n in range(44):p.write(actor+n,0,1)
 for off,v,n in [(0,flags,1),(2,kind,1),(16,x,4),(20,y,4),(24,z,4)]:p.write(actor+off,v,n)
 p.write(0x2001704,pending);p.write(0x2001694,active,1);p.write(0x2005f29,phase,1);p.reg[5]=actor;p.reg[10]=0x2001704
 p.run(entry=0x8023304,stop=0x8023390)
 actual=[len(p.requests)]+[v for row in p.requests for v in row];assert actual==expected,(c,actual,expected);steps+=p.steps;requests+=len(p.requests)
report={'cases':len(cases),'requests':requests,'thumb_instructions':steps,'match':True,'scope':'Original normal23304..2338C complete caller to3390 with31A50 captured request boundary; descriptors/XYZ/scale/flip/stackflags exact. Controlled actor/pending/active/phase fixtures; no cull, projection, marker pixels, full22DF4 proof.'}
(root/'reports/passenger-marker-requests-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
