"""Fresh native126DC through129A6 with actual129E0/12C6C ordinary calls."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_motion_steering.py').read().split('\ncases=')[0])
import sys
root=pathlib.Path('outputs/crazy-taxi-native');label=sys.argv[1] if len(sys.argv)>1 else 'motion-live-left';trace=root/('reports/'+label+'.txt')
records=[list(map(int,l.split())) for l in trace.read_text().splitlines()];assert records and all(len(a)==54 for a in records)
steps=armsteps=0
total_records=len(records);indices=set(round(i*(len(records)-1)/127) for i in range(128));unsupported=[i for i,a in enumerate(records) if a[12]!=0];assert not unsupported,('phase coverage needed',len(unsupported));records=[records[i] for i in sorted(indices)]

for index,a in enumerate(records):
 assert a[12]==0,('phase not covered',index,a[12])
 case=(a[0],a[1],a[2],a[3]//4096,a[4],a[5],a[6],a[7],a[8],a[9]);p=CombinedProbe(case,0,0)
 for off,v,n in [(0x1c,a[3],4),(0x20,a[10],4),(0x358,a[11],1),(0xe6a,a[12],1),(0xe6b,a[13],1),(0x7b8,a[14],4),(0x7b0,a[15],4),(0x7c8,a[16],4),(0xe84,a[17],4),(0x594,a[18],4),(0x540,a[19],4),(0,a[20],4),(4,a[21],4),(8,a[22],4)]:p.write(p.base+off,v,n)
 p.arm.mem[0x2001eb0]=a[23];p.arm.mem[0x2001534]=(a[24]&255)<<24
 for i in range(4):p.arm.mem[0x30053d0+i*4]=sum(a[25+i*4+j]<<(j*8) for j in range(4))
 p.run();got=p.outputs()+[p.read(p.base+0x28,1)];assert got==a[41:],(index,a,got);steps+=p.steps;armsteps+=p.arm.steps
report={'status':'PASS','calls':len(records),'capture_records':total_records,'selected_indices':sorted(indices),'Thumb_instructions':steps,'ARM_instructions':armsteps,'angle_ARM_instructions':angle_probe.steps,'scope':'Actual126DC..129A6 native live before/after states, nested actual129E0 steering and12C6C ordinaryphase0 orientation; actualARM rotations/angle. Captured all16 commandbytes and exact actor/settings/scale/geometry fields; source sine table decoded CTR fixture and signeddivision scalar contract. Exactresponse/headings/displacementXYZ/force/drift/timer/speed/positionXZ/flags. Not later129A8 traction14394/world/audio/fullsource driving trajectory.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [pathlib.Path(__file__),trace,root/'src/motion.c',root/'src/orientation.c',root/'src/scene_viewer.c',root/'bin/scene-viewer-live-movement-capture.exe']}}
(root/('reports/'+label+'-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(records),steps,armsteps)
