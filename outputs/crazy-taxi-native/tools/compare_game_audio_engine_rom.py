"""Original10C58..10D5A engine owners; audio callees captured requests."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for start,end in [(0x10c58,0x10c94),(0x10cbc,0x10d24),(0x10d30,0x10d5c)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class EngineProbe(LevelsProbe):
 def external(self,target):
  if target==0x8032488:return super().external(target)
  handle=self.reg[0]&0xffffffff
  if target==0x80267c8:
   index=next(i for i in [9,10] if struct.unpack_from('<I',r,0x7ec658+i*4)[0]==handle)
   assert self.reg[1:3]==[1,1]
   self.calls.append([1,index,257]);self.reg[0]=self.starts[index-9];return
  assert target in [0x8027610,0x80276cc,0x80276dc],hex(target)
  op={0x8027610:4,0x80276cc:2,0x80276dc:3}[target]
  self.calls.append([op,handle,0 if op==4 else signed(self.reg[1])])
cases=[]
for low,high,amplitude,a,b,failed in itertools.product([0,1,48,-1],[0,1,48,-1],[0,16384,2147483647],[0,0x2009000],[0,0x2009020],[0,1]):
 cases.append((a,b,low,high,amplitude,10512,0 if failed else 0x2009040,0 if failed else 0x2009060))
cp=subprocess.run([str(root/'bin/game-audio-engine-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 a,b,low,high,amp,pitch,sa,sb=case;p=EngineProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.starts=[sa,sb]
 p.write(0x20025e8,a);p.write(0x20025ec,b);p.write(0x20025dc,amp)
 p.reg[5]=low&0xffffffff;p.reg[7]=high&0xffffffff;p.reg[8]=pitch&0xffffffff
 p.run(entry=0x8010c58,stop=0x8010d5c)
 actual=[p.read(0x20025e8),p.read(0x20025ec),len(p.calls)]+[v for call in p.calls for v in call]
 assert actual==want,(case,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original engine owners10C58..10D5A: two retained handles and ordered start/protected flags/volume/pitch/stop requests, zero start results and wrapped amplitudes. Audio callees explicit boundaries, decoder/live mixing/remaining controller excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_engine.c','src/game_audio_engine.h','tests/game_audio_engine_rom_compare.c','tools/compare_game_audio_engine_rom.py']})
(root/'reports/game-audio-engine-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
