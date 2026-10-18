"""Execute original taxi renderer with explicit diagnostic submission contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_projection_rom.py').read().split('\nd=json.loads')[0])
for name in ['taxi-renderer','taxi-half-transform']:
 cfg=json.loads((root/('reports/'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class TaxiProbe(ProjectionProbe):
 def write(self,a,v,n=4):
  if 0x2005c58<=a<0x2005c60 or 0x2009ab0<=a<0x2009abc:
   sh=(a&3)*8;mask=((1<<(8*n))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x803245c:
   entry=self.reg[4];assert entry==0x8000aa8;self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  elif target==0x802dc78:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x802dd5c:self.reg[0]=((signed(self.reg[0])-signed(self.reg[1])+2048)%4096-2048)&MASK
  elif target==0x8029b44:self.reg[0]=0 # ground/shadow height fixture only
  elif target in [0x803201c,0x8031a50]:pass # shadow submission/graphics state boundary
  elif target==0x8027fb8:self.reg[0]=0 # sprite allocator contract
  elif target==0x8032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:super().external(target)
p=TaxiProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];p.arm.mem[0x2001708]=0;p.arm.mem[0x30053e0]=0;base=0x2002634
for a in range(base,base+0xea0,4):p.arm.mem[a]=0
for off,val,n in [(0x498,0,4),(0x49c,468,4),(0x4a0,-1024,4),(0x4a4,0,2),(0x4a6,0,2),(0x4a8,0,2),(0x770,1024*4096,4)]:p.write(base+off,val,n)
p.arm.mem[0x3006ab0]=0x8000aa8;p.arm.mem[0x3006abc]=0x8000a1c;p.arm.mem[0x3006940]=0x80029fc;p.arm.mem[0x3006810]=0x10002000
for o,v in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x10002000+o]=v;p.arm.mem[0x3006810+o]=v
p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
print('requests',p.requests,'steps',p.steps,p.arm.steps,'frame',p.read(base+0xe64))
(root/'reports/taxi-renderer-first-probe.json').write_text(json.dumps(dict(requests=p.requests,frame=p.read(base+0xe64),thumb=p.steps,arm=p.arm.steps,scope='Original full renderer under fixed camera/actor/ground and allocator/shadow contracts; no GBA camera fidelity or native comparison'),indent=2))
