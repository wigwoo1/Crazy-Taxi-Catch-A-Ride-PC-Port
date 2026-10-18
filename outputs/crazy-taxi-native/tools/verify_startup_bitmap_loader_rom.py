"""Actual25E38 with actual26198 and1E860; explicit setup/palette/DMA contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_bitmap_fill_rom.py').read().split('\ncases=')[0])
saved_allowed=allowed_thumb.copy()
source=source.replace('self.steps<50000','self.steps<3000000')
exec(source)
allowed_thumb.update(saved_allowed)
for line in (root/'reports/startup-bitmap-loader-source.txt').read_text().splitlines():
 a=int(line[:8],16);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
todo=[0x801e860];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x801e860<=a<=0x801e95c,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in branches:todo.append(i.operands[0].imm)
class BitmapProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t in [0x8026198,0x801e860]:
   if t==0x8026198:self.fills+=1
   else:self.decodes+=1
   self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x8025c18:
   assert self.reg[:2]==[4,0]
   self.write(0x2007541,4,1);self.write(0x2007542,240,2);self.write(0x2007544,160,2);self.write(0x2007546,8,2);self.write(0x2007548,0,1)
   self.write(0x200754c,0x6000000);self.write(0x2007550,0x600a000);self.write(0x4000000,0x444,2)
   self.setup+=1
  elif t==0x800f5e8:self.reset+=1
  elif t==0x800e154:
   assert self.reg[:3]==[self.image+8,32,1]
   self.regions[0x5000000][:512]=r[self.image-0x8000000+8:self.image-0x8000000+520];self.palettes+=1
  else:raise AssertionError(('unexpected helper',hex(t)))
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   assert v==0x8400003c,hex(v)
   src,dst=self.read(0x40000d4),self.read(0x40000d8)
   for i in range(60):super().write(dst+i*4,self.read(src+i*4),4)
   self.transfers+=1
records=[]
for state in range(3):
 p=BitmapProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007540:bytearray(32),0x4000000:bytearray(256),0x5000000:bytearray([165]*1024),0x6000000:bytearray([123]*40960),0x600a000:bytearray([77]*40960)};p.scalars={}
 p.fills=p.decodes=p.setup=p.reset=p.palettes=p.transfers=0
 p.image=struct.unpack_from('<I',r,0x49d560+state*28)[0]
 p.reg[:3]=[p.image,0,0];p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy()
 p.run(entry=0x8025e38,stop=0x10020000)
 assert p.reg[0]==1 and p.reg[13]==0x10000400 and p.reg[4:12]==saved
 expected=(root/f'assets/startup-screen-{state}.indices').read_bytes()
 for address,seed in [(0x6000000,123),(0x600a000,77)]:
  assert p.regions[address][:38400]==expected,(state,hex(address),'bitmap mismatch')
  assert p.regions[address][38400:]==bytes([seed]*2560),(state,'tail overwritten')
 assert p.regions[0x5000000][:512]==(root/f'assets/startup-screen-{state}.palette.rgb555').read_bytes()
 assert p.regions[0x5000000][512:]==bytes([165]*512)
 assert p.read(0x2007554)==p.image+8 and p.read(0x2007548,1)==0 and p.read(0x4000000,2)==0x444
 assert (p.fills,p.decodes,p.setup,p.reset,p.palettes,p.transfers)==(2,320,1,1,1,320)
 records.append({'screen':state,'Thumb_instructions':p.steps,'row_decodes':p.decodes,'fills':p.fills,'word_transfers':p.transfers,'indices_sha256':hashlib.sha256(expected).hexdigest()})
report={'status':'PASS','screens':records,'scope':'Whole25E38 first3 original240x160 compressed mode4 bitmaps with R1=0/R2=0; actual26198 twice andactual1E860 320rows/screen; both38400byte banks, retained2560byte tails, palette/rest bank, source palette pointer, bank/DISPCNT/SP/callee-saved. Mode25C18 explicit previously-qualified mode4 owner setup contract; F5E8 reset call count andE154 logical512palette copy contracts; DMA exact8400003C requests/logical copies. No actual palette registry ownership/full display or optional loader own-fade/mode3 proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/startup-bitmap-loader-rom-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
