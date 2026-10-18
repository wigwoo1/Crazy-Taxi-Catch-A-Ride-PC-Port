# Retired returning-child fixture; retained for research history.
raise SystemExit('INVALIDATED: AC70 is a Retry restart transfer; see retry-transfer-rom-audit.json')
"""Original Retry caller shared-global handoff; AC70 is opaque, not implemented.
Checks original state at qualified credits call and caller completion after
an explicit child boundary changes all modeled shared owners.
"""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
addresses=[0x2001ea8,0x2001697,0x2005f20,0x2001ea6,0x2003897,0x200389a]
def owners(p):return [p.read(a,1) for a in addresses]+[p.read(0x2003878)]
class ChildOwnersProbe(ExitProbe):
 def external(self,target):
  super().external(target)
  if target==0x800ac70:
   frozen,qualified,exit,results,callback=self.child
   for a,v in [(0x2001ea8,57),(0x2001697,3),(0x2005f20,frozen),(0x2001ea6,55),(0x2003897,77),(0x200389a,results),(0x2001691,qualified),(0x2005f1a,exit)]:self.write(a,v,1)
   self.write(0x2003878,callback)
  if target==0x80243a4:self.credits_boundary=owners(self)
cases=list(itertools.product([0,1,128],[0,1,255],[0,1],[0,1,99],[0,0x12345679]))
cp=subprocess.run([str(root/'bin/normal-exit-child-owners-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 p=ChildOwnersProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.child=case
 for a,v in [(0x2001ea8,1),(0x200177b,0),(0x2005f20,1),(0x2001691,0),(0x2005f1a,0),(0x2001697,9),(0x2001ea6,55),(0x2003897,77),(0x200389a,88),(0x2001708,0)]:p.write(a,v,1)
 p.write(0x2003878,123);p.run(entry=0x800b580,stop={0x800af80,0x800b6d0})
 final=owners(p);prepared=p.credits_boundary if case[1] else final
 actual=prepared+final+[len(p.calls)]+p.calls
 assert actual==want,(case,actual,want);steps+=p.steps
names=['src/normal_exit.c','src/normal_exit.h','tests/normal_exit_child_owners_compare.c','tools/compare_normal_exit_child_owners_rom.py']
report={'equal':True,'cases':len(cases),'thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in names},'scope':'Actual original B580..B6CE Retry caller after opaque AC70 boundary changes shared fields. Native explicit return handoff and credits-call/final owners ordered requests exact. Child game, real nested return and helper effects not implemented or verified by this fixture.'}
(root/'reports/normal-exit-child-owners-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
