"""Normal restart mask1 registered banks vs original E5A8..EDDC."""
exec(open('outputs/crazy-taxi-native/tools/compare_palette_fade_start_rom.py').read().split('\nvalues=')[0])
saved=allowed_thumb.copy()
exec(source.replace("'bhi':c and not z", "'bhs':c,'blo':not c,'bhi':c and not z"))
allowed_thumb.update(saved);StartProbe.run=MotionProbe.run
todo=[0x0800ee10];seen=set()
while todo:
 a=todo.pop()
 if a==0x0800f3c4 or a in seen:continue
 assert 0x0800ee10<=a<0x0800f3c4,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class BankProbe(StartProbe):
 def __init__(self,*args):self.records=bytearray(2560);self.palette=bytearray(512);self.banks=bytearray([0xa5]*1024);self.temp=bytearray(1536);self.dma={};super().__init__(*args)
 def read(self,a,size=4):
  if 0x02001790<=a<0x02002190:return int.from_bytes(self.records[a-0x02001790:a-0x02001790+size],'little')
  if 0x084a5a34<=a<0x084a5c34:return int.from_bytes(self.palette[a-0x084a5a34:a-0x084a5a34+size],'little')
  if 0x05000000<=a<0x05000400:return int.from_bytes(self.banks[a-0x05000000:a-0x05000000+size],'little')
  if 0x03004cb0<=a<0x030052b0:return int.from_bytes(self.temp[a-0x03004cb0:a-0x03004cb0+size],'little')
  if a in self.dma:return self.dma[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x05000000<=a<0x05000400:self.banks[a-0x05000000:a-0x05000000+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  elif 0x03004cb0<=a<0x030052b0:self.temp[a-0x03004cb0:a-0x03004cb0+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  elif a in [0x040000d4,0x040000d8,0x040000dc]:
   self.dma[a]=v&MASK
   if a==0x040000dc:
    assert v&0xffff0000==0x84000000,hex(v)
    src,dst=self.dma[0x040000d4],self.dma[0x040000d8]
    words=[self.read(src+j*4) for j in range(v&65535)]
    for j,w in enumerate(words):self.write(dst+j*4,w)
  else:super().write(a,v,size)
 def external(self,t):
  assert t==0x08032488,hex(t);self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
cases=[(mode,32,1,1,limit,active,domain,16,offset,131,progress,fade_active) for mode in [0,1] for limit,active in [(32,1),(33,1),(32,0)] for domain in [0,1] for offset in [0,32,480] for progress in [0,15,31] for fade_active in [0,1]]
run=subprocess.run([str(root/'bin/palette-fade-registered-frame-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=run.stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 mode,cap,bg,obj,limit,active,domain,colors,offset,seed,progress,fade_active=case
 p=BankProbe((0,0,0,0,0,1,0,0,0,0));p.globals=bytearray(32)
 for off,value,size in [(3,mode,1),(11,fade_active,1),(12,256,2),(14,256,2),(16,1,2),(22,bg,2),(24,obj,2),(26,progress,2),(28,cap,2)]:p.write(0x02001770+off,value,size)
 for off in range(5,11):p.write(0x02001770+off,1,1)
 for off,value in [(0,colors),(2,offset),(4,limit)]:p.records[off:off+2]=value.to_bytes(2,'little')
 p.records[6]=active;p.records[7]=domain;p.records[16:20]=(0x084a5a34).to_bytes(4,'little')
 for i in range(256):p.palette[i*2:i*2+2]=((i*131+seed)&65535).to_bytes(2,'little')
 p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x0800ee10,stop=0x0800f3c4)
 assert bytes(p.banks)==bytes.fromhex(row),(case,bytes(p.banks).hex(),row)
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'EE10..F3C2 normal0/1 mask1 in both banks, equal scale fields, nonanimated records, 16color palettes at bank offsets0/32/480, all1024bank bytes. Explicit DMA data backend; motion/timing tail excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/palette-fade-small-frame-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
