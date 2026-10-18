import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');chain={};rn={}
source=(root/'tools/compare_startup_source_transition_persistent_rom.py').read_text().split('\nreport=dict(')[0]
needle=' for o in range(0xea0):p.write(p.base+o,c.read(cb+o,1),1)'
assert source.count(needle)==1
source=source.replace(needle,needle+'\n if tick==406:render_actor=bytes(p.read(p.base+o,1) for o in range(0xea0))')
exec(source,chain)
actor=chain['render_actor'];assert len(actor)==0xea0
exec((root/'tools/compare_taxi_renderer_original_angle_rom.py').read_text().split('\ncases=')[0],rn)
p=rn['TaxiProbe']((0,0,0,0,0,1,0,0,0,0));p.requests=[];base=0x2002634
for o,v in enumerate(actor):p.write(base+o,v,1)
d=json.loads((root/'reports/playable-render-source-inputs.json').read_text());assert d['alpha']==0
x,y,z,h,bodyroll=d['vehicle'];cx,cy,cz,pitch,yaw,roll=d['camera']
for o,v,n in [(0,x,4),(4,y,4),(8,z,4),(0x1c,h,4),(0x1a,bodyroll,2),(0x498,cx,4),(0x49c,cy,4),(0x4a0,cz,4),(0x4a4,pitch,2),(0x4a6,yaw,2),(0x4a8,roll,2),(0x680,d['base'],4),(0x770,d['distance']*4096,4)]:
 mask=4095 if o==0x1a else ((1<<(8*n))-1)
 assert p.read(base+o,n)&mask==v&mask,('source actor/render capture',hex(o),p.read(base+o,n),v)
p.arm.mem.update({0x2001708:0,0x30053e0:0,0x3006ab0:0x8000aa8,0x3006abc:0x8000a1c,0x3006940:0x80029fc,0x3006810:0x10002000})
for o,v in [(0x18,120),(0x1c,80),(0x28,192)]:p.arm.mem[0x3006810+o]=v
p.reg[:3]=[base,0,0];p.reg[14]=0x10020001;p.run(entry=0x801556c,stop=0x10020000)
got=dict(frame=p.read(base+0xe64),centers=[q[:2] for q in p.requests],depth=p.requests[0][2],scale=p.requests[0][3]);assert got=={k:d[k] for k in got},got
report=dict(status='PASS',source_actor_frame=406,original=got,scope='Original B6EC/14A58 initializer and persistent gear/driver/world/camera chain produces allactorbytes for alpha0 display endpoint406; original1556C projects directly from source-produced actor, no native pose reseed. Captured controls/freeze and original arithmetic/ground-shadow/allocator contracts; fullviewport120/80/focal192fixture; not original framebuffer or rear-facing intent proof.')
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'tools/compare_startup_source_transition_persistent_rom.py',root/'reports/playable-render-source-inputs.json',root/'bin/scene-viewer.exe']}
(root/'reports/source-chain-renderer-rom-comparison.json').write_text(json.dumps(report,indent=2));print(report)


