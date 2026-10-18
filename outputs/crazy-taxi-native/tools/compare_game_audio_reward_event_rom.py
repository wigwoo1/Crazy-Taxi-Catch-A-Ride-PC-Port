"""Original pending sound44 plus speed-based reward request."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for a,b in [(0x10fb8,0x10fdc),(0x10fec,0x11020),(0x11034,0x11046),(0x1104c,0x1105e),(0x11064,0x11074)]:
 for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[a:b],0x8000000+a):allowed_thumb[ins.address]=bytes(ins.bytes)
class RewardEventProbe(UpdateProbe):
 def external(self,target):
  if target==0x80267c8:
   assert self.reg[0]==struct.unpack_from('<I',r,0x7ec658+44*4)[0] and self.reg[1:3]==[0,0]
   self.calls.append([1,44,0]);self.reg[0]=self.start;return
  assert target==0x802901c and self.reg[0]==1
  self.calls.append([7,self.reg[1],signed(self.reg[2])])
cases=list(itertools.product([0,0x2009000],[0,255],[0,1,255],[-1,0,68280,68281,102420,102421,136560,136561,2147483647],[0,1,-32768],[0,0x2009020]))
cp=subprocess.run([str(root/'bin/game-audio-reward-event-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for c,want in zip(cases,rows):
 h,pending,cooldown,speed,gate,start=c;p=RewardEventProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.start=start
 for a,v,n in [(0x20025fc,h,4),(0x20025d3,pending,1),(0x20025d5,cooldown,1),(0x2002634+0x544,speed,4),(0x20000e4,gate,2)]:p.write(a,v,n)
 p.run(entry=0x8010fb8,stop=0x8011074)
 actual=[p.read(0x20025fc),p.read(0x20025d3,1),p.read(0x20025d5,1),len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,(c,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original10FB8..11072 slot5/pending/cooldown, sound44 start and speed thresholds reward2901C ordered requests; audio/reward callees explicit boundaries, no audible mix or earned reward integration claim.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_reward_event.c','src/game_audio_reward_event.h','tests/game_audio_reward_event_rom_compare.c','tools/compare_game_audio_reward_event_rom.py']})
(root/'reports/game-audio-reward-event-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
