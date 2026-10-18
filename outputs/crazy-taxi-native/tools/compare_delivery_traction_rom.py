"""Actual14394 traction calls against fresh native before/after captures."""
exec(open('outputs/crazy-taxi-native/tools/compare_traction_rom.py').read().split('\ncases=')[0])
import sys
root=pathlib.Path('outputs/crazy-taxi-native');label=sys.argv[1] if len(sys.argv)>1 else 'traction-live-left';trace=root/('reports/'+label+'.txt');records=[list(map(int,l.split())) for l in trace.read_text().splitlines()];assert records and all(len(a)==23 for a in records);steps=slips=0
total_records=len(records);indices=set(round(i*(len(records)-1)/63) for i in range(64));indices.update(i for i,a in enumerate(records) if a[7]&8);records=[records[i] for i in sorted(indices)]

for index,a in enumerate(records):
 p=TractionProbe(tuple(a[:8])+tuple([a[10],a[13]]));p.write(p.base+0x564,a[8]);p.arm.mem[0x2001eb0]=a[9];p.arm.mem[0x2001694]=(a[10]&255)<<16|(a[11]&255)<<24;p.arm.mem[0x20025d4]=a[12];p.arm.mem[0x2002630]=a[14]
 p.run(entry=0x8014394,stop=0x10020000);got=p.outputs();assert got==a[15:],(index,a,got);steps+=p.steps;slips+=int(got[0]==0)
report={'status':'PASS','calls':len(records),'capture_records':total_records,'selected_indices':sorted(indices),'slip_calls':slips,'Thumb_instructions':steps,'scope':'Actualwhole14394 and2E00C vector-angle vs fresh native traction call exactstate/context/score outputs. Source sqrt and signeddivision explicitcontracts, CTR trig globals fixture. Does notprove precedingforce/capacity caller, full126DC/world/frame/rewardplayback.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [pathlib.Path(__file__),trace,root/'src/traction.c',root/'src/scene_viewer.c',root/'bin/scene-viewer-live-movement-capture.exe']}}
(root/('reports/'+label+'-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(records),steps,'slips',slips)
