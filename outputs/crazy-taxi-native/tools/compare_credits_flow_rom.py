"""Actual credits running-frame2446E..245FA vs native; setup/callees explicit."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
import random
for lo,hi in [(0x2446e,0x244cc),(0x24524,0x245fc)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[lo:hi],0x8000000+lo):allowed_thumb[i.address]=bytes(i.bytes)
class CreditsProbe(UpdateProbe):
 def write(self,a,v,n=4):
  if a in (0x4000012,0x4000014,0x4000016):
   assert n==2;self.scroll[a]=v&65535;return
  super().write(a,v,n)
 def external(self,target):
  if target==0x8028fe4:
   self.text_id=self.reg[0];assert 750<=self.text_id<=853
   base=0x49dfe0;self.reg[0]=0x8000000+base+struct.unpack_from('<I',r,base+4+self.text_id*4)[0]
  elif target==0x801e420:
   assert self.reg[0]==self.reg[13] and self.reg[1]==0x849d778
   off=self.reg[2]-0x8000000;raw=r[off:r.index(0,off)];assert len(raw)==31
   for j,v in enumerate(raw+b'\0'):self.write(self.reg[0]+j,v,1)
  elif target==0x801e784:
   assert self.reg[:3]==[self.reg[13],32,33]
   for j in range(33):self.write(self.reg[0]+j,32,1)
  elif target==0x80267c8:
   assert self.reg[:3]==[struct.unpack_from('<I',r,0x7ec658+29*4)[0],0,0];self.sound29=1;self.reg[0]=0
  elif target==0x800e5a8:
   assert self.reg[:4]==[0,32,0x2401,0x2401];self.fade+=1
  else:raise AssertionError(hex(target))
def original_frame(case):
 y,x,acc,angle,row,line,timer,sound,delta,sine,effects,confirm,start=case
 p=CreditsProbe((0,0,0,0,0,1,0,0,0,0));p.scroll={};p.text_id=-1;p.sound29=p.fade=0
 sp=0x200e000;p.reg[13]=sp;p.reg[7]=timer;p.reg[10]=line
 for off,v in [(0x44,y),(0x4c,x),(0x48,acc),(0x54,angle),(0x40,sound),(0x50,row),(0x58,sp)]:p.write(sp+off,v)
 base=0x200a000;p.write(0x2000000,base);p.write(0x2003b00,delta)
 p.write(0x2007602,effects,1);p.write(0x2003500,confirm,1);p.write(0x2003503,start,1)
 p.write(0x202c4f0+(angle&4095)*4,sine)
 for j in range(32):p.write(base+row*2+j*2,0xaaaa,2)
 p.run(entry=0x802446e,stop=0x80245fc)
 wrote=int(signed(acc+(delta<<7))>2047)
 tiles=[p.read(base+row*2+j*2,2) for j in range(30)] if wrote else [0]*30
 actual=[signed(p.read(sp+0x44)),signed(p.read(sp+0x4c)),signed(p.read(sp+0x48)),p.read(sp+0x54)&4095,p.read(sp+0x50),p.reg[10],p.reg[7],p.read(sp+0x40),p.text_id,p.sound29,p.fade,wrote,p.scroll[0x4000012],p.scroll[0x4000014],p.scroll[0x4000016],*tiles]
 return actual,p.steps
rng=random.Random(243)
cases=[]
for line in (0,1,102,103,104,105):
 for delta in (1,4,60):
  for boundary in (0,1920,2047,2048):
   cases.append((rng.choice((45056,-212864,-213000)),rng.choice((0,65280,-256)),boundary,rng.randrange(4096),rng.choice((0,704,992)),line,rng.randrange(256),rng.randrange(2),delta,rng.choice((0,32767,-32768)),rng.randrange(2),rng.randrange(2),rng.randrange(2)))
cp=subprocess.run([str(root/'bin/credits-flow-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 y,x,acc,angle,row,line,timer,sound,delta,sine,effects,confirm,start=case
 p=CreditsProbe((0,0,0,0,0,1,0,0,0,0));p.scroll={};p.text_id=-1;p.sound29=p.fade=0
 sp=0x200e000;p.reg[13]=sp;p.reg[7]=timer;p.reg[10]=line
 for off,v in [(0x44,y),(0x4c,x),(0x48,acc),(0x54,angle),(0x40,sound),(0x50,row),(0x58,sp)]:p.write(sp+off,v)
 base=0x200a000;p.write(0x2000000,base);p.write(0x2003b00,delta)
 p.write(0x2007602,effects,1);p.write(0x2003500,confirm,1);p.write(0x2003503,start,1)
 p.write(0x202c4f0+(angle&4095)*4,sine)
 for j in range(32):p.write(base+row*2+j*2,0xaaaa,2)
 p.run(entry=0x802446e,stop=0x80245fc)
 wrote=int(signed(acc+(delta<<7))>2047)
 tiles=[p.read(base+row*2+j*2,2) for j in range(30)] if wrote else [0]*30
 actual=[signed(p.read(sp+0x44)),signed(p.read(sp+0x4c)),signed(p.read(sp+0x48)),p.read(sp+0x54)&4095,p.read(sp+0x50),p.reg[10],p.reg[7],p.read(sp+0x40),p.text_id,p.sound29,p.fade,wrote,p.scroll[0x4000012],p.scroll[0x4000014],p.scroll[0x4000016],*tiles]
 assert actual==want,(case,actual,want);steps+=p.steps
# Persistent native comparison inputs come from prior ACTUAL ROM state,
# never from native output or a mirrored Python implementation.
chain=[];expected=[];state=(45056,0,0,0,704,0,0,0)
while True:
 case=(*state,1,0,1,0,0)
 actual,count=original_frame(case);chain.append(case);expected.append(actual);steps+=count
 state=tuple(actual[:8])
 if actual[10]:break
 assert len(chain)<3000
assert len(chain)==2016 and state[5]==126
cp=subprocess.run([str(root/'bin/credits-flow-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in chain),text=True,capture_output=True,check=True)
assert [list(map(int,x.split())) for x in cp.stdout.splitlines()]==expected
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':len(cases),'persistent_frames':len(chain),'thumb_instructions':steps,'equal':True,'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/credits_flow.c','src/credits_flow.h','tests/credits_flow_rom_compare.c','tests/credits_text_fixture.h','tools/compare_credits_flow_rom.py']},'scope':'Actual2446E..245FA running-frame instructions compare scalar locals, lineIDs750..853, palette-prefixed30tile row, scrollregister writes, one-shot29 andfade(0,32,2401,2401). Lookup exact ROM bytes;1E420 formatting and1E784 fill explicit contracts; originaltrig value supplied, audio/fade callees captured. Persistent2016frame delta1/sine0 trajectory reaches original auto-fade and126 rows with104 original strings then22 blanks. Setup243A4, fading gates/frame service/return, registeredfont/pixels and livequalifiedQuit excluded.'}
(root/'reports/credits-flow-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
