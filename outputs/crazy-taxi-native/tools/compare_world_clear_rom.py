"""Actual2C9F0 rectangle clear with complete33F2C byte-fill callee."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
source=source.replace("   elif m=='pop':", """   elif m=='stm':
    base=rn(ops[0]);address=self.reg[base];registers=sorted(rn(o) for o in ops[1:])
    for j,k in enumerate(registers):self.write(address+4*j,self.reg[k])
    self.reg[base]=(address+4*len(registers))&MASK
   elif m=='pop':""").replace('self.steps<1000','self.steps<1000000')
source=source.replace('    self.reg[13]+=4*len(regs)', '    self.reg[13]+=4*len(regs)\n    if 15 in regs:nextpc=self.reg[15]&~1')
saved=allowed_thumb.copy();exec(source);allowed_thumb.update(saved);ExitProbe.run=MotionProbe.run
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
for start,end in [(0x802c9f0,0x802ca72),(0x8033f2c,0x8033f7e)]:
 todo=[start];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert start<=a<end,hex(a);seen.add(a)
  i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':
   todo.append(int(i.op_str.lstrip('#'),16))
   if i.mnemonic!='b':todo.append(a+i.size)
  else:todo.append(a+i.size)
frame_slot=int.from_bytes(r[0x2ca74:0x2ca78],'little')+4
fill_slot=int.from_bytes(r[0x2ca78:0x2ca7c],'little')
class ClearProbe(ExitProbe):
 def read(self,a,size=4):
  if a==frame_slot:return 0x21000000
  if a==fill_slot:return self.fill
  if 0x21000000<=a and a+size<=0x21009600:return int.from_bytes(self.frame[a-0x21000000:a-0x21000000+size],'little')
  return super().read(a,size)
 def write(self,a,value,size=4):
  if 0x21000000<=a and a+size<=0x21009600:self.frame[a-0x21000000:a-0x21000000+size]=(value&((1<<(8*size))-1)).to_bytes(size,'little')
  else:super().write(a,value,size)
 def external(self,target):
  assert target==0x8033f2c,hex(target);self.fills+=1;self.run(entry=target,stop=self.reg[14]&~1)
rects=[(0,0,240,160),(-5,-3,12,8),(237,158,12,8),(240,0,8,8),(0,160,8,8),(-20,0,4,4),(0,-20,4,4),(4,5,0,4),(4,5,4,0),(1,2,7,3),(0,0,3,1),(0,0,16,1)]
cases=[(*rect,fill) for rect in rects for fill in [0,0x78563412,0xffffffff]]
cp=subprocess.run([str(root/'bin/world-clear-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=cp.stdout.splitlines();assert len(rows)==len(cases);steps=fills=0
for c,want in zip(cases,rows):
 p=ClearProbe((0,0,0,0,0,1,0,0,0,0));p.frame=bytearray([165])*38400;p.fill=c[4];p.fills=0;p.reg[:4]=[v&MASK for v in c[:4]];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x802c9f0,stop=0x10020000)
 assert p.frame==bytes.fromhex(want),c
 steps+=p.steps;fills+=p.fills
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,fill_calls=fills,rom_sha256=hashlib.sha256(r).hexdigest(),frame_slot=hex(frame_slot),fill_slot=hex(fill_slot),scope='Complete original2C9F0 plus actual33F2C callee; all38400 bytes compare. Fixed viewport, clipped/unaligned/empty/offscreen rectangles, distinct fill bytes. Explicit frame-pointer/fill-global fixtures; excludes host/GPU/restart integration.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_background.c','src/world_background.h','tests/world_clear_rom_compare.c','tools/compare_world_clear_rom.py']})
(root/'reports/world-clear-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
