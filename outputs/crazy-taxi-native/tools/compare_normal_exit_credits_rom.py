"""Original B5E6 credits call boundary and post-return ordinary exit owners.
Credits itself is an explicit opaque callee; its native runtime has separate
ROM qualification. This verifies suspension ordering, not an earned Quit.
"""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
addresses=[0x2001ea8,0x2001697,0x2005f20,0x2001ea6,0x2003897,0x200389a]
def owners(p):return [p.read(a,1) for a in addresses]+[p.read(0x2003878)]
class CreditsExitProbe(ExitProbe):
 def external(self,target):
  if target==0x80243a4:
   assert not hasattr(self,'credits_boundary')
   self.credits_boundary=owners(self)
  super().external(target)
cases=list(itertools.product([0,1],range(1,256),[0,1]))
cp=subprocess.run([str(root/'bin/normal-exit-credits-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 frozen,qualified,exit=case;p=CreditsExitProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
 for a,v in [(0x2001ea8,2),(0x200177b,0),(0x2005f20,frozen),(0x2001691,qualified),(0x2005f1a,exit),(0x2001697,9),(0x2001ea6,55),(0x2003897,77),(0x200389a,88),(0x2001708,0)]:p.write(a,v,1)
 p.write(0x2003878,123);p.run(entry=0x800b580,stop={0x800af80,0x800b6d0})
 actual=p.credits_boundary+owners(p)+[len(p.calls)]+p.calls
 assert actual==want,(case,actual,want);steps+=p.steps
names=['src/normal_exit.c','src/normal_exit.h','tests/normal_exit_test.c','tests/normal_exit_credits_compare.c','tools/compare_normal_exit_credits_rom.py']
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':len(cases),'thumb_instructions':steps,'equal':True,'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in names},'scope':'Actual original ordinary Quit owner state at credits BL243A4 and after opaque callee return. All nonzero qualification bytes and freeze/exit gates. Native prepare retains callback/frozen/results while waiting; 2100-frame unchanged wait tested separately. Credits behavior, earned qualification, live viewer routing and result/name helper effects excluded.'}
(root/'reports/normal-exit-credits-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
