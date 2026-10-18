"""Original registered BG palette fade setup and service diagnostic."""
exec(open('outputs/crazy-taxi-native/tools/probe_scene_bg_palette_registration_rom.py').read().split('\nresults=[]')[0])
entries=[(0xe5a8,0xee10),(0xee10,0xf5e8)]
rows=[]
for lo,hi in entries:
 todo=[lo];seen={}
 while todo:
  off=todo.pop()
  if off in seen:continue
  assert lo<=off<hi,(hex(lo),hex(off))
  i=next(md.disasm(r[off:off+4],off+0x8000000,count=1));seen[off]=i
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):
   todo.append(int(i.op_str[1:],16)-0x8000000)
   if i.mnemonic=='b':continue
  todo.append(off+i.size)
 for off,i in sorted(seen.items()):
  row={'address':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str};h=struct.unpack_from('<H',r,off)[0]
  if h&0xf800==0x4800:
   at=((off+4)&~3)+(h&255)*4;row['literal']=hex(struct.unpack_from('<I',r,at)[0])
  rows.append(row);allowed_thumb[i.address]=bytes(i.bytes)
(root/'reports/scene-bg-palette-fade-cfg.json').write_text(json.dumps({'instructions':rows,'scope':'Bounded original E5A8 andEE10 CFG; external callees not traversed'},indent=2))
class FadeProbe(PaletteProbe):
 def read(self,a,n=4):
  if 0x4000000<=a<0x4000400 and not 0x40000d4<=a<0x40000e0:
   assert a&~3 in self.arm.mem,('unmapped hardware',hex(a))
   return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(8*n))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if a==0x40000dc and v&0x80000000:
   assert n==4 and v==0x84000080,hex(v)
   start=self.read(0x40000d4);dest=self.read(0x40000d8)
   assert dest in (0x5000000,0x5000200,0x3004eb0),hex(dest)
   for k in range(128):self.write(dest+4*k,self.read(start+4*k))
   self.dma.append({'source':hex(start),'destination':hex(dest),'bytes':512});self.arm.mem[a]=v&0x7fffffff;return
  if 0x4000000<=a<0x4000400 and not 0x40000d4<=a<0x40000e0:
   sh=(a&3)*8;mask=((1<<(8*n))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
  else:super().write(a,v,n)
p=FadeProbe((0,0,0,0,0,1,0,0,0,0));p.dma=[]
for i in cfg+rows:
 if 'literal' in i:
  a=int(i['literal'],16)
  if 0x2000000<=a<0x2040000:
   for off in range(0,32,4):p.write((a&~3)+off,0)
for a in range(0x5000000,0x5000400,4):p.write(a,0)
palette=struct.unpack_from('<I',r,0x7556cc+24)[0]
p.reg[:3]=[palette,32,1];p.reg[14]=0x10020001;p.run(entry=0x800e154,stop=0x10020000)
p.steps=0;p.reg[:4]=[1,32,1,1];p.reg[14]=0x10020001;p.run(entry=0x800e5a8,stop=0x10020000)
p.write(0x2003b00,1)
original=struct.unpack_from('<256H',r,palette-0x8000000)
native=subprocess.run([str(root/'bin/scene-palette-fade-rom-compare.exe')],input=' '.join(map(str,original)),text=True,capture_output=True,check=True)
native_rows=[list(map(int,row.split())) for row in native.stdout.splitlines()];assert len(native_rows)==35
frames=[]
for tick in range(35):
 progress=signed(p.read(0x200178a,2)<<16)>>16
 p.steps=0;p.reg[14]=0x10020001;p.run(entry=0x800ee10,stop=0x10020000)
 actual=[p.read(0x5000000+2*k,2) for k in range(256)]
 expected=[sum(min((v>>shift)&31,max(0,progress))<<shift for shift in [0,5,10]) for v in original]
 assert actual==expected,(tick,progress,actual[:8],expected[:8])
 assert native_rows[tick]==[progress,p.read(0x200178a,2),p.read(0x200177b,1)]+actual,(tick,'native palette/timing mismatch')
 frames.append({'tick':tick,'cap':progress,'instructions':p.steps,'next_cap':p.read(0x200178a,2),'active':p.read(0x200177b,1)})
print('PASS35 original/native fadeframes,8960colors andprogress/active exact; original instructions',sum(f['instructions'] for f in frames))
(root/'reports/scene-bg-palette-fade-rom-probe.json').write_text(json.dumps({'rom_sha256':hashlib.sha256(r).hexdigest(),'palette':hex(palette),'frames':frames,'compared_colors':35*256,'equal':True,'native_fade_equal':True,'source_sha256':{name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['src/palette_fade.c','tests/scene_palette_fade_rom_compare.c','tools/probe_scene_bg_palette_fade_rom.py','tools/probe_scene_bg_palette_registration_rom.py']},'scope':'Actual E154 then E5A8(1,32,1,1) then35 EE10 frames delta1 cleanbank. Original active BG colors and progress/active fields compared actualnative palette_fade.c plus channelcap reference; explicit DMA copy contract, not hardware timing/fullbootstrap or laterpaletteoverwrite proof.'},indent=2))
