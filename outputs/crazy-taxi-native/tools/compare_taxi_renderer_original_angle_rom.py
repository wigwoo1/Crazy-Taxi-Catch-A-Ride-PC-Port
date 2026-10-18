"""Execute original taxi renderer with explicit diagnostic submission contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_projection_rom.py').read().split('\nd=json.loads')[0])
for name in ['taxi-renderer','taxi-half-transform']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
exec(open('outputs/crazy-taxi-native/tools/original_angle_difference_probe.py').read())
class TaxiProbe(ProjectionProbe):
 def write(self,a,v,n=4):
  if 0x2005c58<=a<0x2005c60 or 0x2009ab0<=a<0x2009abc or a==0x30053e0:
   sh=(a&3)*8;mask=((1<<(8*n))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x803245c:
   entry=self.reg[4];assert entry==0x8000aa8;self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  elif target==0x802dc78:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x802dd5c:self.reg[0]=original_angle_difference(*self.reg[:2])
  elif target==0x8029b44:self.reg[0]=0 # ground/shadow height fixture only
  elif target in [0x803201c,0x8031a50]:pass # shadow submission/graphics state boundary
  elif target==0x8027fb8:self.reg[0]=0 # sprite allocator contract
  elif target==0x8032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:super().external(target)
cases=[(0,height,-dist,0,0,0,heading*4096,bodyroll,dist,cam_base) for cam_base in [-128,0,128] for dist in [1024,2048,3072,5120] for height in [0,468,1024] for bodyroll in [0,128,-128] for heading in [0,1024,2048,3072,-128]]
res=subprocess.run([str(root/'bin/taxi-renderer-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases)
for index,(case,row) in enumerate(zip(cases,rows)):
 cx,cy,cz,pitch,yaw,roll,heading,bodyroll,dist,cam_base=case;p=TaxiProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];p.arm.mem[0x2001708]=0;p.arm.mem[0x30053e0]=0;base=0x2002634
 for a in range(base,base+0xea0,4):p.arm.mem[a]=0
 for off,val,n in [(0x498,cx,4),(0x49c,cy,4),(0x4a0,cz,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x1c,heading,4),(0x1a,bodyroll,2),(0x770,dist*4096,4),(0x680,cam_base,4)]:p.write(base+off,val,n)
 p.arm.mem[0x3006ab0]=0x8000aa8;p.arm.mem[0x3006abc]=0x8000a1c;p.arm.mem[0x3006940]=0x80029fc;p.arm.mem[0x3006810]=0x10002000
 for o,v in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+o]=v
 p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
 got=[p.read(base+0xe64)]+[v for req in p.requests for v in req[:2]]+[p.requests[0][2],p.requests[0][3]];assert got==row,dict(case=case,original=got,native=row)
report=dict(cases=len(cases),scope='Complete original1556C+ARMcamera+2DC78 vs nativecamera_project/taxi_sprite_frame/obj_half_centers for controlledcamera heights/distances/bodyroll; originalARM52B4 angle body; allocator/shadow/ground/division contracts, not currentlivecamera/GBApixelmatch')
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [root/'tools/compare_taxi_renderer_original_angle_rom.py',root/'tests/taxi_renderer_rom_compare.c',root/'bin/taxi-renderer-rom-compare.exe',root/'src/camera_angle.c',root/'src/obj_order.c',root/'src/taxi_sprite.c']}
(root/'reports/taxi-renderer-original-angle-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
