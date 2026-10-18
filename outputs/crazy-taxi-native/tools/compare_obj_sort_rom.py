"""Complete original sorter and paired swap versus native; diagnostic only."""
exec(open('outputs/crazy-taxi-native/tools/compare_smoke_rom.py').read().split('\nclock=')[0])
source=source.replace("   elif m=='pop':", """   elif m in ['ldm','stm']:
    base_reg=rn(ops[0]);address=self.reg[base_reg];registers=sorted(rn(o) for o in ops[1:])
    for j,k in enumerate(registers):
     if m=='ldm':self.reg[k]=self.read(address+4*j)
     else:self.write(address+4*j,self.reg[k])
    self.reg[base_reg]=(address+4*len(registers))&MASK
   elif m=='pop':""")
exec(source);VehicleProbe.run=MotionProbe.run
allowed_thumb={}
for name in ['obj-sort','obj-swap']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class SortProbe(VehicleProbe):
 def read(self,a,n=4):
  if 0x10008000<=a<0x10010000:return (self.arm.mem.get(a&~3,0)>>((a&3)*8))&((1<<(8*n))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x10008000<=a<0x10010000 or 0x20089ec<=a<0x20089ec+1024 or 0x2008dec<=a<0x2008dec+1024:
   old=self.arm.mem.get(a&~3,0);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x80280c4,0x8028e88]:self.run(entry=target,stop=self.reg[14]&~1)
  else:raise AssertionError(hex(target))
cases=[[],[(1,5),(2,5)],[(1,5),(2,5),(3,5)],[(1,5)]*6,[(-20,500),(20,500),(-26,290),(266,290),(70,709),(170,709)]]
import random
rng=random.Random(280)
for n in [1,2,3,4,6,10,32,128]:
 for trial in range(12):cases.append([(rng.choice([-32768,-1,0,1,32767]),rng.choice([-32768,-1,0,1,32767])) for i in range(n)])
res=subprocess.run([str(root/'bin/obj-sort-rom-compare.exe')],input=''.join(str(len(c))+' '+ ' '.join(str(v) for p in c for v in p)+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases)
steps=0
for case,row in zip(cases,rows):
 p=SortProbe((0,0,0,0,0,1,0,0,0,0))
 for i,(x,z) in enumerate(case):
  for j,v in enumerate([x,i,z,100+i]):p.write(0x2008dec+8*i+2*j,v,2)
  for j in range(4):p.write(0x20089ec+8*i+2*j,i*4+j+1000,2)
 p.reg[0]=0;p.reg[1]=(len(case)-1)&MASK;p.run(entry=0x80280c4,stop=0x10020000)
 got=[]
 for i in range(len(case)):
  for j in range(4):v=p.read(0x2008dec+8*i+2*j,2);got.append(v-65536 if v&32768 else v)
  for j in range(4):got.append(p.read(0x20089ec+8*i+2*j,2))
 assert got==row,dict(case=case,original=got,native=row);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original280C4 nestsactual28E88 pairedswap vs native all anchors and attributes, ties/signedboundaries/count0..128; controlled fixtures, not fullframe/render oracle.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/obj_order.c',root/'tests/obj_sort_rom_compare.c',root/'tools/compare_obj_sort_rom.py',root/'bin/obj-sort-rom-compare.exe']}
(root/'reports/obj-sort-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
