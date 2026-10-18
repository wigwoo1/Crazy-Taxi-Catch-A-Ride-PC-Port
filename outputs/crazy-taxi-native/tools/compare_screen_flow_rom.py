"""Actual generic16542..6654 timer/input/fade/drain control."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
todo=[0x8016542];seen=set()
while todo:
 a=todo.pop()
 if a in [0x8016454,0x8016656] or a in seen:continue
 assert 0x8016542<=a<=0x8016654,hex(a);seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class ScreenProbe(InitProbe):
 def external(self,target):
  assert target in [0x80267c8,0x800e5a8,0x802776c],hex(target)
  args=[0,0,0,0]
  if target==0x80267c8:
   assert self.reg[0]==struct.unpack_from('<I',r,0x7ec658+20)[0];args=[5,self.reg[1],self.reg[2],0]
  elif target==0x800e5a8:args=self.reg[:4]
  self.calls+=[target]+args+[self.read(a) for a in [0x200359c]]+[self.reg[10]]+[self.read(a) for a in [0x2003898,0x2001771,0x2001772]]
  self.reg[0]=0
fixtures=[(180,0,1,0,160,0,0,0,0),(1,59,1,0,160,0,0,0,0),(300,59,1,0,160,0,1,0,255),(300,0,60,0,160,0,0,255,0),(1,0,60,0x80167d1,160,0,1,0,255),(300,60,1,0x80167d1,160,0,1,0,255),(300,60,1,0,159,0,1,1,255),(300,60,1,0,160,255,1,1,255),(0,60,1,0,160,0,1,1,255),(0x7fffffff,0x7fffffff,60,0,160,0,1,0,255),(1,0xffffffff,0xffffffff,0,160,0,0,0,0),(0xffffffff,60,1,0,160,0,1,1,255)]
cases=[(remaining,elapsed,delta,flags,callback,height,fade,confirm,start,sound) for flags in range(256) for remaining,elapsed,delta,callback,height,fade,confirm,start,sound in fixtures]
rows=subprocess.run([str(root/'bin/screen-flow-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 remaining,elapsed,delta,flags,callback,height,fade,confirm,start,sound=case;p=ScreenProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10000600:bytearray(r[0x49d560:0x49d560+28]),0x2003500:bytearray([confirm,0,0,start])};p.scalars={0x200359c:remaining,0x200177b:fade,0x20035a0:0x10000600,0x2003b00:delta,0x2007602:sound,0x2001771:99,0x2001772:165,0x2003898:77};p.calls=[]
 p.write(0x10000600+25,flags,1);p.write(0x10000600+20,callback);p.reg[9]=height;p.reg[10]=elapsed;p.reg[13]=0x10000400
 parts=list(map(int,line.split()));expected_remaining=parts[0];stop=0x8016454 if 0<expected_remaining<0x80000000 else 0x8016656
 p.run(entry=0x8016542,stop=stop)
 got=[p.read(0x200359c),p.reg[10],p.read(0x2001771),p.read(0x2001772),p.read(0x2003898),len(p.calls)//10]+p.calls
 assert got==parts,(case,got,parts);steps+=p.steps
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Actual16542..6654 generic post-presentation screen control vsnative: ALL256 descriptorflags x12 boundary/gate fixtures, wrappedremaining/elapsed, exactdrawing160/fadeinactive gates, time/input flags andspecialcallback167D1, ordered soundindex5/fade masks/drain requests andstate snapshots at eachcall. Sound/fade/drain bodies arecontracts; source descriptors0..3 initialization/rendering/streaming/rowadvance lifecycle not yetcomposed.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/screen_flow.c',root/'src/screen_flow.h',root/'tests/screen-flow-rom-compare.c',Path(__file__),root/'bin/screen-flow-rom-compare.exe']}}
(root/'reports/screen-flow-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
