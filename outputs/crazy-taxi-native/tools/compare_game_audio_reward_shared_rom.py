"""Original full controller nests original reward and RNG; shared voice timing."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_controller_rom.py').read().split('\nrng=random.Random')[0])
controller_class=ControllerProbe;controller_allowed=allowed_thumb.copy()
exec(open('outputs/crazy-taxi-native/tools/compare_reward_rom.py').read().split('\ncases=[]')[0])
allowed_thumb.update(controller_allowed)
class SharedProbe(controller_class):
 def external(self,target):
  if target==0x802901c:
   assert self.reg[:3]==[1,0,25];self.reward_calls+=1
   self.inside_reward=True;self.run(entry=target,stop=self.reg[14]&~1);self.inside_reward=False;return
  if self.inside_reward and target==0x801e72c:
   self.run(entry=target,stop=self.reg[14]&~1);return
  if self.inside_reward and target==0x80267c8:
   assert self.reg[1:3]==[0,0];return
  if target==0x80267c8:
   pointer=self.reg[0];index=next(j for j in range(203) if struct.unpack_from('<I',r,0x7ec658+j*4)[0]==pointer)
   if 83<=index<131:self.voice=index
  return super().external(target)
cp=subprocess.run([str(root/'bin/game-audio-controller-reward-shared-test.exe')],capture_output=True,text=True,check=True)
rows=[list(map(int,line.split()[1:])) for line in cp.stdout.splitlines() if line.startswith('ROW ')];assert len(rows)==128;steps=0
for seed,rng,voice,sound,selection,combo,whole,fraction in rows:
 p=SharedProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.inside_reward=False;p.reward_calls=0;p.voice=0
 s={k:0 for k in sf};s.update(factor=256,amplitude=16384,pending3=1,context_selection=-1,driver_selection=-1)
 i={k:0 for k in inf};i.update(max_speed=184572,scale=4096,delta=1,vehicle564=1,current_class=0,alternate_class=1)
 for j in range(9):i[f'starts[{j}]']=j+1;i[f'busy[{j}]']=1
 p.inputs=i
 for k,(a,n,_) in smap.items():p.write(a,s[k],n)
 for k,(a,n) in imap.items():p.write(a,i[k],n)
 for a,v,n in [(0x2001688,0x2009500,4),(0x200168c,0x2009520,4),(0x2009501,1,1),(0x2009521,0,1),(0x2001694,1,1),(0x2009aa4,0,1),(0x2009aa6,0,1),(0x2009aa8,0,4),(0x2009aac,0,4),(0x2003b04,seed,4)]:p.write(a,v,n)
 for j in range(560):p.write(0x2009abc+j,0,1)
 p.run(entry=0x8010b54,stop=0x801146a)
 got=[p.read(0x2003b04),p.read(0x20025d6,1),p.voice,p.read(0x20025d6,1),p.read(0x2009aa6,1),signed(p.read(0x2009aa8)),signed(p.read(0x2009aac))]
 assert got==[rng,voice,sound,selection,combo,whole,fraction],(seed,got,rows[seed]);assert p.reward_calls==1 and p.read(0x2009abe,1)==64;steps+=p.steps
report=dict(cases=len(rows),thumb_instructions=steps,equal=True,scope='Original10B54..11468 nests actual2901C and1E72C; same-frame voice sound index/selector, RNG, combo totals and tip age match native shared dispatch. Audio/action/division explicit helper boundaries, no device/full gameplay proof.',rom_sha256=hashlib.sha256(r).hexdigest(),source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_controller.c','src/reward.c','tests/game_audio_controller_reward_shared_test.c','tools/compare_game_audio_reward_shared_rom.py']})
(root/'reports/game-audio-reward-shared-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
