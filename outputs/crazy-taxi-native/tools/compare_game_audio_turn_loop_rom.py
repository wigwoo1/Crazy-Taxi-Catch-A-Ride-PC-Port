"""Original retained event check and slot6 looping sound owner."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for a,b in [(0x11074,0x11102),(0x11128,0x1114e)]:
 for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[a:b],0x8000000+a):allowed_thumb[ins.address]=bytes(ins.bytes)
class TurnLoopProbe(UpdateProbe):
 def external(self,target):
  h=self.reg[0]&0xffffffff
  if target==0x80267c8:
   assert h==struct.unpack_from('<I',r,0x7ec658+58*4)[0] and self.reg[1:3]==[1,1]
   self.calls.append([1,58,257]);self.reg[0]=self.start;return
  if target==0x80277a4:
   ix=next(i for i in [44,58] if struct.unpack_from('<I',r,0x7ec658+i*4)[0]==self.reg[1])
   self.calls.append([6,h,ix]);self.reg[0]=self.eb if ix==44 else self.lb;return
  assert target in [0x8027610,0x80276cc],hex(target)
  self.calls.append([4 if target==0x8027610 else 2,h,0 if target==0x8027610 else signed(self.reg[1])])
cases=[]
for a,b,angle,enabled,flags,gates,eb,lb,start in itertools.product([0,0x2009000],[0,0x2009020],[-32768,-769,-768,0,768,769,32767],[0,255],[0,8],range(8),[0,255],[0,255],[0,0x2009040]):
 cases.append((a,b,angle,enabled,flags,gates&1,(gates>>1)&1,(gates>>2)&1,eb,lb,start))
cp=subprocess.run([str(root/'bin/game-audio-turn-loop-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for c,want in zip(cases,rows):
 a,b,angle,enabled,flags,freeze,end,fade,eb,lb,start=c;p=TurnLoopProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.eb=eb;p.lb=lb;p.start=start
 for addr,v,n in [(0x20025fc,a,4),(0x2002600,b,4),(0x2002634+0x578,angle,2),(0x20025d4,enabled,1),(0x2002634+0x28,flags,1),(0x2005f20,freeze,1),(0x2001697,end,1),(0x200177b,fade,1)]:p.write(addr,v,n)
 p.run(entry=0x8011074,stop=0x801114e)
 actual=[p.read(0x20025fc),p.read(0x2002600),len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,(c,actual,want);assert p.read(0x20025d4,1)==enabled;steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original11074..1114C retained event44 busy clearing and slot6 loop58 angle/gates/start/volume64/busy/stop state. Audio callees captured, full controller and live mixing excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_turn_loop.c','src/game_audio_turn_loop.h','tests/game_audio_turn_loop_rom_compare.c','tools/compare_game_audio_turn_loop_rom.py']})
(root/'reports/game-audio-turn-loop-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
