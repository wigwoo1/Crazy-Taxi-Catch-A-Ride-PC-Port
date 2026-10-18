"""Original1556C on actual owned-renderer snapshot inputs; offline diagnostic."""
import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_taxi_renderer_original_angle_rom.py').read_text().split('\ncases=')[0],ns)
d=json.loads((root/'reports/ordinary-physics-startup.json').read_text());p=ns['TaxiProbe']((0,0,0,0,0,1,0,0,0,0));p.requests=[];base=0x2002634
for a in range(base,base+0xea0,4):p.arm.mem[a]=0
x,y,z,heading,bodyroll=d['vehicle'];cx,cy,cz,pitch,yaw,roll=d['camera']
for o,v,n in [(0,x,4),(4,y,4),(8,z,4),(0x498,cx,4),(0x49c,cy,4),(0x4a0,cz,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x1c,heading,4),(0x1a,bodyroll,2),(0x770,d['distance']*4096,4),(0x680,d['base'],4)]:p.write(base+o,v,n)
p.arm.mem.update({0x2001708:0,0x30053e0:0,0x3006ab0:0x8000aa8,0x3006abc:0x8000a1c,0x3006940:0x80029fc,0x3006810:0x10002000})
for o,v in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+o]=v
p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
got=dict(frame=p.read(base+0xe64),centers=[q[:2] for q in p.requests],depth=p.requests[0][2],scale=p.requests[0][3]);expected={k:d[k] for k in got}
assert got==expected,dict(original=got,native=expected)
report=dict(status='PASS',original=got,thumb_instructions=p.steps,arm_instructions=p.arm.steps,rom_sha256=hashlib.sha256(ns['r']).hexdigest(),scope='Original1556C and ARM projection/half-transform/angle on actual native owned4K startup snapshot. Allocator/shadow/ground/division contracts; verifies frame/centers/depth/scale on captured inputs, not source bootstrap/fullframe pixels or original rear-facing intent.',hashes={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'reports/ordinary-physics-startup.json',root/'bin/scene-viewer-ordinary-physics-capture-check.exe']})
(root/'reports/ordinary-startup-renderer-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
