"""Original complete6BC0 with actual recorded cell/neighbor/RNG spawn inputs."""
exec(open('outputs/crazy-taxi-native/tools/compare_random_range_rom.py').read().split('\ncases=')[0])
for name in ['traffic-spawn','traffic-path-initialize']:
 allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/f'reports/{name}-cfg.json').read_text())['instructions']})
class SpawnProbe(RandomProbe):
 def read(self,a,n=4):
  if 0x2000000<=a<0x2040000 or a in [0x3004c98,0x3006810,0x3006940]:
   assert a&~3 in self.arm.mem,('unmapped',hex(a));return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(n*8))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x2000000<=a<0x2040000 or a==0x3004c98:
   sh=(a&3)*8;mask=((1<<(n*8))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x802e190,0x801e72c]:
   if target==0x801e72c:self.calls+=1
   self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032454:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  elif target==0x8032520:
   a,b=map(signed,self.reg[:2]);self.reg[0]=(a-trunc(a,b)*b if b else 0)&MASK
  else:raise AssertionError(('unexpected spawn call',hex(target)))
world=0x87556cc;descriptors=struct.unpack_from('<I',r,0x7556cc+0x34)[0];grid=root/'assets/traffic/city1-grid.ctg';g=grid.read_bytes()
rows=[list(map(int,l.split())) for l in (root/'reports/current-traffic-spawn-states.txt').read_text().splitlines()];steps=0
mapping=[(0,4,0),(4,4,1),(0x10,4,2),(0x14,4,3),(0x1c,4,4),(0x20,4,5),(0x58,4,7),(0x40,4,8),(0x48,4,9),(0x4c,4,10),(0x54,4,11),(0x18,4,12),(0x28,4,13),(0x2c,4,14),(0x34,4,15),(0x38,4,16),(0x3c,2,17)]
for index,a in enumerate(rows):
 count,slot,spacing,cell,route,phase,seed,counter=a[:8];assert len(a)==34+18*count
 settings=[i for i in range(16) if r[0x4a54b0+i]==count and r[0x4a54b9+i]==spacing];assert len(settings)==1
 ptr=struct.unpack_from('<I',g,40+cell*28)[0];assert g[40+cell*28+4:40+cell*28+28]==r[ptr-0x8000000:ptr-0x8000000+24]
 p=SpawnProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;base=0x20000f4
 for i in range(count):
  actor=base+i*0x5c;values=a[13+i*18:13+(i+1)*18]
  for off in range(0,0x5c,4):p.arm.mem[actor+off]=0
  for off,n,j in mapping:p.write(actor+off,values[j],n)
  desc=descriptors+values[6]*20;points=struct.unpack_from('<I',r,desc-0x8000000+16)[0]
  p.write(actor+0x24,desc);p.write(actor+0x30,points+values[14]*16)
 p.arm.mem[0x2003b04]=seed;p.arm.mem[0x3004c98]=counter;p.arm.mem[0x3006810]=world;p.arm.mem[0x3006940]=0x8032489;p.arm.mem[0x2001eb0]=(settings[0]<<4)<<24
 actor=base+slot*0x5c;p.reg[:2]=[actor,ptr];p.reg[14]=0x10020001;p.run(entry=0x8006bc0,stop=0x10020000)
 post=a[13+count*18:13+count*18+18]
 for off,n,j in mapping:assert p.read(actor+off,n)==post[j]&((1<<(n*8))-1),dict(case=index,offset=hex(off),original=p.read(actor+off,n),native=post[j])
 assert p.read(actor+0x24)==descriptors+post[6]*20
 assert p.read(0x2003b04)==a[-3] and p.read(0x3004c98)==a[-2] and p.calls==a[-1],dict(case=index,rng=p.read(0x2003b04),counter=p.read(0x3004c98),calls=p.calls,expected=a[-3:])
 steps+=p.steps
report=dict(cases=len(rows),accepted=sum(a[-1] for a in rows),rejected=sum(not a[-1] for a in rows),pass_match=True,thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original6BC0 +original2E190 pathinitialize +1E72C RNG vs actual1003spawnapply entries, actualROMcity1 cell bytes/descriptors and27priorneighbors. Settingsuppernibble inferred uniquely fromcaptured density/spacing; originalbootstrap and continuousframeownership notexecuted, signeddivision/remainder contracts. No computednativepath injected intooriginalinitializer.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'tools/compare_live_traffic_spawn_rom.py',root/'reports/current-traffic-spawn-states.txt',grid,root/'src/traffic_spawn.c',root/'src/traffic_init.c',root/'bin/scene-viewer.exe']}
(root/'reports/live-traffic-spawn-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
