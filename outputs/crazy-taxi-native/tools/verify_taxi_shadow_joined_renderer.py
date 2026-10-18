"""Join actual original taxi renderer into its original normal shadow producer."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_taxi_shadow_producer.py').read_text().split('\ncases=[]')[0])
class JoinedShadowProbe(ShadowProbe):
 def external(self,target):
  if target==0x08029b44:self.reg[0]=self.ground&MASK
  elif target==0x0803201c:
   self.depth_offsets.append(signed(self.reg[0]));self.write(0x020368f0,self.reg[0]) # Explicit depth-state setter boundary.
  elif target==0x08031a50:
   descriptor=self.reg[0];point=self.reg[1]
   self.shadow_call=dict(descriptor=[self.read(descriptor+i,1) for i in range(4)],pixels=hex(self.read(descriptor+4)),position=[signed(self.read(point+i*4)) for i in range(3)],scale=self.reg[2],mode=self.reg[3],flags=self.read(self.reg[13]))
   self.run(entry=target,stop=self.reg[14]&~1)
   self.shadow_packet=bytes(self.read(0x02009eec+i,1) for i in range(144))
  else:super().external(target)
expected=json.loads((root/'reports/taxi-shadow-producer-verification.json').read_text());results=[]
for ticks,row in zip([1,30,60,90,120],expected['cases']):
 d=json.loads((root/'reports'/('live-taxi-gpu-'+str(ticks)+'.json')).read_text());x,y,z,heading,bodyroll=d['vehicle'];cx,cy,cz,pitch,yaw,roll=d['camera'];base=0x02002634
 p=JoinedShadowProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];p.depth_offsets=[];p.ground=row['case'][3]
 for a in range(base,base+0xea0,4):p.arm.mem[a]=0
 for a in range(0x10001000,0x10004000,4):p.arm.mem[a]=0
 for a in range(0x03006810,0x03006b00,4):p.arm.mem[a]=0
 for a in range(0x02009cec,0x02009eec+144,4):p.arm.mem[a]=0
 p.arm.mem[0x02001708]=0;p.arm.mem[0x030053e0]=0
 for off,value,n in [(0,x,4),(4,y,4),(8,z,4),(0x498,cx,4),(0x49c,cy,4),(0x4a0,cz,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x1c,heading,4),(0x1a,bodyroll,2),(0x770,d['distance']*4096,4),(0x680,d['base'],4)]:p.write(base+off,value,n)
 for off,value in [(4,0x10002000),(0x18,120),(0x1c,80),(0x28,192),(0x2c,256),(0x30,38400),(0x38,160),(0x3c,106)]:p.write(0x03006810+off,value)
 for i,value in enumerate([cx,cy,cz,pitch,yaw]):p.write(0x10002000+i*4,value)
 for slot,target in [(0x03006ab0,0x08000aa8),(0x03006abc,0x08000a1c),(0x03006940,0x080029fc)]:p.arm.mem[slot]=target
 p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x0801556c,stop=0x10020000)
 call=p.shadow_call;assert call['descriptor']==[64,32,6,5] and call['scale']==0x100000 and call['mode']==call['flags']==0
 assert call['position']==[x,p.ground-192,z] and p.depth_offsets==[4608,0]
 assert call['pixels']==hex(0x0820ff08+d['frame']*2048),(ticks,call,d['frame'])
 packet=p.shadow_packet
 get=lambda off:int.from_bytes(packet[off:off+4],'little',signed=True)
 assert [get(k) for k in [64,68,72,128,132,136,48,60,112,124]]==row['native'][1:11]
 body=[p.read(base+0xe64)]+sum([v[:2] for v in p.requests],[])+[p.requests[0][2],p.requests[0][3]]
 assert body==[d['frame']]+sum(d['centers'],[])+[d['depth'],d['scale']]
 results.append(dict(ticks=ticks,shadow_call=call,depth_offsets=p.depth_offsets,shadow_rectangle=[get(k) for k in [48,60,112,124]],thumb_instructions=p.steps,arm_instructions=p.arm.steps))
report=dict(status='PASS',cases=results,scope='Actual original1556C body renderer calls actual31A50 shadow producer; frame-selected originalshadowpixelpointer/64x32descriptor/scale/groundminus192anchor/depthoffset4608 and projectedpacket matchnative at5liveposes, bodycenters/frame/depth/scale retainedexact. Nativegroundquery suppliesexplicit29B44return, originalterrainroutine stillunexecuted; depthsetter/division/allocatorcontracts. Shadowpixelraster/livepalette/fullscene/cadence unverified, offlineonly.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/taxi-shadow-joined-renderer-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report),flush=True)
