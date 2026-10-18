"""Fresh native12C6C call input/output comparison, ordinaryphase0 only."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_orientation.py').read().split('\ncases=')[0])
import sys
root=pathlib.Path('outputs/crazy-taxi-native');label=sys.argv[1] if len(sys.argv)>1 else 'orientation-live-left'
binary=pathlib.Path(sys.argv[2]) if len(sys.argv)>2 else root/'bin/scene-viewer-orientation-state-check.exe'
trace=root/('reports/'+label+'.txt');records=[list(map(int,l.split())) for l in trace.read_text().splitlines()];assert records and all(len(a)==26 for a in records)
steps=armsteps=enabled=distinct=0
for index,a in enumerate(records):
 assert a[12]==0,('phase not covered',index,a[12])
 p=OrientationProbe(a[:11]);p.write(p.base+0x358,a[11],1);p.write(p.base+0xe6a,a[12],1);p.arm.mem[0x2001eb0]=a[13];p.arm.mem[0x2001534]=(a[14]&255)<<24
 for off,v in zip([0,4,8],a[15:18]):p.write(p.base+off,v)
 p.run();got=p.outputs();assert got==a[18:],(index,a,got);steps+=p.steps;armsteps+=p.arm.steps;enabled+=int(bool(a[11]&4));distinct+=int(got[0]!=got[1])
report={'status':'PASS','calls':len(records),'orientation_enabled_calls':enabled,'distinct_output_headings':distinct,'nonzero_input_force':sum(a[5]!=0 for a in records),'nonzero_input_drift':sum(a[4]!=0 for a in records),'Thumb_instructions':steps,'ARM_instructions':armsteps,'scope':'Exact fresh native orientation call inputs/outputs vs full original12C6C ordinaryphase0: pitch/body/motion/roll/drift/force/timer/state/speed/flags/delta/settings/submode/position. Original ARM rotations/angle, signeddivision scalar contract. Independent calls from live native driver pipeline, not full sourcecaller or continuous original driving fidelity.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [pathlib.Path(__file__),trace,root/'src/orientation.c',root/'src/scene_viewer.c',binary]}}
(root/('reports/'+label+'-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(records),'calls; enabled',enabled,'distinct',distinct)
