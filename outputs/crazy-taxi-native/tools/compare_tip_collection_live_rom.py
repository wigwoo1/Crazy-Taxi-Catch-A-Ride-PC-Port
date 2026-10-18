"""Actual native delivery collection states vs complete original2939C."""
exec(open('outputs/crazy-taxi-native/tools/compare_tip_collection_rom.py').read().split('\nrows=')[0])
import sys
files=sys.argv[1:] or ['tip-collection-native.txt','tip-collection-traffic.txt'];steps=cases=active=0
for name in files:
 rows=[list(map(int,l.split())) for l in (root/'reports'/name).read_text().splitlines()];assert rows,name
 for row in rows:
  assert len(row)==1162,(name,len(row));pre,expected=row[:581],row[581:];p=CollectProbe((0,0,0,0,0,1,0,0,0,0))
  for a,v,n in [(0x20016a0,pre[0],4),(0x20016a4,pre[1],4),(0x2009aa7,pre[2],1)]:p.write(a,v,n)
  for i in range(9):p.write(0x2005374+2*i,pre[3+i],2);p.write(0x2005360+2*i,pre[12+i],2)
  for i,v in enumerate(pre[21:]):p.write(0x2009abc+i,v,1)
  p.run(entry=0x802939c,stop=0x10020000)
  actual=[signed(p.read(0x20016a0)),signed(p.read(0x20016a4)),p.read(0x2009aa7,1)]+[p.read(a+2*i,2) for a in [0x2005374,0x2005360] for i in range(9)]+[p.read(0x2009abc+i,1) for i in range(560)]
  assert actual==expected,dict(file=name,mismatches=[(i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b][:10]);steps+=p.steps;cases+=1;active+=sum(bool(pre[21+i*56+2]) for i in range(10))
report=dict(cases=cases,collected_active_records=active,thumb_instructions=steps,match=True,scope='Completeoriginal2939C throughreturn nestedC540mode2/10830 vsactualnative normaldelivery collection pre/post buffers from the listed input captures. Pendingcash/count/18glyphhalfwords/560tipbytes exact. Sourcefullframe/renderpixels notproved.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['src/fare.c','src/fare_session.c','src/passenger_scene.c','src/scene_viewer.c','bin/scene-viewer.exe','tools/compare_tip_collection_live_rom.py']+['reports/'+f for f in files]}
(root/('reports/delivery-after-traffic-collection-rom-comparison.json' if sys.argv[1:] else 'reports/tip-collection-live-rom-comparison.json')).write_text(json.dumps(report,indent=2));print('PASS',report)
