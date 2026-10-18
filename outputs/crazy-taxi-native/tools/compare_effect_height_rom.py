"""Original29B44 effect height query on current ROM world geometry versus native."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_ground_contact_rom.py').read().split('\nschema=')[0])
cfg=json.loads((root/'reports/effect-height-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
schema=json.loads((root/'reports/bound-route-world-full-state-schema.json').read_text())['fields'];points=[]
for l in (root/'reports/bound-route-world-full-states.txt').read_text().splitlines():
 a=list(map(int,l.split()));d=dict(zip(schema,a[1:]));points.append(tuple(d[k] for k in ['query_x','query_y','query_z']))
points += [(-1000000,0,-1000000),(1000000,0,1000000),(0,-2147483648,0)]
res=subprocess.run([str(root/'bin/effect-height-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in points),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(points);steps=0
for point,row in zip(points,rows):
 probe=GroundProbe((0,0,0,0,0,1),0)
 for off,v in zip([0,4,8],point):probe.write(probe.base+off,v)
 probe.arm.mem[0x3006810]=address;probe.arm.mem[0x3006940]=0x80029fc;probe.reg[0]=probe.base;probe.reg[14]=0x10020001;probe.run(entry=0x8029b44,stop=0x10020000);got=signed(probe.reg[0]);assert got==row[1] and row[0]==int(got!=-2147483648),dict(point=point,original=got,native=row);steps+=probe.steps
report=dict(cases=len(points),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal29B44 readsoriginalBoundROMdescriptor/grid/geometry vsnativeworld_asset_height on161currentrampqueries+3outside/extremeYpoints. Originalboundedcontainment anddivisioncontracts; noeffectcallback/fullframe/renderclaim.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/world_asset.c',root/'src/ground.c',root/'tests/effect_height_rom_compare.c',root/'tools/compare_effect_height_rom.py',root/'bin/effect-height-rom-compare.exe',root/'reports/effect-height-cfg.json',root/'reports/bound-route-world-full-states.txt']}
(root/'reports/effect-height-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
