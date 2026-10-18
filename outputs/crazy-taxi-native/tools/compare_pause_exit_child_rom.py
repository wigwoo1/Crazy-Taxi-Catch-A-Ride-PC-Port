# Retired returning-child fixture; retained for research history.
raise SystemExit('INVALIDATED: AC70 is a Retry restart transfer; see retry-transfer-rom-audit.json')
"""Original exit caller with explicit opaque-child shared-global changes."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
class ChildExitProbe(ExitProbe):
 def external(self,target):
  super().external(target)
  if target==0x800ac70:
   self.write(0x2001691,self.child_qualified,1)
   self.write(0x2005f1a,self.child_exit,1)
cases=list(itertools.product([0,1],[0,1],range(256),[0,1]))
cp=subprocess.run([str(root/'bin/pause-exit-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 frozen,initial,child,flag=case;p=ChildExitProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.child_qualified=child;p.child_exit=flag
 for a,v in [(0x2001ea8,1),(0x200177b,0),(0x2005f20,frozen),(0x2001691,initial),(0x2005f1a,0),(0x2001697,9),(0x2001ea6,55),(0x2003897,77),(0x200389a,88),(0x2001708,0)]:p.write(a,v,1)
 p.write(0x2003878,123);p.run(entry=0x800b580,stop={0x800af80,0x800b6d0})
 actual=[p.read(a,1) for a in [0x2001ea8,0x2001697,0x2005f20,0x2001ea6,0x2003897,0x200389a]]+[p.read(0x2003878),len(p.calls)]+p.calls
 assert actual==want,(case,actual,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual B580..B6CE exit caller vs pause_exit coordinator; child opaque boundary changes shared qualification/exit flag. Actual child game, device effects and full caller frame excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/pause_exit.c','tests/pause_exit_rom_compare.c','tools/compare_pause_exit_child_rom.py']})
(root/'reports/pause-exit-child-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
