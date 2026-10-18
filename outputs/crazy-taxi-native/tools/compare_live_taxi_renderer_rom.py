"""Compare a real native driving capture with the original taxi renderer."""
exec(open('outputs/crazy-taxi-native/tools/compare_taxi_renderer_original_angle_rom.py').read().split('\ncases=')[0])
import sys
name=sys.argv[1] if len(sys.argv)>1 else 'live-taxi'
f=root/('reports/'+name+'-source-inputs.json');d=json.loads(f.read_text());assert d['alpha']==0
x,y,z,heading,bodyroll=d['vehicle'];cx,cy,cz,pitch,yaw,roll=d['camera'];base=0x2002634
p=TaxiProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[];p.arm.mem[0x2001708]=0;p.arm.mem[0x30053e0]=0
for a in range(base,base+0xea0,4):p.arm.mem[a]=0
for off,val,n in [(0,x,4),(4,y,4),(8,z,4),(0x498,cx,4),(0x49c,cy,4),(0x4a0,cz,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x1c,heading,4),(0x1a,bodyroll,2),(0x770,d['distance']*4096,4),(0x680,d['base'],4)]:p.write(base+off,val,n)
p.arm.mem[0x3006ab0]=0x8000aa8;p.arm.mem[0x3006abc]=0x8000a1c;p.arm.mem[0x3006940]=0x80029fc;p.arm.mem[0x3006810]=0x10002000
for o,v in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+o]=v
p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
got=[p.read(base+0xe64)]+[v for req in p.requests for v in req[:2]]+[p.requests[0][2],p.requests[0][3]]
expected=[d['frame']]+sum(d['centers'],[])+[d['depth'],d['scale']]
report=dict(pass_match=got==expected,original=got,native=expected,capture=d,scope='Actual native alpha0 driving pose and camera compared to original1556C/ARMmatrix/halftransform/angle. Groundheight0, shadow graphics noops, allocator0, division contracts. Does not prove source camera follow or GBA pixel framing.')
files=[f,root/'tools/compare_live_taxi_renderer_rom.py',root/'tools/compare_taxi_renderer_original_angle_rom.py',root/'tools/original_angle_difference_probe.py',root/'src/scene_viewer.c',root/'bin/scene-viewer.exe']
report['sha256']={str(a):hashlib.sha256(a.read_bytes()).hexdigest() for a in files}
(root/('reports/'+name+'-renderer-rom-comparison.json')).write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2));assert got==expected
