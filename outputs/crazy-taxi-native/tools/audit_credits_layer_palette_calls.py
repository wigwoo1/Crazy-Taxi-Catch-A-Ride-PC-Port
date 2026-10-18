"""Original5778 layer caller; asset helpers and DMA deliberately opaque."""
exec(open('outputs/crazy-taxi-native/tools/probe_scene_bg_palette_registration_rom.py').read().split('\nresults=[]')[0])
needle="a=val(ops[1]);n=val(ops[2]);value="
assert needle in source
source=source.replace(needle,"a=val(ops[1]) if len(ops)==3 else self.reg[rn(ops[0])];n=val(ops[2]) if len(ops)==3 else val(ops[1]);assert n<32;value=")
exec(source);PaletteProbe.run=MotionProbe.run
rows=json.loads((root/'reports/background-asset-initializer-full-cfg.json').read_text())['instructions'];allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in rows})
class InitProbe(PaletteProbe):
 def read(self,a,n=4):
  if 0x4000000<=a<0x4000400 or 0x6000000<=a<0x6020000:return (self.arm.mem.get(a&~3,0)>>((a&3)*8))&((1<<(8*n))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x4000000<=a<0x4000400 or 0x6000000<=a<0x6020000:
   sh=(a&3)*8;mask=((1<<(8*n))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
   if a==0x40000dc and v&0x80000000:self.arm.mem[a]=v&0x7fffffff
  else:super().write(a,v,n)
 def external(self,target):
  self.calls.append([hex(target),*self.reg[:3]])
  if target==0x800f7b0:self.reg[0]=0x2009000
  elif target==0x8007190:pass
  elif target==0x800e388:self.reg[0]=0
  else:raise AssertionError(hex(target))
p=InitProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
for i in rows:
 if 'literal' in i:
  a=int(i['literal'],16)
  if 0x2000000<=a<0x2040000:
   for off in range(0,64,4):p.write((a&~3)+off,0)
p.write(0x2007559,1,1);p.reg[0]=0x80344f8;p.reg[14]=0x10020001;p.run(entry=0x8005778,stop=0x10020000)
pal=[x[1:] for x in p.calls if x[0]=='0x800e388']
assert pal==[[0x80be7ec,0,1],[0x80b927c,0,1]],pal
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'entry':'08005778','descriptor':'080344F8','thumb_instructions':p.steps,'calls':p.calls,'palette_sources':pal,'scope':'Original full5778 instructions using explicit layer range0..1. RLE7190 andF7B0 registry helpers opaque, paletteE388 returns0 fixture; DMA completion observed without copying. Only proves two ordered palette caller pointers and arguments, NOT allocation offsets, live palette contents or source pixels. Register shift2operand diagnostic support scoped n<32; original thumb opcodes whitelist unchanged.'}
(root/'reports/credits-layer-palette-call-audit.json').write_text(json.dumps(report,indent=2));print('PASS',report)
