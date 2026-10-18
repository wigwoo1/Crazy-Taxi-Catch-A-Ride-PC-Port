"""Whole5778 title map loader withactualE388 smallpalette registration."""
exec(open('outputs/crazy-taxi-native/tools/verify_startup_bg_loader_rom.py').read().split('\nclass BGProbe')[0])
saved_allowed=allowed_thumb.copy();source=source.replace('self.steps<50000','self.steps<200000');exec(source);allowed_thumb.update(saved_allowed)
branches={'beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl','bvs','bvc'}
todo=[0x800e388];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x800e388<=a<0x800e5a8,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in branches:todo.append(i.operands[0].imm)
class TitleBGProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t==0x8007190:
   assert self.reg[0]==struct.unpack_from('<I',r,0x3415c+32)[0]
   data=(root/'assets/title/background.tiles').read_bytes();assert self.reg[1]*32==len(data) and self.reg[2:4]==[16,1],self.reg[:4]
   dst=self.read(0x200152c)
   for i,v in enumerate(data):self.write(dst+i,v,1)
   self.tiles+=1;self.reg[0]=0
  elif t==0x800f7b0:assert self.reg[0]==1;self.reg[0]=0
  elif t==0x800e388:
   self.registration_args=self.reg[:4].copy();self.run(entry=t,stop=self.reg[14]&~1);self.palettes+=1
  else:raise AssertionError(('unexpectedtitleloaderhelper',hex(t)))
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   assert v==0x84000008,hex(v)
   src,dst=self.read(0x40000d4),self.read(0x40000d8)
   assert dst==0x5000000
   for i in range(8):super().write(dst+4*i,self.read(src+4*i),4)
   self.transfers.append((src,dst,v))
p=TitleBGProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(0x1600),0x2001770:bytearray(0xa20),0x4000000:bytearray(1024),0x5000000:bytearray([165]*1024),0x6000000:bytearray(0x18000)};p.scalars={0x2007558:0,0x2007559:0};p.tiles=p.palettes=0;p.transfers=[]
p.reg[0]=0x803415c;p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x8005778,stop=0x10020000)
assert p.reg[13]==0x10000400 and p.reg[4:12]==saved and p.tiles==p.palettes==1
control=p.read(0x4000008,2);assert control&0x80==0 and control&3==0
mapbase=0x6000000+((control>>8)&31)*2048;charbase=0x6000000+((control>>2)&3)*16384;indices=(root/'assets/title/background.indices').read_bytes()
for y in range(160):
 for x in range(240):
  entry=p.read(mapbase+((y//8)*32+x//8)*2,2);xx,yy=x%8,y%8
  if entry&1024:xx=7-xx
  if entry&2048:yy=7-yy
  index=(p.read(charbase+(entry&1023)*32+yy*4+xx//2,1)>>(4*(xx&1)))&15;index+=16*(entry>>12)
  assert index==indices[y*512+x],(x,y,index,indices[y*512+x])
assert p.regions[0x5000000][:32]==bytes(32) and p.regions[0x5000000][32:]==bytes([165]*992)
assert p.read(0x2001780,2)==4 and p.read(0x2001782,2)==128 and p.read(0x2001784,2)==0
record=bytes(p.read(0x2001790+i,1) for i in range(20));assert struct.unpack_from('<H',record)[0]==16 and struct.unpack_from('<I',record,16)[0]==0x80b70f8
report={'status':'PASS','Thumb_instructions':p.steps,'control':hex(control),'mapbase':hex(mapbase),'charbase':hex(charbase),'registration_args':[hex(x) for x in p.registration_args],'record':record.hex(),'records':[bytes(p.read(0x2001790+j*20+i,1) for i in range(20)).hex() for j in range(p.read(0x2001780,2))],'transfers':p.transfers,'blend':hex(p.read(0x4000050,2)),'alpha':hex(p.read(0x4000052,2)),'scope':'Actualwhole5778 type1title descriptor0803415C withactualwholeE388 registration, exact32initialBLACKBGpalette/992retainedpalettebytes, sourceforcedcount4/bgbytes128/obj0/activefirstrecord source identity, SP/callee-saved, all240x160initialviewport4bppindices. Tile7190 uses8032decodedbytes independentlyactual1E860verified andallocatorF7B0return0 contract; DMA84000008logicalcopy. Noactualtiledecompress/font/objectloader/visiblecolors/titleparent/frame proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'assets/title/background.tiles',root/'assets/title/background.indices']}}
(root/'reports/title-bg-loader-rom-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
