"""Whole source callback on initial phases255/0/1/2/3/4/5; later phases pending."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
saved_allowed=allowed_thumb.copy()
exec(source.replace("elif m=='bl':self.reg[14]=nextpc;self.external(val(ops[0]))","elif m=='bl':\n    self.reg[14]=nextpc\n    if val(ops[0])==0x801729a:nextpc=0x801729a\n    else:self.external(val(ops[0]))"))
allowed_thumb.update(saved_allowed)
todo=[0x80167d0];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x80167d0<=a<=0x80172a8,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b' or (i.mnemonic=='bl' and i.operands[0].imm==0x801729a):todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class AnimationProbe(InitProbe):
 run=MotionProbe.run
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a in [0x4000010,0x4000050,0x4000052]:self.events.append([{0x4000010:1,0x4000050:2,0x4000052:3}[a],v&65535]+[0]*10)
  if 0x5000260<=a<0x500027a:self.events.append([6,(a-0x5000260)//2,v&65535]+[0]*9)
  if a==0x40000dc:
   assert v==0x84000080
   source=self.read(0x40000d4);destination=self.read(0x40000d8);assert destination==0x20035a4
   self.regions[destination][:]=b''.join(self.read(source+i,1).to_bytes(1,'little') for i in range(512))
 def external(self,t):
  if t==0x800e5a8:self.events.append([7]+self.reg[:4]+[0]*7);self.reg[0]=0;return
  assert t in [0x8028288,0x8028658],hex(t)
  scaled=t==0x8028658;sp=self.reg[13];pos=self.reg[2]
  values=[self.reg[0],self.reg[1]]+[self.read(pos+i*4) for i in range(3)]+[self.read(sp+i*4) for i in range(3)]+[self.read(sp+12) if scaled else 0,self.reg[3],self.read(sp+16) if scaled else 0]
  self.events.append([5 if scaled else 4]+values);self.reg[0]=0
fixtures=[(4096,64,65536,768,512,20,0),(65,128,24577,1,264,0,165),(0xffffffff,0x80000000,0x80000000,0xffffffff,0x80000000,0x80000000,65535),(0x7fffffff,0xffffffff,0,0x80000000,65535,0x7fffffff,12345)]
cases=[(phase,a,astep,x,xstep,scale,delay,fade,mode,width,seed,1,0,0) for phase in [255,0,1,2,3,4,5,6,7,8,9,127,128,254] for a,astep,x,xstep,scale,delay,seed in fixtures for fade,mode in [(0,0),(1,1),(255,1),(255,255)] for width in [0,240,3840,65535]]
cases += [(255,0,0,0,0,0,0,fade,mode,240,seed,519,0,0) for fade,mode,seed in [(0,0,7),(1,1,123),(255,255,999)]]
cases += [(9,4096,512,0,0,256,delay,0,0,240,13,1,confirm,start) for delay in [58,59,298,299,0x7fffffff,0xffffffff] for confirm,start in [(0,0),(1,0),(0,255)]]
cases += [(8,4096,512,0,0,256,0,fade,mode,240,seed,1,0,0) for seed in [0,1,12,13,0x8000000d,0xfffffff9] for fade,mode in [(0,0),(255,0),(255,1)]]
rows=subprocess.run([str(root/'bin/startup-animation-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==sum(c[11] for c in cases)
cursor=0;steps=0
for case in cases:
 phase,a,astep,x,xstep,scale,delay,fade,mode,width,seed,frames,confirm,start=case
 p=AnimationProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2003574:bytearray(44),0x5000260:bytearray(26),0x10002000:bytearray(r[0x49d560+84:0x49d560+112]),0x2003500:bytearray([confirm,0,0,start]),0x20037a4:bytearray(208),0x20035a4:bytearray(512),0x10001000:bytearray(512)};p.scalars={0x20035a0:0x10002000,0x2001771:seed&255,0x2001772:seed&255,0x200177b:fade,0x2001773:mode,0x2007542:width,0x20017b4:0x10001000,0x4000010:123,0x4000050:456,0x4000052:789,0x40000d4:0,0x40000d8:0,0x40000dc:0};p.events=[]
 for owner,val,size in [(0x2003574,phase,1),(0x2003578,a,4),(0x200357c,astep,4),(0x2003580,x,4),(0x2003584,xstep,4),(0x2003588,scale,4),(0x200358c,delay,4),(0x2003590,seed,4),(0x2003594,seed,4),(0x200359c,seed,4)]:p.write(owner,val,size)
 for i in range(256):p.write(0x10001000+i*2,seed+i*37,2);p.write(0x20035a4+i*2,seed+i*53,2)
 for owner in [0x20037cc,0x20037d0,0x20037d4]:p.write(owner,seed)
 p.write(0x2003598,seed%14)
 for i in range(13):p.write(0x5000260+i*2,seed+i*53,2)
 for i in range(13):
  for owner,size in [(0x20037a4+i,1),(0x20037b2+i*2,2),(0x20037d8+i*4,4),(0x200380c+i*4,4),(0x2003840+i*4,4)]:p.write(owner,seed+i*53,size)
 for frame in range(frames):
  p.events=[];p.steps=0;p.reg[13]=0x10000400;p.reg[14]=0x10020001
  saved=[p.reg[i] for i in range(4,12)];p.run(entry=0x80167d0,stop=0x10020000)
  assert p.reg[13]==0x10000400 and saved==[p.reg[i] for i in range(4,12)]
  got=[p.read(0x2003574,1)]+[p.read(addr) for addr in [0x2003578,0x200357c,0x2003580,0x2003584,0x2003588,0x200358c]]+[p.read(addr,2) for addr in [0x4000010,0x4000050,0x4000052]]+[p.read(0x2003590),p.read(0x2003594)]+[int(p.read(0x20017b4)==0x20035a4),len(p.events)]+[v for e in p.events for v in e]+[p.read(owner) for owner in [0x2003598,0x20037cc,0x20037d0,0x20037d4]]+[p.read(owner,size) for i in range(13) for owner,size in [(0x20037a4+i,1),(0x20037b2+i*2,2),(0x20037d8+i*4,4),(0x200380c+i*4,4),(0x2003840+i*4,4)]]+[p.read(0x200359c),p.read(0x2001771,1),p.read(0x2001772,1)]+[p.read(0x5000260+i*2,2) for i in range(13)]+[p.read(0x20035a4+i*2,2) for i in range(256)]
  expected=list(map(int,rows[cursor].split()));cursor+=1
  assert got==expected,(case,frame,next((i for i,(g,e) in enumerate(zip(got,expected)) if g!=e),None),got[:24],expected[:24]);steps+=p.steps
report={'status':'PASS','cases':len(cases),'callback_frames':cursor,'continuous_frames':1557,'source_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Wholeactual167D0 callback entry/exit, phases255/0..9 andunknown127/128/254 final-stage behavior, savedregs/SP restored, ordered scroll/blend/alpha writes, palette snapshot andpointer switch, scaled/unscaled draw arguments, wrapped boundaries andfade gates, ordered lettering groups/positions andall13letter initialization records. 3x519continuous native/source frames exerciseentireintroexit orholdfadegate; phase8 palette/scale/reveal propagation includingnewrecords in same callback andphase9 input/time boundaries. Whole28288/28658 sprite-renderer andE5A8 fade bodies remaincontracts, DMA palette transfer modeledas memorycopy; native guards revealedcount>13 outside original owned arrays, no fullstartup/liveviewer/pixel equivalence claim.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/startup_animation.c',root/'src/startup_animation.h',root/'tests/startup-animation-rom-compare.c',Path(__file__),root/'bin/startup-animation-rom-compare.exe']}}
(root/'reports/startup-animation-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
