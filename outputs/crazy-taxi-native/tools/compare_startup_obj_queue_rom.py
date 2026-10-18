"""Whole28288 source startup8bpp requests, originmode0 and owned buckets."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_rom.py').read().split('\ncases=')[0])
saved_allowed=allowed_thumb.copy()
source=source.replace("a=val(ops[1]);n=val(ops[2]);value=", "a=val(ops[1]) if len(ops)==3 else self.reg[rn(ops[0])];n=val(ops[2]) if len(ops)==3 else val(ops[1])&255;value=")
exec(source.replace("elif m=='cmp':self.arithmetic(val(ops[0]),val(ops[1]),True)","elif m in ['cmp','cmn']:self.arithmetic(val(ops[0]),val(ops[1]),m=='cmp')"));allowed_thumb.update(saved_allowed)
todo=[0x8028288];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x8028288<=a<=0x8028656,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='mov' and i.op_str=='pc, r0':todo.append(0x802839a);continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class QueueProbe(InitProbe):
 run=MotionProbe.run
 def external(self,t):raise AssertionError(('unexpected source helper',hex(t)))
fixtures=[(120,52,0,0,240,160,0,0,0,0),(0,0,127,127,240,160,0xffff,255,128,165),(240,160,0,1,240,160,0x2001,3,32,65535),(0x7fffffff,0x80000000,0,3,65535,65535,0x400,1,10,12345)]
cases=[(frame,x&0xffffffff,y&0xffffffff,0x12345678,bucket,priority,flags,mask,tile,total,count,seed,width,height) for frame in range(33) for flags in range(8) for bucket in range(3) for x,y,total,count,width,height,mask,priority,tile,seed in fixtures]
assets=json.loads((root/'reports/startup-animation-assets.json').read_text())
for frame in range(33):
 m=assets['frames'][frame]['metrics'];w=m[0]*8
 for inside in [0,1]:cases.append((frame,(m[2]-w+inside)&0xffffffff,m[3],0,0,0,0,0,0,0,0,0,240,160))
 for total in [128,255]:cases.append((frame,120,52,0,0,0,0,0,0,total,0,77,240,160))
rows=subprocess.run([str(root/'bin/startup-obj-queue-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 frame,x,y,z,bucket,priority,flags,mask,tile,total,count,seed,width,height=case;p=QueueProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10001000:bytearray(struct.pack('<III',x,y,z)),0x20081ec:bytearray(1024),0x20085ec:bytearray(1024),0x20089ec:bytearray(1024),0x2008dec:bytearray(1024)}
 p.scalars={0x2007e2c:99,0x2007e2d:165,0x2007e2e:0,0x2007e2f:total,0x2007e32:count,0x2007e33:count,0x2007e34:count,0x2007e38:0x8082104,0x2007e3c:(0x6010000+(tile<<5))&0xffffffff,0x2001788:mask,0x2007542:width,0x2007544:height}
 for b,address in enumerate([0x20081ec,0x20085ec,0x20089ec]):
  for i in range(128):
   for j in range(4):p.write(address+i*8+j*2,seed+b*1000+i*37+j*53,2)
 for i in range(128):
  for j in range(4):p.write(0x2008dec+i*8+j*2,seed+i*37+j*53,2)
 p.reg[:4]=[frame,0,0x10001000,priority];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 for i,value in enumerate([bucket,flags,0]):p.write(p.reg[13]+i*4,value)
 saved=[p.reg[i] for i in range(4,12)];p.run(entry=0x8028288,stop=0x10020000)
 assert p.reg[13]==0x10000400 and saved==[p.reg[i] for i in range(4,12)]
 parts=row.split();got=[int(p.reg[0]!=0)]+[p.read(address,1) for address in [0x2007e2f,0x2007e32,0x2007e33,0x2007e34,0x2007e2c,0x2007e2d]]
 assert got==list(map(int,parts[:7])),(case,got,parts[:7]);data=b''.join(p.regions[address] for address in [0x20081ec,0x20085ec,0x20089ec,0x2008dec]);assert data==bytes.fromhex(parts[7]),(case,'queue data mismatch')
 if p.reg[0]:assert p.reg[0]==[0x20081ec,0x20085ec,0x20089ec][bucket]+count*8
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole28288 entry..caller return foractual33startup8bpp frames/originmode0/buckets0..2, all8flip/semi flag combinations, priority0/1/3/255,objmask400/2001/FFFF,tilebases0/10/32/128, bytecounter127->128/negative disabled guard, clip boundaries/offscreen/wrappedanchors. ALL4096queued/depthbytes/counters/lastdimensions/source return pointer andcallee-savedregs/SP compared. Original asset shape family squares8..64 and32x16; native bounds bucket>2/count>=128 unsupported, no other asset families/affineproducer/finalpixel/rendering/liveviewer claim.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/startup_obj_queue.c',root/'src/startup_obj_queue.h',root/'src/startup_obj.c',root/'src/startup_obj.h',root/'tests/startup-obj-queue-rom-compare.c',Path(__file__),root/'bin/startup-obj-queue-rom-compare.exe',root/'assets/startup-animation/sprites.crs']}}
(root/'reports/startup-obj-queue-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
