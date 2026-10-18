"""Full ordinary AC70..AF80 caller composition, opaque helper contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_restart_graphics_rom.py').read().split('\ncases=')[0])
for start,end in [(0xac70,0xacc6),(0xae48,0xaf0c),(0xaf5c,0xaf80)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
def fullstate(p):return [p.read(a,1) for a in [0x2001691,0x200153c,0x200153d]]+[p.read(a) for a in [0x20034d4,0x20034d8,0x200187c,0x2001804]]+[p.read(a,2) for a in [0x2007542,0x2007544,0x4000010,0x4000012]]+[p.read(0x200177b,1)]
setup_targets=[0x8014a58,0x8006580,0x800b6ec,0x800df8c,0x80227ac,0x8011490,0x802954c,0x8021c10]
class FlowProbe(GraphicsProbe):
 def read(self,a,size=4):
  if a in [0x4000010,0x4000012]:assert size==2;return self.io[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  if a in [0x4000010,0x4000012]:assert size==2;self.io[a]=v&65535;return
  return super().write(a,v,size)
 def external(self,target):
  used={0x8027730:0,0x8026ef8:1,0x801e72c:2,0x8027110:1,0x800f5e8:0,0x8025c18:2,0x8007378:0,0x80073c0:0,0x8027994:2,0x800fec8:6,0x800e154:3,0x800e5a8:4,0x802ac6c:7,0x802c834:1,0x802ca94:1,0x802c688:3,0x80203c4:0,0x8027fb8:5,0x8026440:0,0x800d764:0,0x8014a58:1,0x8006580:0,0x800b6ec:0,0x800df8c:0,0x80227ac:0,0x8011490:0,0x802954c:0,0x8021c10:0,0x802c9f0:4,0x8026318:0,0x801ee28:0,0x802277c:0}
  assert target in used,hex(target);n=used[target];args=[self.reg[j] if j<4 else self.read(self.reg[13]+4*(j-4)) for j in range(n)]+[0]*(7-n)
  self.calls.append([target,*args,*fullstate(self)])
  if target==0x802ac6c:self.write(0x2007542,240,2);self.write(0x2007544,160,2)
  if target==0x8026318:
   for a in [0x2007542,0x2007544]:self.write(a,(self.read(a,2)+1)&65535,2)
  if target==0x800e5a8:
   self.left=self.wait_in if self.fades==0 else self.wait_out if self.fades==1 else 1;self.fades+=1;self.write(0x200177b,int(self.left!=0),1)
  if target==0x800d764:self.left=max(0,self.left-1);self.write(0x200177b,int(self.left!=0),1)
  if target in setup_targets:
   self.setup_calls+=1;self.write(0x20034d4,(self.read(0x20034d4)+self.setup_calls)&0xffffffff);self.write(0x20034d8,self.read(0x20034d8)^self.setup_calls)
  self.reg[0]=self.random if target==0x801e72c else 0
cases=[(city,random,viewport,wi,wo) for city in [0,1] for random in [0,1,2,65535] for viewport in [0,128,255] for wi,wo in [(0,0),(1,2),(33,33)]]
cp=subprocess.run([str(root/'bin/normal-restart-flow-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (city,random,viewport,wi,wo),want in zip(cases,rows):
 p=FlowProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.io={};p.random=random;p.wait_in=wi;p.wait_out=wo;p.left=p.fades=p.setup_calls=0;p.reg[10]=1;p.reg[13]=0x10000400
 for a,v,size in [(0x2001708,0,1),(0x2001eb4,city,1),(0x2001780,viewport,1),(0x3004ca0,0x2002634,4),(0x200187c,123,4),(0x2001804,456,4),(0x20034d4,123,4),(0x20034d8,456,4),(0x2001691,99,1),(0x200153c,98,1),(0x200153d,97,1),(0x2007542,0,2),(0x2007544,0,2),(0x4000010,55,2),(0x4000012,77,2),(0x200177b,0,1),(0x10000424,0x2001790,4)]:p.write(a,v,size)
 p.run(entry=0x800ac70,stop=0x800af80)
 actual=[len(p.calls),*[v for call in p.calls for v in call],*fullstate(p)]
 assert actual==want,((city,random,viewport,wi,wo),actual[:41],want[:41]);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Full ordinary AC70..AF80 source caller compared with composed native prefix/graphics/setup/entry. Ordered40fixed helper requests plus actual loading fades0/1/2/33, seven used register/stack args and12 shared owners at every call/final. Opaque helper contracts include RNG return, viewport dimensions and mutating paired owners; helper bodies, actual shared RNG algorithm, graphics/music/runtime integration and playable Retry excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/normal_restart.c','src/normal_restart_graphics.c','src/normal_restart_entry.c','src/normal_restart_flow.c','src/normal_restart_flow.h','tests/normal_restart_flow_rom_compare.c','tools/compare_normal_restart_flow_rom.py']})
(root/'reports/normal-restart-flow-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
