from pathlib import Path
import json,itertools,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
for lo,hi in [(0x79d2,0x7a36),(0x7adc,0x7afc)]:
 for i in ns['md'].disasm(ns['r'][lo:hi],0x8000000+lo):ns['allowed_thumb'][i.address]=bytes(i.bytes)
class Probe(ns['InitProbe']):
 def external(self,t):
  assert t in [0x8010aac,0x8027754,0x802771c,0x8027730,0x8024674,0x8026ef8,0x8027110],hex(t)
  if t==0x8026ef8:assert self.reg[0]==int.from_bytes(ns['r'][0x7ec048:0x7ec04c],'little')
  if t==0x8027110:assert self.reg[0]==ns['r'][0x7ec064]
  self.calls.append(t)
cases=list(itertools.product(range(256),[0,1],[0,1,128,255],[0,1,255]))
p=subprocess.run([str(root/'bin/challenge-exit-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True)
rows=p.stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 outer,fade,frozen,success=case;p=Probe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
 p.scalars={0x2001ea8:outer,0x200177b:fade,0x2005f20:frozen,0x2001697:9,0x200153c:success,0x2001ea6:55,0x2003897:77,0x200389a:88,0x2003878:123}
 for a in list(p.scalars):p.write(a,p.scalars[a],4 if a==0x2003878 else 1)
 p.run(entry=0x80079d2,stop={0x8007538,0x800743c,0x8007afc})
 rc=0 if outer not in (1,2) or fade else 2 if outer==1 else 1
 got=[rc]+[p.read(a,1) for a in [0x2001ea8,0x2001697,0x2005f20,0x2001ea6,0x2003897,0x200389a]]+[p.read(0x2003878),len(p.calls)]+p.calls
 want=list(map(int,row.split()));assert got==want,(case,got,want);steps+=p.steps
report={'status':'PASS_ORIGINAL_CHALLENGE_EXIT_CALLER','cases':len(cases),'thumb_instructions':steps,'rom_sha256':hashlib.sha256(ns['r']).hexdigest(),'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/challenge_exit.c','src/challenge_exit.h','tests/challenge_exit_rom_compare.c','tools/compare_challenge_exit_rom.py']},'scope':'Actual79D2..7AFA original challenge parent, all outer byte values, fade gates, frozen signed byte values and failure/success branches. Ordered external no-state requests and menu source arguments; Restart stops at743C, success24674 body/return and audio/menu helper bodies remain contracts. Not full joined gameplay or hardware.'}
(root/'reports/challenge-exit-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
