"""Original1114E..1123A driver voice owner, explicit sound boundaries."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for a,b in [(0x1114e,0x11186),(0x111a4,0x111ee),(0x11204,0x1123c)]:
 for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[a:b],0x8000000+a):allowed_thumb[ins.address]=bytes(ins.bytes)
class VoiceProbe(UpdateProbe):
 def external(self,target):
  source=struct.unpack_from('<I',r,0x7ec658+self.index*4)[0]
  if target==0x80267c8:
   assert self.reg[0]==source and self.reg[1:3]==[0,0];self.calls.append([1,self.index,0]);self.reg[0]=self.start;return
  assert target==0x80277a4 and self.reg[1]==source
  self.calls.append([6,self.reg[0]&0xffffffff,self.index]);self.reg[0]=self.busy
cases=[]
for h,other,sel,settings,driver,gates,busy,start in itertools.product([0,0x2009000],[0,0x2009020],[-128,-1,0,17],[0,128,160],[0,3],range(8),[0,255],[0,0x2009040]):
 cases.append((h,other,sel,settings,driver,(driver+1)%4,gates&1,(gates>>1)&1,(gates>>2)&1,busy,start))
cp=subprocess.run([str(root/'bin/game-audio-voice-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for c,want in zip(cases,rows):
 h,other,sel,settings,driver,cd,intro,end,freeze,busy,start=c;p=VoiceProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.busy=busy;p.start=start;p.index=(cd if settings&224==128 else driver)*18+sel+131
 for a,v,n in [(0x2002608,h,4),(0x2002604,other,4),(0x20025d7,sel,1),(0x2001eb0,settings,1),(0x2001690,driver,1),(0x2001539,cd,1),(0x2001696,intro,1),(0x2001697,end,1),(0x2005f20,freeze,1)]:p.write(a,v,n)
 p.run(entry=0x801114e,stop={0x801123c,0x801144e})
 s=p.read(0x20025d7,1);s=s if s<128 else s-256
 actual=[p.read(0x2002608),s,len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,(c,actual,want);assert p.read(0x2002604)==other;steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original driver voice selection/owner2608/start/busy and signed selection clear. Sound pointer matches original table index, normal/challenge driver contexts and gated paths. Sound callees captured; full controller and audible voice excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_voice.c','src/game_audio_voice.h','tests/game_audio_voice_rom_compare.c','tools/compare_game_audio_voice_rom.py']})
(root/'reports/game-audio-voice-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
