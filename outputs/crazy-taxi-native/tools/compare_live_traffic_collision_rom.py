"""Original complete7050 compared with exact actual native contact boundaries."""
exec(open('outputs/crazy-taxi-native/tools/probe_full_camera_view_rom.py').read().split('\np=CameraProbe')[0])
import math
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/traffic-collision-cfg.json').read_text())['instructions']})
class CollisionProbe(TaxiProbe):
 def read(self,a,n=4):
  if 0x10003000<=a<0x10003060:return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(8*n))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x10003000<=a<0x10003060:
   assert n==4;self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x8032038:self.reg[0]=math.isqrt(self.reg[0]&MASK)
  else:super().external(target)
f=root/'reports/current-traffic-contact-states.txt';rows=[list(map(int,l.split())) for l in f.read_text().splitlines()];assert len(rows)==22
steps=0
for index,a in enumerate(rows):
 ax,az,ix,iz,flags,timer,x,z,speed,sine,cosine,delta=a[:12];p=CollisionProbe((0,0,0,0,0,1,0,0,0,0));base=0x2002634;actor=0x10003000
 for addr in range(actor,actor+0x60,4):p.arm.mem[addr]=0
 for off,val in [(0,flags),(0x14,ax),(0x1c,az),(0x40,ix),(0x48,iz),(0x58,timer)]:p.write(actor+off,val)
 for off,val in [(0,x),(8,z),(0x1c,0),(0x544,speed)]:p.write(base+off,val)
 p.arm.mem[0x202c4f0]=sine&MASK;p.arm.mem[0x20304f0]=cosine&MASK;p.arm.mem[0x30053c0]=delta
 p.reg[:2]=[actor,base];p.reg[14]=0x10020001;p.run(entry=0x8007050,stop=0x10020000)
 got=[signed(p.read(actor+o)) for o in [0x14,0x1c,0x40,0x48]]+[p.read(actor),signed(p.read(actor+0x58))]
 assert got==a[12:],dict(index=index,original=got,native=a[12:]);steps+=p.steps
report=dict(contacts=len(rows),pass_match=True,thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original7050 vs actual native collision boundaries all22contacts. Original table index0 populated with captured exact Q8sin/cos. Square root math.isqrt and signed division explicit arithmetic contracts; not original root body/fulltrafficframe proof.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [f,root/'tools/compare_live_traffic_collision_rom.py',root/'reports/traffic-collision-cfg.json',root/'src/traffic_collision.c',root/'bin/scene-viewer.exe']}
(root/'reports/live-traffic-collision-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
