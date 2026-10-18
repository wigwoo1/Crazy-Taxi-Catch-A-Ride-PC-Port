"""Original20A20 with real native captured opaque boost records; diagnostic."""
exec(open('outputs/crazy-taxi-native/tools/compare_body_points_rom.py').read().split('\nschema=')[0])
allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/effect-projection-cfg.json').read_text())['instructions']}
class ProjectionProbe(VehicleProbe):
 def write(self,a,v,n=4):
  if 0x2002634<=a<0x20034d4 or 0x2005c60<=a<0x2005ee8 or 0x10008000<=a<0x10010000:
   old=self.arm.mem.get(a&~3,0);sh=(a&3)*8;mask=((1<<(n*8))-1)<<sh;self.arm.mem[a&~3]=((old&~mask)|((v<<sh)&mask))&MASK
  else:super().write(a,v,n)
 def read(self,a,n=4):
  if 0x2002634<=a<0x20034d4 or 0x10008000<=a<0x10010000:return (self.arm.mem.get(a&~3,0)>>((a&3)*8))&((1<<(8*n))-1)
  return super().read(a,n)
 def external(self,target):
  if target in [0x8032460,0x8032458]:
   entry=self.reg[5 if target==0x8032460 else 3];self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  elif target==0x8032454:
   a,b=map(signed,self.reg[:2]);self.reg[0]=(trunc(a,b) if b else a)&MASK
  elif target==0x8028658:
   point=[signed(self.read(self.reg[2]+j*4)) for j in range(3)];args=[self.read(self.reg[13]+j*4) for j in range(5)]
   scale=args[3]&65535;scale=scale-65536 if scale&32768 else scale
   self.requests.append(point+[scale,self.reg[1],self.reg[0],args[2],args[0],self.reg[3],args[1]])
  else:raise AssertionError(('unqualified dependency',hex(target)))
allowed.append((0x9bc,0xa40))
d=json.loads((root/'reports/live-effect-source-inputs.json').read_text());p=ProjectionProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[]
x,y,z,pitch,yaw,roll=d['camera']
for off,val,n in [(0x498,x,4),(0x49c,y,4),(0x4a0,z,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x544,d['phase'],4)]:p.write(0x2002634+off,val,n)
p.arm.mem[0x3006ab0]=0x8000aa8;p.arm.mem[0x3006abc]=0x8000a1c
p.arm.mem[0x3006810]=0x10002000
for off,val in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+off]=val
p.arm.mem[0x3006940]=0x80029fc;
p.arm.mem[0x2001708]=0;p.arm.mem[0x2005f20]=0
for i,record in enumerate(d['records']):
 for j in range(0,72,4):p.write(0x2005c60+i*72+j,int.from_bytes(bytes(record[j:j+4]),'little'))
print('dispatch',hex(p.arm.mem[0x3006ab0]),hex(p.arm.mem.get(0x3006abc,0)))
p.run(entry=0x8020a20,stop=0x10020000)
assert p.requests==d['requests'],dict(original=p.requests,native=d['requests'])
report=dict(requests=len(p.requests),thumb_instructions=p.steps,arm_instructions=p.arm.steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete20A20 original with originalARMmatrix and captured28658submission args; division explicitcontract, controlledprojectionblockglobals/real native500tick records; no fullframe/pixeloracle.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'tools/compare_effect_projection_rom.py',root/'reports/live-effect-source-inputs.json',root/'reports/effect-projection-cfg.json',root/'src/effect_sprite.c',root/'src/impact.c']}
(root/'reports/effect-projection-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
