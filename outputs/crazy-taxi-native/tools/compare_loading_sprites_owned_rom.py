"""WholeD764 with actual nested clear/fill/string/image source routines."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_clear_rom.py').read().split('\nrects=')[0])
for start,end in [(0xd764,0xd8a8),(0x28fe4,0x29008),(0x29010,0x29016)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/tip-bitmap-copy-cfg.json').read_text())['instructions']})
saved=allowed_thumb.copy();exec(source.replace('self.steps<50000','self.steps<500000'));allowed_thumb.update(saved);ClearProbe.run=MotionProbe.run
class IndexedProbe(ClearProbe):
 def external(self,target):
  if target in [0x802c9f0,0x8033f2c,0x8028fe4,0x8031ef8]:self.run(entry=target,stop=self.reg[14]&~1);return
  used={0x800f7dc:9,0x8028288:7,0x8027fb8:5,0x8026318:0};assert target in used,hex(target);count=used[target]
  args=[self.reg[i] if i<4 else self.read(self.reg[13]+4*(i-4)) for i in range(count)]+[0]*(9-count);pos=[0]*3
  if target==0x8028288:pos=[self.read(args[2]+4*i) for i in range(3)];args[2]=0
  self.calls.append([target,*args,*pos]);self.reg[0]=0
# Reachable source27FB8 instruction map, no runtime dispatcher.
todo=[0x8027fb8];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x8027fb8<=a<=0x80280c0;seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in ['beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl']:todo.append(i.operands[0].imm)
source=source.replace("   elif m=='stm':", """   elif m=='ldm':
    base=rn(ops[0]);address=self.reg[base];registers=sorted(rn(o) for o in ops[1:])
    for j,k in enumerate(registers):self.reg[k]=self.read(address+4*j)
    self.reg[base]=(address+4*len(registers))&MASK
   elif m=='stm':""")
saved=allowed_thumb.copy();exec(source.replace('self.steps<50000','self.steps<500000').replace("'bhi':c and not z","'bhs':c,'blo':not c,'bhi':c and not z"));allowed_thumb.update(saved);ClearProbe.run=MotionProbe.run
class SpriteProbe(IndexedProbe):
 def __init__(self,*args):self.hardware={};super().__init__(*args)
 def read(self,a,size=4):
  if a in self.hardware:return self.hardware[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  if a in self.hardware:
   self.hardware[a]=v&((1<<(8*size))-1)
   if a==0x40000dc and v&0x80000000:
    source=self.hardware[0x40000d4]-0x8000000;dest=self.hardware[0x40000d8]-0x6010000;length=(v&65535)*4
    assert 0<=source<=len(r)-length and 0<=dest<=len(self.objects)-length
    self.objects[dest:dest+length]=r[source:source+length]
  else:super().write(a,v,size)
 def external(self,target):
  if target==0x8027fb8:self.run(entry=target,stop=self.reg[14]&~1);return
  super().external(target)

cases=[(a,b,w,h,fill) for a,b in [(0,0),(0x60,0),(0x80,0),(0,1)] for w,h in [(240,160),(160,100)] for fill in [0,0x78563412]]
capture=root/'reports/loading-sprites-native-capture.bin'
subprocess.run([str(root/'bin/loading-sprites-owned-rom-compare.exe'),'Crazy Taxi - Catch a Ride (USA).gba',str(capture)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
data=capture.read_bytes();offset=steps=0
for a,b,w,h,fill in cases:
 count=int.from_bytes(data[offset:offset+4],'little');offset+=4
 want=list(struct.unpack_from('<'+'I'*(count*13),data,offset));offset+=count*52;frame=data[offset:offset+38400];offset+=38400
 p=SpriteProbe((0,0,0,0,0,1,0,0,0,0));p.hardware={0x40000d4:0x11111111,0x40000d8:0x22222222,0x40000dc:0x33333333};p.objects=bytearray([165])*32768;p.calls=[];p.frame=bytearray([165])*38400;p.fill=fill;p.reg[13]=0x10018000;p.reg[14]=0x10020001
 for addr,value,size in [(0x2007542,w,2),(0x2007544,h,2),(0x2001eb0,a,1),(0x2001708,b,1),(0x2001684,0,1),(0x2009a88,5,1),(0x2009aa0,0x849dfe0,4)]:p.write(addr,value,size)
 p.write(0x2007e38,0x08070e50);p.write(0x20094ec,0,2)
 for i in range(32):p.write(0x2007e3c+i*4,0x06010000)
 for i in range(0,768,4):p.write(0x20091ec+i,0xa5a5a5a5)
 p.run(entry=0x800d764,stop=0x10020000)
 assert len(p.calls)==count and [v for row in p.calls for v in row]==want,(a,b,w,h,fill,'calls')
 assert bytes(p.frame)==frame,(a,b,w,h,fill,'frame')
 header=struct.pack('<4I',p.read(0x20094ec,2),p.hardware[0x40000d4],p.hardware[0x40000d8],p.hardware[0x40000dc]);queue=bytes(p.read(0x20091ec+i,1) for i in range(768));wantobjects=header+queue+p.objects
 assert data[offset:offset+len(wantobjects)]==wantobjects,(a,b,w,h,fill,'objects');offset+=len(wantobjects);steps+=p.steps
assert offset==len(data)
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,framebuffer_bytes_compared=len(cases)*38400,object_bytes_compared=len(cases)*32768,rom_sha256=hashlib.sha256(r).hexdigest(),capture_sha256=hashlib.sha256(data).hexdigest(),scope='WholeD764 + actual clear/fill/string/image/27FB8 againstnative loading_sprites_owned. All indexedframe/object/queue/requestbytes; logical immediateDMA copy on source register publication. Remainingtext/28288/26318 contracts; actual allocator groupbases fixture6010000, not whole rendered loading or audio/fade timing proof.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/loading_sprites_owned.c','src/loading_sprites_owned.h','src/sprite_replacement_owned.c','src/loading_indexed_owned.c','src/loading_indexed_owned.h','src/loading_strings_owned.c','src/loading_present.c','src/loading_image.c','tests/loading_sprites_owned_rom_compare.c','tools/compare_loading_sprites_owned_rom.py']})
(root/'reports/loading-sprites-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
