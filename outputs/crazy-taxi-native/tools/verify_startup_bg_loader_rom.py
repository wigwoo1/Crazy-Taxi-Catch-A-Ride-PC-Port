"""Actual intro5778 loader control/maps with explicit decodedtile/palette service contracts."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
saved_allowed=allowed_thumb.copy()
source=source.replace("'b':True,'bne'", "'bhs':c,'blo':not c,'b':True,'bne'")
source=source.replace("   elif m.startswith('b'):", "   elif m=='bics':\n    value=self.reg[rn(ops[0])]&(~val(ops[1])&MASK);self.reg[rn(ops[0])]=value;_,_,c,v=self.flags;self.flags=(bool(value&0x80000000),value==0,c,v)\n   elif m.startswith('b'):")
exec(source);allowed_thumb.update(saved_allowed)
import re
for line in (root/'reports/startup-bg-loader-source.txt').read_text().splitlines():
 a=int(line[:8],16);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
class BGProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t==0x8007190:
   count=self.reg[1];source=self.reg[0];layer=self.tile_calls;assert layer<2
   assert source==struct.unpack_from('<I',r,0x34474+32+layer*4)[0]
   data=(root/f'assets/startup-animation/background-{layer}.tiles').read_bytes();assert count*64==len(data) and self.reg[2:4]==[256,1]
   dst=self.read(0x200152c)
   for i,v in enumerate(data):self.write(dst+i,v,1)
   self.tile_calls+=1;self.reg[0]=0
  elif t==0x800f7b0:assert self.reg[0]==1;self.reg[0]=0
  elif t==0x800e154:
   assert self.reg[1:3]==[0,1];data=r[self.reg[0]-0x8000000:self.reg[0]-0x8000000+512]
   self.regions[0x5000000][:512]=data;self.palette_calls+=1;self.reg[0]=0
  else:raise AssertionError(('unexpected loader helper',hex(t)))
p=BGProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(0x1600),0x4000000:bytearray(1024),0x5000000:bytearray(1024),0x6000000:bytearray(0x18000)};p.scalars={0x2007558:0,0x2007559:1,0x2001780:0,0x2001782:0};p.tile_calls=p.palette_calls=0
p.reg[0]=0x8034474;p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x8005778,stop=0x10020000)
assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
assert p.tile_calls==2 and p.palette_calls==1,(p.tile_calls,p.palette_calls)
assert p.read(0x4000050,2)==0x1f40 and p.read(0x4000052,2)==0x0808
layers=[]
for layer in range(2):
 control=p.read(0x4000008+2*layer,2);assert control&3==layer and control&0x80
 mapbase=0x6000000+((control>>8)&31)*2048;charbase=0x6000000+((control>>2)&3)*16384
 w=512 if layer==0 else 256;h=256;decoded=bytearray()
 for y in range(h):
  for x in range(w):
   tx,ty=x//8,y//8;block=(tx//32)+(ty//32)*(w//256);entry=p.read(mapbase+block*2048+((ty%32)*32+tx%32)*2,2)
   sx,sy=x%8,y%8
   if entry&0x400:sx=7-sx
   if entry&0x800:sy=7-sy
   decoded.append(p.read(charbase+(entry&1023)*64+sy*8+sx,1))
 expected=(root/f'assets/startup-animation/background-{layer}.indices').read_bytes()
 if layer==1:
  tile0=(root/'assets/startup-animation/background-1.tiles').read_bytes()[:64];source=expected
  expected=bytes(source[y*240+x] if x<240 and y<160 else tile0[(y%8)*8+x%8] for y in range(256) for x in range(256))
 assert decoded==expected,(layer,'resolved indices mismatch',next((i,decoded[i],expected[i]) for i in range(len(decoded)) if decoded[i]!=expected[i]),hex(control))
 layers.append({'layer':layer,'control':hex(control),'mapbase':hex(mapbase),'charbase':hex(charbase),'indices_sha256':hashlib.sha256(decoded).hexdigest()})
assert bytes(p.regions[0x5000000][:512])==(root/'assets/startup-animation/background-1.palette.rgb555').read_bytes()
report={'status':'PASS','tile_service_calls':p.tile_calls,'shared_palette_calls':p.palette_calls,'Thumb_instructions':p.steps,'layers':layers,'scope':'Actual whole5778 intro descriptor loader control/map assembly andresolvedindices/palette. Tile7190 uses previously source-verified decodedtiles, paletteE154 logicalcopy andallocatorF7B0 returns0 contracts; not whole service/device orhardware pixel equivalence.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/startup-bg-loader-rom-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
