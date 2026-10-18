"""Whole28658 startup scaled sprite producer; diagnostic ROM comparison."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_rom.py').read().split('\ncases=')[0])
saved_allowed=allowed_thumb.copy()
source=source.replace("a=val(ops[1]);n=val(ops[2]);value=", "a=val(ops[1]) if len(ops)==3 else self.reg[rn(ops[0])];n=val(ops[2]) if len(ops)==3 else val(ops[1])&255;value=")
exec(source.replace("elif m=='cmp':self.arithmetic(val(ops[0]),val(ops[1]),True)","elif m in ['cmp','cmn']:self.arithmetic(val(ops[0]),val(ops[1]),m=='cmp')"));allowed_thumb.update(saved_allowed)
todo=[0x8028288,0x8028658];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x8028288<=a<=0x8028bcc,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='mov' and i.op_str=='pc, r0':todo.append(0x802839a if a<0x8028658 else 0x80287ec);continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
division_namespace={}
exec(open('outputs/crazy-taxi-native/tools/compare_projection_divide_rom.py').read().split('\nvalues=')[0],division_namespace)
class QueueProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):
  if t==0x8028288:self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x8032454:
   assert self.reg[2]==0x80029fc
   self.reg[0]=division_namespace['original'](self.reg[0],self.reg[1])
  else:raise AssertionError(('unexpected source helper',hex(t)))
assets=json.loads((root/'reports/startup-animation-assets.json').read_text())
cases=[(f,120,52,123,b,2,4,0x400,10,0,0,165,240,160,scale,state) for f in range(33) for b in [0,1,2] for scale in [128,255,256,264,512] for state in [0,2,3,4]]
for f in range(33):
 m=assets['frames'][f]['metrics']
 for scale in [128,255,256,264,512]:
  fw=m[0]*8*(2 if scale>256 else 1);fh=m[1]*8*(2 if scale>256 else 1)
  ox=m[2] if scale==256 else fw//2;oy=m[3] if scale==256 else fh//2
  for x,y in [(ox-fw,52),(ox-fw+1,52),(240+ox,52),(239+ox,52),(120,oy-fh),(120,oy-fh+1),(120,160+oy),(120,159+oy)]:
   cases.append((f,x&0xffffffff,y&0xffffffff,123,2,255,7,0x400,128,127,127,65535,240,160,scale,0))
  for total in [128,255]:cases.append((f,120,52,123,0,0,0,0,0,total,0,165,240,160,scale,0))
rows=subprocess.run([str(root/'bin/startup-obj-scaled-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 frame,x,y,z,bucket,priority,flags,mask,tile,total,count,seed,width,height,scale,state=case;p=QueueProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10001000:bytearray(struct.pack('<III',x,y,z)),0x20081ec:bytearray(1024),0x20085ec:bytearray(1024),0x20089ec:bytearray(1024),0x2008dec:bytearray(1024),0x200806c:bytearray(384)}
 p.scalars={0x2007e31:0,0x3006940:0x80029fc,0x20304f0:256,0x202c4f0:0,0x2007e2c:99,0x2007e2d:165,0x2007e2e:0,0x2007e2f:total,0x2007e32:count,0x2007e33:count,0x2007e34:count,0x2007e38:0x8082104,0x2007e3c:(0x6010000+(tile<<5))&0xffffffff,0x2001788:mask,0x2007542:width,0x2007544:height}
 for b,address in enumerate([0x20081ec,0x20085ec,0x20089ec]):
  for i in range(128):
   for j in range(4):p.write(address+i*8+j*2,seed+b*1000+i*37+j*53,2)
 for i in range(128):
  for j in range(4):p.write(0x2008dec+i*8+j*2,seed+i*37+j*53,2)
 for i in range(32):
  p.write(0x200806c+i*12,1 if state>=3 else state)
  for j in range(4):p.write(0x200806c+i*12+4+j*2,seed+i*37+j*53,2)
 if state>=3:
  if state==3:p.write(0x200806c,0)
  for j,v in enumerate([65536//scale,0,0,65536//scale]):p.write(0x200806c+31*12+4+j*2,v,2)
 p.reg[:4]=[frame,0,0x10001000,priority];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 for i,value in enumerate([bucket,flags,0,scale,0]):p.write(p.reg[13]+i*4,value)
 saved=[p.reg[i] for i in range(4,12)];p.run(entry=0x8028658,stop=0x10020000)
 assert p.reg[13]==0x10000400 and saved==[p.reg[i] for i in range(4,12)]
 parts=row.split();got=[int(p.reg[0]!=0)]+[p.read(address,1) for address in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d]]
 assert got==list(map(int,parts[:7])),(case,got,parts[:7]);data=b''.join(p.regions[address] for address in [0x20081ec,0x20085ec,0x20089ec,0x2008dec]);assert data==bytes.fromhex(parts[7]),(case,'queue data mismatch')
 if p.reg[0]:assert p.reg[0]==[0x20081ec,0x20085ec,0x20089ec][bucket]+count*8
 assert p.read(0x2007e31,1)==int(parts[8]);assert p.regions[0x200806c]==bytes.fromhex(parts[9]);steps+=p.steps

report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'nested_ARM_instructions':division_namespace['steps'],'scope':'Whole28658 startup8bpp positivezeroangle origin0; actual isolatedARM029FC signeddivision; startup relocation notexecuted. Allqueue bytes/counters andcallerregisters; matrixrecords checked separately.'}
report['hashes']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/startup_obj_transport.c',root/'src/startup_obj_transport.h',root/'tests/startup-obj-scaled-rom-compare.c',Path(__file__),root/'bin/startup-obj-scaled-rom-compare.exe']}
(root/'reports/startup-obj-scaled-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
