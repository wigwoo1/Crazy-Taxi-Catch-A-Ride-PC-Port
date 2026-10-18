"""Original E154 execution with an explicit synchronous DMA transfer contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_customer_dispatch_rom.py').read().split('\ncases=')[0])
source=source.replace("'bhi':c and not z,", "'bhi':c and not z,'bls':not c or z,'bhs':c,'blo':not c,")
exec(source);VehicleProbe.run=MotionProbe.run
cfg=json.loads((root/'reports/palette-registration-full-cfg.json').read_text())['registration256']['instructions']
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg})
class PaletteProbe(DispatchProbe):
 def read(self,a,n=4):
  if 0x40000d4<=a<0x40000e0 or 0x5000000<=a<0x5000400:
   assert a&~3 in self.arm.mem,('unmapped palette/DMA',hex(a))
   return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(n*8))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x40000d4<=a<0x40000e0 or 0x5000000<=a<0x5000400:
   sh=(a&3)*8;mask=((1<<(8*n))-1)<<sh;self.arm.mem[a&~3]=(self.arm.mem.get(a&~3,0)&~mask)|((v<<sh)&mask)
   if a==0x40000dc and v&0x80000000:
    assert v==0x84000080,hex(v)
    start=self.read(0x40000d4);dest=self.read(0x40000d8);assert dest in (0x5000000,0x5000200),hex(dest)
    for k in range(128):self.write(dest+4*k,self.read(start+4*k))
    self.dma.append({'source':hex(start),'destination':hex(dest),'bytes':512})
    self.arm.mem[a]=v&0x7fffffff
  else:super().write(a,v,n)
 def external(self,target):raise AssertionError(('unqualified callee',hex(target)))
results=[]
for name,scene in [('city1',0x7556cc),('city2',0x755734),('startup',0x7ebb90)]:
 palette=struct.unpack_from('<I',r,scene+24)[0];p=PaletteProbe((0,0,0,0,0,1,0,0,0,0));p.dma=[]
 for i in cfg:
  if 'literal' in i:
   a=int(i['literal'],16)
   if 0x2000000<=a<0x2040000:
    for off in range(0,32,4):p.write((a&~3)+off,0)
 for a in range(0x5000000,0x5000400,4):p.write(a,0)
 p.reg[:3]=[palette,32,1];p.reg[14]=0x10020001;p.run(entry=0x800e154,stop=0x10020000)
 actual=bytes(p.read(0x5000000+k,1) for k in range(512))
 assert p.dma==[{'source':'0x84a50b0','destination':'0x5000000','bytes':512}],p.dma
 expected=r[0x4a50b0:0x4a52b0];assert actual==expected and not any(actual),(name,'initial black palette clear')
 record=bytes(p.read(0x2001790+k,1) for k in range(20))
 assert struct.unpack_from('<H',record)[0]==256 and struct.unpack_from('<I',record,16)[0]==palette,(name,record.hex())
 results.append({'scene':name,'palette':hex(palette),'instructions':p.steps,'dma':p.dma,'return':signed(p.reg[0]),'record':record.hex(),'bg_palette_sha256':hashlib.sha256(actual).hexdigest(),'initial_bg_clear_equal':True,'retained_scene_palette_source':True})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':results,'scope':'Actual original E154 throughreturn, arguments scene palette/32/BG1, clean registration table/zero allocation offsets. DMA is explicit synchronous 32bit copy contract, hardware execution excluded. Actual512 initial BLACK BG palette bytes and registered scene source record verified; E154 first uploads ROM black palette084A50B0 and retains requested scene pointer for later palette service, so immediate scene-colour upload assumption was disproved. Shared wholebootstrap later allocations/overwrites and native marker pixels unverified.'}
(root/'reports/scene-bg-palette-registration-rom-probe.json').write_text(json.dumps(report,indent=2));print('PASS',report)
