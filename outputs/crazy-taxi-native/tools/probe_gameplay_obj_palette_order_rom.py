"""Original HUD asset parser and nested palette registrations; image helpers opaque."""
exec(open('outputs/crazy-taxi-native/tools/probe_scene_bg_palette_registration_rom.py').read().split('\nresults=[]')[0])
registration_cfg=cfg
source=source.replace("   elif m=='pop':", """   elif m=='ldm':
    base=rn(ops[0]);regs=sorted(rn(o) for o in ops[1:]);assert base not in regs,('unqualified LDM base in list',hex(pc))
    address=self.reg[base]
    for j,k in enumerate(regs):self.reg[k]=self.read(address+4*j)
    self.reg[base]=(address+4*len(regs))&MASK
   elif m=='pop':""")
exec(source);VehicleProbe.run=MotionProbe.run
cfg=registration_cfg
allowed_thumb.update({int(i["address"],16):bytes.fromhex(i["bytes"]) for i in cfg})
functions=[(0x27994,0x27cd0),(0xe388,0xe5a8)];instructions=[]
for lo,hi in functions:
 todo=[lo];seen={}
 while todo:
  off=todo.pop()
  if off in seen:continue
  assert lo<=off<hi,hex(off)
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
  instructions.append(row);allowed_thumb[i.address]=bytes(i.bytes)
class LoaderProbe(PaletteProbe):
 def write(self,a,v,n=4):
  if a==0x40000dc and v&0x80000000:
   assert n==4 and v&0xffff0000==0x84000000,hex(v)
   count=v&65535;assert 1<=count<=128,count
   start=self.read(0x40000d4);dest=self.read(0x40000d8)
   assert 0x2007e38<=dest and dest+4*count<=0x2009eec or 0x5000000<=dest and dest+4*count<=0x5000400,hex(dest)
   for k in range(count):self.write(dest+4*k,self.read(start+4*k))
   self.dma.append({'source':hex(start),'destination':hex(dest),'bytes':4*count});self.arm.mem[a]=v&0x7fffffff;return
  super().write(a,v,n)
 def external(self,target):
  if target in (0x800e388,0x800e154):
   self.registrations.append({'source':hex(self.reg[0]),'argument':self.reg[1],'BG':self.reg[2],'index_before':self.read(0x2001780,2)})
   self.run(entry=target,stop=self.reg[14]&~1)
  elif target in (0x8007190,0x8028bd0):
   self.image_helpers.append(hex(target));self.reg[0]=0
  else:raise AssertionError(('unknown asset helper',hex(target)))
results=[]
for name,asset in [('flag1708_nonzero',0x805a338),('flag1708_zero',0x8070e50)]:
 p=LoaderProbe((0,0,0,0,0,1,0,0,0,0));p.dma=[];p.registrations=[];p.image_helpers=[]
 for i in cfg+instructions:
  if 'literal' in i:
   a=int(i['literal'],16)
   if 0x2000000<=a<0x2040000:
    for off in range(0,32,4):p.write((a&~3)+off,0)
 for a in range(0x2001790,0x2001790+32*20,4):p.write(a,0)
 for a in range(0x5000000,0x5000400,4):p.write(a,0)
 p.reg[:2]=[asset,1];p.reg[14]=0x10020001;p.run(entry=0x8027994,stop=0x10020000)
 index=p.read(0x2001780,2);records=[]
 for j in range(index):records.append(bytes(p.read(0x2001790+20*j+k,1) for k in range(20)).hex())
 assert all(q['BG']==0 for q in p.registrations),p.registrations
 palette=struct.unpack_from('<I',r,0x7556cc+24)[0];p.reg[:3]=[palette,32,1];p.reg[14]=0x10020001;p.run(entry=0x800e154,stop=0x10020000)
 assert p.read(0x2001780,2)==index+1 and p.read(0x2001790+20*index+16)==palette
 results.append({'path':name,'asset':hex(asset),'obj_records':index,'scene_record_index':index,'registrations':p.registrations,'records':records,'instructions':p.steps,'opaque_image_helpers':p.image_helpers})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':results,'scope':'Actual27994 HUD asset parsing with actual nestedE154/E388 palette registrations thenactual sceneE154, clean reset state; DMA32bit bounded copycontract. Image/decompression7190 and28BD0 explicit opaque helpers returning0; full original image memory/allocation and boot not executed. Palette effects of these helpers remain outside scope.'}
(root/'reports/gameplay-obj-palette-order-rom-probe.json').write_text(json.dumps(report,indent=2));print('PASS',report)
