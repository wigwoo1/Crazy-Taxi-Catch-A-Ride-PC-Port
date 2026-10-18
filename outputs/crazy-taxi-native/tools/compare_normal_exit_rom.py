"""Original ordinary normal-game exit owner; called helpers explicit requests."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_movement_gate_rom.py').read().split('\nvalues=')[0])
import itertools
cfg=json.loads((root/'reports/normal-loop-cfg.json').read_text())['instructions']
for i in cfg:
 a=int(i['address'],16)
 if 0x800b580<=a<=0x800b6ce:allowed_thumb[a]=bytes.fromhex(i['bytes'])
class RetryTransferObserved(Exception):pass
class ExitProbe(UpdateProbe):
 def external(self,target):
  assert target in [0x8010aac,0x8027754,0x802771c,0x8027730,0x800ac70,0x80243a4,0x802392c,0x80253e4,0x8024098,0x8024674],hex(target)
  self.calls.append(target)
  if target==0x800ac70:raise RetryTransferObserved()
cases=list(itertools.product(range(256),[0,1],[0,1],[0,1,128],[0,1]))
cp=subprocess.run([str(root/'bin/normal-exit-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 outer,fade,frozen,qualified,exit=case;p=ExitProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
 for a,v in [(0x2001ea8,outer),(0x200177b,fade),(0x2005f20,frozen),(0x2001691,qualified),(0x2005f1a,exit),(0x2001697,9),(0x2001ea6,55),(0x2003897,77),(0x200389a,88),(0x2001708,0)]:p.write(a,v,1)
 p.write(0x2003878,123)
 try:p.run(entry=0x800b580,stop={0x800af80,0x800b6d0})
 except RetryTransferObserved:pass
 actual=[p.read(a,1) for a in [0x2001ea8,0x2001697,0x2005f20,0x2001ea6,0x2003897,0x200389a]]+[p.read(0x2003878),len(p.calls)]+p.calls
 assert actual==want,(case,actual,want);steps+=p.steps
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':len(cases),'thumb_instructions':steps,'equal':True,'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/normal_exit.c','src/normal_exit.h','tests/normal_exit_rom_compare.c','tools/compare_normal_exit_rom.py']},'scope':'Actual B580..B6CE ordinary mode0 through loop continuation, Quit before epilogue, or Retry AC70 transfer boundary. All outer byte values with fade/freeze/qualification/exit fixtures; ordered requests and shared owner state exact. Retry initialization and subsequent gameplay are not executed; AC70 never treated as an opaque returning child. Called audio/credits/results helpers explicit contracts.'}
(root/'reports/normal-exit-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
