"""OriginalBF78..BFCE countdown-driven final owner, helpers explicit."""
exec(open('outputs/crazy-taxi-native/tools/compare_game_audio_levels_rom.py').read().split('\ncases=')[0])
for ins in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0xbf78:0xbfd0],0x800bf78):allowed_thumb[ins.address]=bytes(ins.bytes)
# Separate source caller proves zero r2 atBF78; literalpoints gate, not guessed.
assert r[0xbeba:0xbec0]==bytes.fromhex('0278002a5bd0')
class FinalProbe(UpdateProbe):
 def external(self,target):
  source=struct.unpack_from('<I',r,0x7ec658+70*4)[0]
  if target==0x80267c8:
   assert self.reg[0]==source and self.reg[1:3]==[0,0];self.calls.append([1,70,0]);self.reg[0]=self.start;return
  assert target==0x80277a4 and self.reg[1]==source;self.calls.append([6,self.reg[0],70]);self.reg[0]=self.busy
cases=list(itertools.product([0,0x2009000],[-32768,-1,0,1,60,32767],[1,60,-1,2147483647],[0,0x2009020],[0,255]))
cp=subprocess.run([str(root/'bin/game-audio-final-event-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for c,want in zip(cases,rows):
 h,timer,delta,start,busy=c;p=FinalProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.start=start;p.busy=busy;p.reg[2]=0;p.write(0x200260c,h);p.write(0x20016be,timer,2);p.write(0x2003b00,delta)
 p.run(entry=0x800bf78,stop={0x800bfd0,0x800c524})
 t=p.read(0x20016be,2);t=t if t<32768 else t-65536
 actual=[p.read(0x200260c),t,int(p.stopped_pc==0x800bfd0),len(p.calls)]+[v for call in p.calls for v in call];assert actual==want,(c,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='OriginalBF78..BFCE slot9 timer/start70/busy70/requestorder/continuegate, r2zero sourcecaller. Callees explicit boundaries; later UI/startup/session flow excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/game_audio_final_event.c','src/game_audio_final_event.h','tests/game_audio_final_event_rom_compare.c','tools/compare_game_audio_final_event_rom.py']})
(root/'reports/game-audio-final-event-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
