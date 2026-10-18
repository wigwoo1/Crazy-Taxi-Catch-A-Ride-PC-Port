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
cases=[(a,b,w,h,fill) for a,b in [(0,0),(0x60,0),(0x80,0),(0,1)] for w,h in [(240,160),(160,100)] for fill in [0,0x78563412]]
capture=root/'reports/loading-indexed-native-capture.bin'
subprocess.run([str(root/'bin/loading-indexed-owned-rom-compare.exe'),'Crazy Taxi - Catch a Ride (USA).gba',str(capture)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
data=capture.read_bytes();offset=steps=0
for a,b,w,h,fill in cases:
 count=int.from_bytes(data[offset:offset+4],'little');offset+=4
 want=list(struct.unpack_from('<'+'I'*(count*13),data,offset));offset+=count*52;frame=data[offset:offset+38400];offset+=38400
 p=IndexedProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.frame=bytearray([165])*38400;p.fill=fill;p.reg[13]=0x10018000;p.reg[14]=0x10020001
 for addr,value,size in [(0x2007542,w,2),(0x2007544,h,2),(0x2001eb0,a,1),(0x2001708,b,1),(0x2001684,0,1),(0x2009a88,5,1),(0x2009aa0,0x849dfe0,4)]:p.write(addr,value,size)
 p.run(entry=0x800d764,stop=0x10020000)
 assert len(p.calls)==count and [v for row in p.calls for v in row]==want,(a,b,w,h,fill,'calls')
 assert bytes(p.frame)==frame,(a,b,w,h,fill,'frame');steps+=p.steps
assert offset==len(data)
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,framebuffer_bytes_compared=len(cases)*38400,rom_sha256=hashlib.sha256(r).hexdigest(),capture_sha256=hashlib.sha256(data).hexdigest(),scope='WholeD764+actual nested2C9F0/33F2C/28FE4/31EF8 versusnative loading_indexed_owned. Every38400indexedframebytes + orderedremainingservicearguments. Original ordinary/special/override gates, full/partial viewport, distinct lowbyte fill. Text/sprite/26318 remainreturncontracts, no fullrender/audio/fade/displayproof.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/loading_indexed_owned.c','src/loading_indexed_owned.h','src/loading_strings_owned.c','src/loading_present.c','src/loading_image.c','tests/loading_indexed_owned_rom_compare.c','tools/compare_loading_indexed_owned_rom.py']})
(root/'reports/loading-indexed-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
