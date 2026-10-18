from pathlib import Path
import json,itertools,subprocess,types,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/verify_bound_result_resource_input_shadow_rom.py').read_text().split('\nrows=[]')[0],ns)
cases=list(itertools.product((0,1),(0,128),range(5),range(4),range(4)))
rompath='Crazy Taxi - Catch a Ride (USA).gba'
out=subprocess.run([str(root/'bin/hud-start-rom-compare.exe'),rompath],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True,timeout=30).stdout.split('END\n');assert len(out)==len(cases)+1
steps=0
for case,output in zip(cases,out):
 demo,settings,driver,challenge,session=case;p=ns['Probe']((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray([165])*262144,0x3000000:bytearray([165])*65536};p.scalars={};p.requests=[]
 for a,v in ((0x2001708,demo),(0x2001eb0,settings),(0x200388d,driver),(0x2001539,challenge),(0x2001690,session)):p.write(a,v,1)
 before=bytes(p.regions[0x2000000][0x34dc:0x351e]);writes={}
 oldwrite=p.write
 def write(self,a,v,size=4):
  if 0x2000000<=a<0x4000000:
   for j in range(size):writes[a+j]=(v>>(8*j))&255
  oldwrite(a,v,size)
 def external(self,t):
  if t==0x801e784:self.run(entry=t,stop=self.reg[14]&~1)
  else:
   assert t==0x8027fb8;p.requests.append(self.reg[:4]+[self.read(self.reg[13])])
 p.write=types.MethodType(write,p);p.external=types.MethodType(external,p);p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x801ee28,stop=0x10020000)
 native={};requests=[]
 for line in output.splitlines():
  tokens=line.split();values=list(map(int,tokens[1:]))
  if tokens[0]=='C':requests.append(values)
  else:
   assert tokens[0]=='W';a,v,n=values
   for j in range(n):native[a+j]=(v>>(8*j))&255
 assert writes==native,dict(case=case,differences=[hex(a) for a in set(writes)|set(native) if writes.get(a)!=native.get(a)])
 assert requests==p.requests,(case,requests,p.requests)
 assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]);steps+=p.steps
report=dict(status='PASS',cases=len(cases),instructions=steps,rom_sha256=hashlib.sha256(ns['ns']['r']).hexdigest() if 'ns' in ns else hashlib.sha256(ns['r']).hexdigest(),scope='Original whole1EE28 caller plus actual1E784 fills versus compiled native hud_start.256 demo/ordinary/challenge and independently varied three driver owners; all written owner bytes and ordered five27FB8 requests including stackarg exact, input retained.27FB8 no-state request boundary; native graphics replacement effects, VRAM, HUD pixels and playable integration excluded.')
(root/'reports/hud-start-language-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'native HUD initializer cases',steps,'original instructions')
