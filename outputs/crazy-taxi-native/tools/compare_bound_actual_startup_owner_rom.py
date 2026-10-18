"""Whole original14A58 compared to actual native startup owner, offline only."""
import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_vehicle_start_scene_original_markers_rom.py').read_text().split('steps=0;total=0;points=[]')[0],ns)
rows=(root/'reports/bound-whole-startup-preview-owner.txt').read_text().splitlines();assert rows
for index,line in enumerate(rows):
 parts=line.split();assert len(parts)==14
 settings,demo,x,z,heading,rng,gear,sprite,rc=map(int,parts[:9]);before=bytes.fromhex(parts[9]);expected=bytes.fromhex(parts[10]);assert len(before)==len(expected)==0xea0
 p=ns['SceneProbe']((0,0,0,0,0,1,0,0,0,0));p.city_address=0x087ebd98;p.point=[x,z,int.from_bytes(expected[4:8],'little',signed=True)-64,heading,expected[0x374],expected[0x375],expected[0x376],rc];a=0x02002634;p.regions[a]=bytearray(before);p.regions[0x020034d4]=bytearray(8)
 p.scalars.update({0x03006810:p.city_address,0x030053e0:sprite&0xffffffff,0x02002630:gear&0xffffffff});p.write(0x020034d4,1);p.write(0x020034d8,1);p.write(0x02001eb0,settings,1);p.write(0x02001708,demo,1)
 state=rng or 0x4652414e;state=(state<<1)&0xffffffff
 if state&0x80000000:state^=0x1d872b41
 state=((state&0x7fffffff)^0x06000000)&0x7fffffff;p.factor=20+state%41
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x08014a58,stop=0x10020000)
 actual=bytes(p.regions[a]);assert actual==expected,(index,[(hex(i),v,w) for i,(v,w) in enumerate(zip(actual,expected)) if v!=w][:15])
 assert [state,p.scalars[0x02002630],ns['signed'](p.scalars[0x030053e0])]==list(map(int,parts[11:])),index
report=dict(status='PASS',calls=len(rows),scope='Actual native pre/post3744byte initialization owner vs whole original14A58, real ROM Bound marker/ground geometry; initial owner/global/RNG captured, string/RNG/arithmetic/containment contracts; not entire cold bootstrap or countdown trajectory.',hashes={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'reports/bound-whole-startup-preview-owner.txt',root/'bin/scene-viewer-bound-whole-startup-capture-v2.exe',root/'tests/ordinary_capture_vehicle_start.c']})
(root/'reports/bound-whole-startup-owner-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS actual native initialization',len(rows),'calls')
