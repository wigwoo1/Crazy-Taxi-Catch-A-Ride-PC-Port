"""Actual27994 title groups andpalette registration; image/setup contracts explicit."""
exec(open('outputs/crazy-taxi-native/tools/verify_title_bg_loader_rom.py').read().split('\nclass TitleBGProbe')[0])
for entry,end in [(0x8027994,0x8027cce),(0x800e154,0x800e380),(0x8007190,0x8007376),(0x8007378,0x80073be),(0x80073c0,0x8007400)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<=end,hex(a)
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic in branches:todo.append(i.operands[0].imm)
class TitleAssetProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t in [0x800e154,0x800e388]:
   self.palettes.append({'routine':hex(t),'args':self.reg[:3].copy(),'record_before':self.read(0x2001780,2)})
   self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x8007190:
   self.images.append({'args':self.reg[:4].copy(),'first_before':hex(self.read(0x200152c)),'second_before':hex(self.read(0x2001530))});self.run(entry=t,stop=self.reg[14]&~1);self.images[-1]['return']=hex(self.reg[0])
  elif t in [0x8007378,0x80073c0]:self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x803406c:
   src,dst,count=self.reg[:3];assert count%4==0
   for i in range(count//4):self.write(dst+4*i,self.read(src+4*i))
   self.reg[0]=(src+count)&MASK;self.reg[1]=(dst+count)&MASK;self.reg[3]=0
  elif t==0x8028bd0:self.obj_setup+=1;self.reg[0]=0
  else:raise AssertionError(('unexpectedtitleassethelper',hex(t)))
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   assert v&0xffff0000==0x84000000,hex(v)
   count=v&65535;src,dst=self.read(0x40000d4),self.read(0x40000d8)
   assert count<=1024
   for i in range(count):InitProbe.write(self,dst+4*i,self.read(src+4*i),4)
   self.transfers.append({'source':hex(src),'destination':hex(dst),'bytes':count*4})
p=TitleAssetProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(0xa000),0x4000000:bytearray(1024),0x5000000:bytearray([165]*1024),0x6000000:bytearray(0x18000)};p.scalars={};p.palettes=[];p.images=[];p.transfers=[];p.obj_setup=0
p.write(0x2001780,4,2);p.write(0x2001782,128,2);p.write(0x2001790,16,2);p.write(0x2001796,1,1);p.write(0x2001797,1,1);p.write(0x20017a0,0x80b70f8)
p.reg[13]=0x10000400;p.reg[14]=0x10020001
p.run(entry=0x80073c0,stop=0x10020000)
assert p.read(0x2001530)==0x06010000
p.reg[:2]=[0x8034634,1];p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x8027994,stop=0x10020000)
assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
resource=0x34634;groups=[]
for g in range(struct.unpack_from('<I',r,resource)[0]):
 off=resource+struct.unpack_from('<I',r,resource+4+g*4)[0];header=struct.unpack_from('<5I',r,off)
 groups.append({'group':g,'header':list(header),'destination_base':hex(p.read(0x2007e3c+g*4)),'palette_base':p.read(0x2007f54+g*4)})
assert groups[10]['destination_base']=='0x60163a0'
for image,group in zip(p.images,groups):
 src,count,units,domain=image['args'];size=count*(32 if units==16 else 64);dst=int(group['destination_base'],16)
 assert domain==0 and dst==int(image['return'],16) and 0x6010000<=dst and dst+size<=0x6018000
 assert bytes(p.read(dst+i,1) for i in range(size))==r[src-0x8000000:src-0x8000000+size]
records=[bytes(p.read(0x2001790+g*20+i,1) for i in range(20)).hex() for g in range(p.read(0x2001780,2))]
assert p.read(0x2007e38)==0x8034634 and len(groups)>10 and p.obj_setup==1
report={'status':'PASS','Thumb_instructions':p.steps,'groups':groups,'palette_records':records,'palette_count':p.read(0x2001780,2),'bg_bytes':p.read(0x2001782,2),'obj_bytes':p.read(0x2001784,2),'palette_calls':p.palettes,'image_calls':p.images,'transfers':p.transfers,'scope':'Actualwhole27994 originaltitle resource34634, initializedtitleBGcount4/bg128/firstrecord andactual73C0 initializesOBJcursor6010000 beforecaller27994, actualE154/E388palette registrations/logicalDMA. Groupdestinationbase/headers/tablepublished/recordownership/SP/callee-saved established withactual7190. Earlierreturn0 imagecontract couldnotestablishgroupplacement andis superseded. Actual7190/7378/73C0 upload/allocation plusARM0560rawcopy logicalcontract;28BD0objectsetup explicitreturn0 contract, all11rawuploadedgroupbytes checked; notcompressedimage/fulloriginalgraphicsallocation/OAM/titlepixel/frame/parentproof.','rom_sha256':hashlib.sha256(r).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/title-sprite-allocation-rom-probe.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',p.steps,'groups',len(groups),'group10',groups[10],'palettecount',report['palette_count'],'objbytes',report['obj_bytes'])
