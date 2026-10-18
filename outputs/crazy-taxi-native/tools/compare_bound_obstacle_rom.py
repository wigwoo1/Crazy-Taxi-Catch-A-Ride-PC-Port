"""Original obstacle selection reads original Bound ROM geometry."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_ground_contact_rom.py').read().split('contactcfg=')[0])
cfg=json.loads((root/'reports/challenge-bound-obstacle-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
schema=json.loads((root/'reports/bound-launch-world-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-world-states.txt').read_text().splitlines()];cases=[]
for row in rows:
 d=dict(zip(schema,row[1:]));cases.append(tuple(d['query_'+a] for a in ['x','y','z']))
vertices=int.from_bytes(r[address-0x8000000+0x24:address-0x8000000+0x28],'little');polygons=int.from_bytes(r[address-0x8000000+0x28:address-0x8000000+0x2c],'little')
asset=(root/'assets/challenge-worlds/challenge5.ctw').read_bytes()
# Query the centroid of every source polygon, using source vertex coordinates.
for k in range(737):
 off=polygons-0x8000000+k*24;ids=[int.from_bytes(r[off+j:off+j+2],'little') for j in [8,10,12,14]];ids=[i for i in ids if i!=65535]
 pts=[tuple(int.from_bytes(r[vertices-0x8000000+i*16+j:vertices-0x8000000+i*16+j+4],'little',signed=True) for j in [0,4,8]) for i in ids]
 cases.append(tuple(sum(pt[j] for pt in pts)//len(pts) for j in range(3)))
res=subprocess.run([str(root/'bin/obstacle-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);native=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(native)==len(cases);steps=0;kinds={0:0,1:0,2:0}
for case,row in zip(cases,native):
 probe=GroundProbe((0,0,0,0,0,1),0);point=probe.base;out=point+0x100
 for off,v in zip([0,4,8],case):probe.write(point+off,v)
 probe.arm.mem[0x3006810]=address;probe.arm.mem[0x3006940]=0x80029fc;probe.reg[:2]=[point,out];probe.reg[14]=0x10020001;probe.run(entry=0x8029e84,stop=0x10020000)
 height=signed(probe.reg[0]);ptr=probe.read(out);kind=2 if height==2147483647 else (0 if height==-2147483648 else 1)
 got=[kind,height,(ptr-polygons)//24 if kind==2 else 65535];assert got==row,dict(point=case,original=got,native=row);steps+=probe.steps;kinds[kind]+=1
report=dict(cases=len(cases),actual_launch_queries=161,polygon_centroids=737,thumb_instructions=steps,kinds=kinds,containment_entry_v_counts=entry_v_counts,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original29E84 originalROMdescriptor/grid/geometry; height/blockerpolygon compared to native CTW query. Explicit division contract and bounded original containment with callee-save abstraction. Not fullmovement/impact/independent GBA trajectory.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/compare_bound_obstacle_rom.py',root/'tests/obstacle_rom_compare.c',root/'src/ground.c',root/'src/world_asset.c',root/'bin/obstacle-rom-compare.exe',root/'assets/challenge-worlds/challenge5.ctw',root/'reports/challenge-bound-obstacle-cfg.json']}
(root/'reports/bound-obstacle-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
