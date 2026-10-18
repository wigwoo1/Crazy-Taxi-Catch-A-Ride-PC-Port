"""Original10D5C..10E1A pending events and action2 hold timer."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for start,end in [(0x10d5c,0x10de0),(0x10e04,0x10e1c)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class EventProbe(UpdateProbe):
 def external(self,target):
  if target==0x8015d98:
   assert self.reg[0]==2;self.queries+=1;self.reg[0]=self.action;return
  assert target==0x80267c8,hex(target)
  assert self.reg[1:3]==[0,0]
  self.sounds.append(next(i for i in [24,25] if struct.unpack_from('<I',r,0x7ec658+i*4)[0]==self.reg[0]))
  self.reg[0]=0x2009000
cases=[]
for a,b,timer,speed,delta,effects,action,gates in itertools.product([0,1],[0,255],[0,1,60],[-1,0,64,65],[1,60],[0,1],[0,255],range(8)):
 cases.append((a,b,timer,speed,delta,effects,action,gates&1,(gates>>1)&1,(gates>>2)&1))
cases.extend((1,1,timer,65,delta,1,255,0,0,0) for timer,delta in itertools.product([-2147483648,2147483647,-1],[1,60]))
cp=subprocess.run([str(root/'bin/game-audio-events-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 a,b,timer,speed,delta,effects,action,intro,ended,frozen=case;p=EventProbe((0,0,0,0,0,1,0,0,0,0));p.queries=0;p.sounds=[];p.action=action;p.reg[9]=speed&0xffffffff
 for address,value,n in [(0x20025d0,a,1),(0x20025d1,b,1),(0x20025e4,timer,4),(0x2003b00,delta,4),(0x2007602,effects,1),(0x2001696,intro,1),(0x2001697,ended,1),(0x2005f20,frozen,1)]:p.write(address,value,n)
 p.run(entry=0x8010d5c,stop=0x8010e1c)
 actual=[p.read(0x20025d0,1),p.read(0x20025d1,1),signed(p.read(0x20025e4)),p.queries,len(p.sounds),*p.sounds]
 assert actual==want,(case,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original pending sound events25/24 and hold timer through10E1A; action query2 and sound starts explicit captured boundaries; full controller/device mixing excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_events.c','src/game_audio_events.h','tests/game_audio_events_rom_compare.c','tools/compare_game_audio_events_rom.py']})
(root/'reports/game-audio-events-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
