exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
import itertools,subprocess
source=source.replace("'bhi':c and not z,", "'bhi':c and not z,'bls':not c or z,'bhs':c,'blo':not c,")
exec(source);VehicleProbe.run=MotionProbe.run
cfg=json.loads((root/'reports/marker-full-projection-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class PacketProbe(IdleProbe):
 def read(self,a,n=4):
  if 0x2000000<=a<0x2040000 or 0x3000000<=a<0x3008000:return (self.arm.mem.get(a&~3,0)>>((a&3)*8))&((1<<(n*8))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x2000000<=a<0x2040000 or 0x3000000<=a<0x3008000:
   sh=(a&3)*8;mask=((1<<(n*8))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x802dc20,0x802dbc8]:assert self.reg[0]==0
  elif target in [0x8032454,0x8032488]:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:raise AssertionError(('unqualified callee',hex(target)))
runtime=(root/'assets/driving.ctr').read_bytes();sine=[v>>4 for v in struct.unpack_from('<4096h',runtime,4)]
cases=list(itertools.product([0,-6000,6000],[-1500,0,1500],[1024,1025,4096,8192,38399,38400],[0x80000,0xc0000]))
cp=subprocess.run([str(root/'bin/passenger-marker-projection-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=visible=0
for c,expected in zip(cases,rows):
 if expected[0]==1:expected=expected[:6]
 x,y,z,scale=c;p=PacketProbe((0,0,0,0,0,1,0,0,0,0));packet=0x2009eec;point=0x10003000;camera=0x10004000;context=0x3006810
 for j in range(36):p.write(packet+j*4,0)
 for i in range(128):p.write(0x2009cec+4*i,0)
 for off,v in [(0,x),(4,y),(8,z)]:p.write(point+off,v)
 for off in [0,4,8,12,16]:p.write(camera+off,0)
 for off,v in [(4,camera),(24,120),(28,80),(40,192),(44,256),(48,38400),(56,160),(60,106)]:p.write(context+off,v)
 p.write(0x3006910,0);p.write(0x20368f0,1536);p.write(0x3006940,0)
 for i,v in enumerate(sine):p.write(0x202c4f0+4*i,v)
 p.reg[:4]=[0x87ec5d8,point,scale,0];p.write(p.reg[13],0);p.run(entry=0x8031a50,stop=0x10020000)
 used=p.read(0x3006910);actual=[1 if used else 0]
 if used:
  assert used==144,used
  bucket=next(i for i in range(128) if p.read(0x2009cec+4*i)==packet)
  actual += [signed(p.read(packet+48)),signed(p.read(packet+60)),signed(p.read(packet+112)),signed(p.read(packet+124)),bucket]
  # Axis fast path stores only corners1/3; UV stays zero and width masks drive raster sampling.
  assert p.read(packet+4,2) in [8,12]
  visible+=1
 assert actual==expected,dict(case=c,original=actual,native=expected);steps+=p.steps
report={'cases':len(cases),'visible':visible,'thumb_instructions':steps,'match':True,'scope':'Complete original31A50 throughreturn vs native culling/two axis screen corners/depth bucket; packet flags checked as8/12 (not native-compared), axis fast path leaves UV zero, normal64x64 markers, viewport240x160. Explicit identity yaw/pitch0 helper contracts and signed division contract; clean buffer. No nonzero camera transform, rasterpixels or full22DF4 proof.'}
(root/'reports/passenger-marker-full-projection-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
