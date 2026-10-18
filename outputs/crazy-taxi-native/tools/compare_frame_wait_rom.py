"""Actual whole16208 wait, controlled native event acknowledgements."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for address in range(0x8016208,0x8016244,2):
    i=next(md.disasm(r[address-0x8000000:address-0x8000000+4],address,count=1));allowed_thumb[address]=bytes(i.bytes)
class WaitProbe(InitProbe):
 def read(self,a,size=4):
  if a==0x2003528 and self.waiting:
   self.polls+=1;self.left-=1
   if not self.left:self.waiting=False;self.events+=1;self.scalars[a]=1
  return super().read(a,size)
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x2003528:self.waiting=True;self.left=self.delay;self.clear=v&65535
cases=[(n,e,h,d) for n in list(range(256))+[256,257,261,0xffffffff] for e in [0,1,2,65535,65537] for h,d in [(0,1),(1,2),(0xffff,4),(0xa5a5,7)]]
rows=subprocess.run([str(root/'bin/frame-wait-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines()
assert len(rows)==len(cases);steps=events=polls=0
for case,line in zip(cases,rows):
 n,e,h,d=case;p=WaitProbe((0,0,0,0,0,1,0,0,0,0));p.regions={};p.scalars={0x4000208:e&65535,0x2003528:h};p.waiting=False;p.delay=d;p.left=p.events=p.polls=p.clear=0
 p.reg[0]=n;p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:7];p.run(entry=0x8016208,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:7]==saved
 got=[p.read(0x2003528,2),p.events,p.polls,p.clear]
 assert got==list(map(int,line.split())),(case,got,line)
 steps+=p.steps;events+=p.events;polls+=p.polls
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'acknowledged_events':events,'controlled_event_polls':polls,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'WHOLE16208 entry/return against nativeframe_wait: all256 countbytes/highword truncation, enabled16 exactly1 gate, stale handshake clear preserving otherbits, delayed fresh acknowledgements, savedstack/registers. Offlinecontrolled event source is not actualIRQ/timer or live frame/audio service composition.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/frame_wait.c',root/'src/frame_wait.h',root/'tests/frame-wait-rom-compare.c',Path(__file__),root/'bin/frame-wait-rom-compare.exe']}}
(root/'reports/frame-wait-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
