"""Offline original taxi selection on an actual native capture, not full bootstrap proof."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');env={}
exec((root/'tools/compare_taxi_renderer_original_angle_rom.py').read_text().split('\ncases=')[0],env)
d=json.loads((root/'reports/current-facing-chain.json').read_text())
p=env['TaxiProbe']((0,0,0,0,0,1,0,0,0,0));p.requests=[];base=0x2002634
for a in range(base,base+0xea0,4):p.arm.mem[a]=0
for n,v in enumerate(d['vehicle'][:3]):p.write(base+n*4,v,4)
for n,v in enumerate(d['camera'][:3]):p.write(base+0x498+n*4,v,4)
for n,v in enumerate(d['camera'][3:]):p.write(base+0x4a4+n*2,v,2)
for off,v,size in [(0x1c,d['vehicle'][3],4),(0x1a,d['vehicle'][4],2),(0x770,d['distance']*4096,4),(0x680,d['base'],4)]:p.write(base+off,v,size)
p.arm.mem.update({0x2001708:0,0x30053e0:0,0x3006ab0:0x8000aa8,0x3006abc:0x8000a1c,0x3006940:0x80029fc,0x3006810:0x10002000})
for off,v in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+off]=v
p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
got=dict(frame=p.read(base+0xe64),centers=[q[:2] for q in p.requests],depth=p.requests[0][2],scale=p.requests[0][3])
expected={k:d[k] for k in got}
assert got==expected,(got,expected)
report=dict(status='PASS',original=got,native=expected,scope='Original1556C and ARM camera transform on actual native actor/camera inputs; source frame/centres/depth/scale exact. Explicit allocator/division/ground-shadow contracts. Does not prove original startup inputs, full game frame, rear-view intent or source pixels.',capture_sha256=hashlib.sha256((root/'reports/current-facing-chain.json').read_bytes()).hexdigest())
(root/'reports/current-facing-renderer-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS original selection versus current native:',got)
