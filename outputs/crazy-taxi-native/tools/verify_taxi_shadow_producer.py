"""Original normal shadow billboard producer vs production native projection."""
from pathlib import Path
import json,subprocess,hashlib
exec((Path('outputs/crazy-taxi-native/tools/compare_taxi_renderer_original_angle_rom.py')).read_text().split('\ncases=')[0])
assert "'blt':n!=v}" in source
source=source.replace("'blt':n!=v}","'blt':n!=v,'bls':not c or z}")
exec(source);VehicleProbe.run=MotionProbe.run
allowed.append((0x9bc,0xa40))
for name in ['taxi-renderer','taxi-half-transform']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
for start,end in [(0x31a50,0x31ee4),(0x2dbc8,0x2dc12),(0x2dc20,0x2dc6a)]:
 allowed_thumb.update({i.address:bytes(i.bytes) for i in md.disasm(r[start:end],0x08000000+start)})
class ShadowProbe(TaxiProbe):
 def read(self,a,n=4):
  if 0x03006810<=a<a+n<=0x03006b00 or 0x02009cec<=a<a+n<=0x02009eec+144 or 0x020368f0<=a<a+n<=0x020368f4:
   return sum(((self.arm.mem.get((a+i)&~3,0)>>(((a+i)&3)*8))&255)<<(i*8) for i in range(n))
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x03006810<=a<a+n<=0x03006b00 or 0x02009cec<=a<a+n<=0x02009eec+144 or 0x020368f0<=a<a+n<=0x020368f4:
   for i in range(n):
    key=(a+i)&~3;shift=((a+i)&3)*8;self.arm.mem[key]=(self.arm.mem.get(key,0)&~(255<<shift))|(((v>>(i*8))&255)<<shift)
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x0802dc20,0x0802dbc8]:self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
cases=[]
for ticks in [1,30,60,90,120]:
 pose=json.loads((root/'reports'/('live-taxi-gpu-'+str(ticks)+'.json')).read_text());x,y,z,_,_=pose['vehicle'];cx,cy,cz,pitch,yaw,_=pose['camera']
 cases.append([x,y,z,-2147483648,cx,cy,cz,yaw,pitch,160,106]) # Native terrain query, original terrain query still separate.
cp=subprocess.run([str(root/'bin/taxi-shadow-probe.exe'),str(root/'assets/driving.ctr'),str(root/'assets/city1.ctw')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(cases);results=[]
for case,native in zip(cases,rows):
 case[3]=native[0];native=native[1:]
 x,y,z,ground,cx,cy,cz,yaw,pitch,h,v=case;p=ShadowProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[]
 for a in range(0x10001000,0x10004000,4):p.arm.mem[a]=0
 for a in range(0x03006810,0x03006b00,4):p.arm.mem[a]=0
 for a in range(0x02009cec,0x02009eec+144,4):p.arm.mem[a]=0
 for off,value in [(4,0x10002000),(0x18,120),(0x1c,80),(0x28,192),(0x2c,256),(0x30,38400),(0x38,h),(0x3c,v)]:p.write(0x03006810+off,value)
 for i,value in enumerate([cx,cy,cz,pitch,yaw]):p.write(0x10002000+i*4,value)
 for i,value in enumerate([x,ground-192,z]):p.write(0x10003000+i*4,value)
 p.write(0x10001000,0x05062040);p.write(0x020368f0,4608)
 p.reg[:4]=[0x10001000,0x10003000,0x100000,0];p.reg[14]=0x10020001;p.reg[13]-=16;p.write(p.reg[13],0)
 p.run(entry=0x08031a50,stop=0x10020000)
 size=p.read(0x03006910);packet=bytes(p.read(0x02009eec+i,1) for i in range(144))
 if size:
  assert size==144
  words=lambda offsets:[signed(p.read(0x02009eec+off)) for off in offsets]
  rect=words([48,60,112,124]);billboard=words([64,68,72,128,132,136])
  buckets=[i for i in range(128) if p.read(0x02009cec+4*i)==0x02009eec];assert len(buckets)==1
  original=[1]+billboard+rect+buckets
 else:original=[0]
 if original!=native:print("PACKETWORDS",[int.from_bytes(packet[k:k+4],"little",signed=True) for k in range(0,144,4)],flush=True)
 assert original==native,(case,original,native)
 results.append(dict(case=case,original=original,native=native,thumb_instructions=p.steps,packet_hex=packet.hex()))
report=dict(status='PASS',cases=results,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual original31A50 normal64x32 shadow descriptor path, originalyaw/pitch rotationhelpers, full projection/depthbucket/packetlinks vs native shadow. Five actuallivecamera/vehicle fixtures, groundheight from nativeworldquery atactualvehiclepoint; originalgroundquery independentlyunverified, descriptor shape64x32, offset4608, source240 frustum160/106. Division scalarcontracts. Doesnotprove originalgroundquery/1556C descriptorselection/shadowraster/palette/completeframe/cadence; offlineonly.')
(root/'reports/taxi-shadow-producer-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report),flush=True)
