"""Ordinary phase0 original12C6C body/movement heading relationship."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_orientation.py').read().split('\ncases=')[0])
root=pathlib.Path('outputs/crazy-taxi-native')
class OrdinaryProbe(OrientationProbe):
 def __init__(self,case):
  super().__init__(case[:11]);self.write(self.base+0xe6a,0,1);self.write(self.base+0x358,case[11],1);self.arm.mem[0x2001eb0]=0;self.arm.mem[0x2001534]=0
cases=[(0,h,-3,h-196608,d,12345,t,state,122309,flags,delta,actor_flags) for h in [-4096,3073*4096] for d in [-500000,0,500000] for t in [0,4,64,128] for state in [0,1] for flags in [0,8] for delta in [1,2] for actor_flags in [0,4,5]]
rows=subprocess.run([str(root/'bin/ordinary-orientation-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=armsteps=0;distinct=0
for case,row in zip(cases,rows):
 p=OrdinaryProbe(case);p.run();got=p.outputs();assert got==list(map(int,row.split())),(case,got,row);steps+=p.steps;armsteps+=p.arm.steps;distinct+=int(got[0]!=got[1])
report={'status':'PASS','cases':len(cases),'distinct_output_headings':distinct,'Thumb_instructions':steps,'ARM_instructions':armsteps,'angle_ARM_instructions':angle_probe.steps,'scope':'Whole original12C6C ordinaryphase0/settings0/submode0 vs native orientation_step, flags358 bit2 enabled/disabled, initialbody/motion headinggap, drift/timer/state/flag28/delta. Original ARM angle/rotation, signeddivision scalar contract. Exact heading1C/20/drift/timer/speed/positionXZ/no reward request. No original caller/transient actor flags or live trajectories proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [pathlib.Path(__file__),root/'src/orientation.c',root/'src/impact.c',root/'tests/ordinary_orientation_rom_compare.c',root/'bin/ordinary-orientation-rom-compare.exe']}}
(root/'reports/ordinary-orientation-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps,armsteps,'distinct',distinct)
