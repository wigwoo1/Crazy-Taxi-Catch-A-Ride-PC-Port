"""Original bounded idle block against actual native consuming transitions."""
exec(open('outputs/crazy-taxi-native/tools/compare_random_range_rom.py').read().split('\ncases=')[0])
source=source.replace('while pc!=stop:','while pc not in stop if isinstance(stop,set) else pc!=stop:');exec(source);VehicleProbe.run=MotionProbe.run
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/random-range-cfg.json').read_text())['instructions']})
cfg=json.loads((root/'reports/passenger-idle-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class IdleProbe(RandomProbe):
 def read(self,a,n=4):
  if 0x2000000<=a<0x2040000 or 0x10000000<=a<0x10020000:
   assert a&~3 in self.arm.mem,('unmapped',hex(a));return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(8*n))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x2000000<=a<0x2040000 or 0x10000000<=a<0x10020000:
   sh=(a&3)*8;mask=((1<<(8*n))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x801e72c:self.calls+=1;self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
rows=[list(map(int,l.split())) for l in (root/'reports/current-passenger-idle-states.txt').read_text().splitlines()]
steps=0;stages={}
for index,row in enumerate(rows):
 selected,pending,x,z,delta,freeze,before,after,status=row[:9];pre=row[9:53];post=row[53:];assert len(post)==44 and not status
 p=IdleProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;actor=0x2005f2c;taxi=0x10003000
 for i,v in enumerate(pre):p.write(actor+i,v,1)
 for off in [0,4,8]:p.write(taxi+off,x if off==0 else z if off==8 else 0)
 p.write(0x2003b00,delta);p.write(0x2005f20,freeze,1);p.write(0x2003b04,before);p.write(0x2007530,actor if selected else 0)
 sl=int.from_bytes(r[0x22fcc:0x22fd0],'little');p.write(sl,pending)
 p.reg[7]=actor;p.reg[8]=taxi;p.reg[9]=actor;p.reg[10]=sl;p.write(p.reg[13]+0x38,0)
 p.run(entry=0x8022f34,stop={0x802318e,0x8023390,0x8023110})
 got=[p.read(actor+i,1) for i in range(44)]
 assert got==post and p.read(0x2003b04)==after and p.calls==1,dict(case=index,stage=pre[10],original=got,native=post,rng=p.read(0x2003b04),expected=after,calls=p.calls)
 stages[str(pre[10])]=stages.get(str(pre[10]),0)+1;steps+=p.steps
report=dict(cases=len(rows),stages=stages,thumb_instructions=steps,match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual original bounded22F34 idle/culling through explicit318E/3390/3110 boundary, nested original1E72C, all44customer bytes and RNG match actual native consuming calls. Entryloop registers/customerindex/delta/taxi/selected/pending/freeze supplied as captured fixtures. Rendering/boarding excluded; signed remainder helper contract. Not full passenger frame continuity.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_passenger_idle_live_rom.py','reports/passenger-idle-cfg.json','reports/current-passenger-idle-states.txt','src/passenger_idle.c']}
(root/'reports/passenger-idle-live-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
