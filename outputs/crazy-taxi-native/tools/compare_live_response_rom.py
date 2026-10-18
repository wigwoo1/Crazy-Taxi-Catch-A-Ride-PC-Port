"""Offline original15BB8 comparison with live native response captures."""
exec(open('outputs/crazy-taxi-native/tools/compare_response_rom.py').read().split('\nimport random')[0])
import sys
root=pathlib.Path('outputs/crazy-taxi-native');label=sys.argv[1];trace=root/('reports/'+label+'.txt');rows=[list(map(int,l.split())) for l in trace.read_text().splitlines()];assert rows and all(len(a)==5 for a in rows);steps=0
for i,a in enumerate(rows):
 p=MotionProbe((0,0,0,0,0,1),0);p.reg[14]=0x10020001
 for off,value,n in [(0x540,a[0],4),(0x744,a[1],4),(0x4ac,a[2],1)]:p.write(p.base+off,value,n)
 p.run(entry=0x8015bb8,stop=0x10020000);got=[signed(p.read(p.base+x)) for x in [0x540,0x744]];assert got==a[3:],(i,a,got);steps+=p.steps
report={'status':'PASS','calls':len(rows),'Thumb_instructions':steps,'scope':'Complete original15BB8 body response against exact native live inputs/outputs. No external contracts in this routine. Does not prove full vehicle caller/world/render trajectory.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [pathlib.Path(__file__),trace,root/'src/vehicle_math.c',root/'src/scene_viewer.c',root/'bin/scene-viewer-response-state-check.exe']}}
(root/('reports/'+label+'-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(rows),steps)
