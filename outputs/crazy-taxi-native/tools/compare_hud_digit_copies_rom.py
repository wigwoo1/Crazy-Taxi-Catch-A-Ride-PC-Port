from pathlib import Path
import random,json,hashlib,struct,subprocess
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for start,end in [(0x1f636,0x1f68e),(0x1f712,0x1f762)]:
 for i in md.disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class HudProbe(InitProbe):
 def external(self,target):
  assert target==0x8027fb8;self.requests.append(self.reg[:4]+[self.read(self.reg[13])]);self.reg[0]=0
rng=random.Random(195);cases=[[-1]*19,[0]*19,[9]*19]+[[rng.choice([-1,-2,0,1,5,9,32767]) for _ in range(19)] for i in range(64)]
binary=root/'bin/hud-digit-copies-probe.exe';rows=subprocess.run([str(binary)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for values,row in zip(cases,rows):
 expected=[]
 for start,stop,digits in [(0x801f636,0x801f68e,values[:10]),(0x801f712,0x801f762,values[10:])]:
  p=HudProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];buffer=0x10000800;p.regions={buffer:bytearray(struct.pack('<'+'h'*len(digits),*digits))};p.scalars={};p.reg[5]=p.reg[6]=buffer;p.reg[7]=0;p.reg[9]=0xffffffff;p.reg[4]=0;p.reg[13]=0x10000400;p.run(entry=start,stop=stop);expected.extend(v for request in p.requests for v in request);steps+=p.steps
 assert expected==list(map(int,row.split())),(values,expected,row)
report=dict(status='PASS_NATIVE_ORIGINAL_HUD_DIGIT_COPY_PRODUCER',cases=len(cases),requests=len(cases)*15,original_thumb_instructions=steps,scope='OriginalHUDdigit request loops only: blanks use03005400/index0, glyphs actualROMtable/index16bit, sourceframe/group/order/deferredflag and punctuation skips exact. Scratchallzero1201capturedsamples supportsclearbehavior; no completeHUDformatting/resourcecopy/framepixels orliveintegration proof.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),binary,root/'src/hud_digit_copies.c']});(root/'reports/hud-digit-copy-original-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),'originalHUD digit fixtures',len(cases)*15,'requests')
