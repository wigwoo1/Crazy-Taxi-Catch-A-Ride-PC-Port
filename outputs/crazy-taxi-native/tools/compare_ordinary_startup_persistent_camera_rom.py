"""Original ordinary startup driver/world persistent actor-byte chain; offline only."""
import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native')
dn={};wn={};cn={}
exec((root/"tools/probe_full_camera_view_rom.py").read_text().split("\np=CameraProbe")[0],cn)
dsource=(root/'tools/compare_live_ordinary_driver_rom.py').read_text();wsource=(root/'tools/compare_live_ordinary_world_rom.py').read_text()
exec(dsource.split('import sys')[0],dn)
exec(wsource.split('import sys')[0],wn)
exec(wsource.split('fields={')[1].split('steps=armsteps=0')[0].join(['fields={','']),wn)
ds=json.loads((root/'reports/bound-corrected-full-driver-state-schema.json').read_text())['fields'];ws=json.loads((root/'reports/ordinary-full-world-state-schema.json').read_text())['fields']
drows=[dict(zip(ds,map(int,l.split()[1:]))) for l in (root/'reports/ordinary-physics-startup-driver.txt').read_text().splitlines()];wrows=[dict(zip(ws,map(int,l.split()[1:]))) for l in (root/'reports/ordinary-physics-startup-world.txt').read_text().splitlines()];wrows=wrows[-len(drows):];assert len(drows)==60 and len(wrows)==60
p=dn['DriverProbe']((0,0,0,0,0,1,0,0,0,0));p.base=0x2002634
for a in range(p.base,p.base+0xea0,4):p.arm.mem[a]=0
old=p.arm.write
def aw(a,v):
 if p.base<=a<p.base+0xea0:p.arm.mem[a]=v&0xffffffff
 else:old(a,v)
p.arm.write=aw
w=wn['WorldProbe']((0,0,0,0,0,0,0,1));checks=0
for a in [0x200153c,0x2001540,0x2001544,0x2001548,0x200154c,0x2001694]:w.arm.mem[a]=0
for k,(o,n) in dn['power_fields'].items():p.write(p.base+o,drows[0]['input_power_'+k],n)
for k,(o,n) in dn['motion_fields'].items():p.write(p.base+o,drows[0]['input_motion_'+k],n)
p.write(p.base+0x57a,drows[0]['input_steering_rate_57a'],2)
covered=set()
for mapping in [dn['power_fields'],dn['motion_fields']]:
 for o,n in mapping.values():covered.update(range(o,o+n))
for o,k in {**wn['fields'],**wn['extra']}.items():
 n=wn['width'](o)
 if any(i in covered for i in range(o,o+n)):continue
 key=('input_'+k if o in wn['extra'] or o in [0,4,8] else k)
 p.write(p.base+o,wrows[0][key],n)
p.write(p.base+0x36c,wrows[0]['ground36c']);p.write(p.base+0xe60,wrows[0]['input_material_e60'],2)
camera_rows=[list(map(int,l.split())) for l in (root/'reports/ordinary-physics-startup-camera.txt').read_text().splitlines()][-len(drows):]
assert len(camera_rows)==60 and all(len(a)==31 for a in camera_rows)
c=cn['CameraProbe']((0,0,0,0,0,1,0,0,0,0));cb=0x2002634;camera_checks=0
for a in range(cb,cb+0xea0,4):c.arm.mem[a]=0
first=camera_rows[0]
for o,v,n in [(0x64c,first[18],2),(0x654,first[0],4),(0x650,first[2],4),(0x680,first[13],4)]:p.write(p.base+o,v,n)
c.arm.mem[0x2002618]=first[4]&0xffffffff;c.arm.mem[0x2002628]=first[6]&0xffffffff
context_previous=None;context_checks=0
owned_context=[(0x2002630,4),(0x2001654,4),(0x20025d4,1),(0x20025d2,1),(0x20025dc,4)]
for tick,(d,z) in enumerate(zip(drows,wrows)):
 for prefix,mapping in [('power',dn['power_fields']),('motion',dn['motion_fields'])]:
  for k,(o,n) in mapping.items():
   got=p.read(p.base+o,n);want=d['input_'+prefix+'_'+k]&((1<<(8*n))-1);assert got==want,(tick,'persistent input',prefix,k,got,want);checks+=1
 setup=dsource.split(' probe.arm.mem[0x20034d4]=1')[1].split(' probe.actions=')[0]
 setup='probe.arm.mem[0x20034d4]=1'+setup
 # External input/global contexts are explicitly replayed; actor bytes never reseeded.
 scope=dict(d=d,probe=p,row=[tick]);exec('\n'.join(l[1:] if l.startswith(' ') else l for l in setup.splitlines()),scope)
 if context_previous is not None:
  for a,n in owned_context:
   got=context_previous[(a,n)];expected=p.read(a,n)
   assert got==expected,(tick,'driver context feedback',hex(a),got,expected)
   p.write(a,got,n);context_checks+=1
 p.actions={i:d['control_action'+str(i)] for i in [2,3,4,5]};p.steps=0;p.arm.steps=0;p.reg[14]=0x10020001;p.run(entry=0x8011a3c,stop=0x10020000)
 for prefix,mapping in [('power',dn['power_fields']),('motion',dn['motion_fields'])]:
  for k,(o,n) in mapping.items():assert p.read(p.base+o,n)==d['output_'+prefix+'_'+k]&((1<<(8*n))-1),(tick,'driver',prefix,k)
 for o,k,n in [(0xc,'displacement_x',4),(0x10,'displacement_y',4),(0x14,'displacement_z',4),(0x57c,'response_57c',2)]:
  assert p.read(p.base+o,n)==d['output_'+k]&((1<<(8*n))-1),(tick,'driver output',k)
 for a,k,n in [(0x2002630,'context_gear_state_2630',4),(0x2001654,'context_gear_state_1654',4),(0x20025d4,'context_gear_state_25d4',1),(0x20025dc,'context_power_state_25dc',4),(0x20025d2,'context_power_state_25d2',1),(0x2002620,'context_freeze_2620',1)]:
  assert p.read(a,n)==d['output_'+k]&((1<<(8*n))-1),(tick,'driver global output',k)
 context_previous={(a,n):p.read(a,n) for a,n in owned_context}
 for o in range(0xea0):w.write(w.base+o,p.read(p.base+o,1),1)
 for o,k in {**wn['fields'],**wn['extra']}.items():
  n=wn['width'](o);key=('input_'+k if o in wn['extra'] or o in [0,4,8] else k)
  assert w.read(w.base+o,n)==z[key]&((1<<(8*n))-1),(tick,'driver-world handoff',k,w.read(w.base+o,n),z[key])
 for a,k in wn['globals_map'].items():
  n=4 if 0x2001540<=a<0x2001550 else 1
  if tick==0:w.write(a,z['input_global_'+k],n)
  else:assert w.read(a,n)==z['input_global_'+k]&((1<<(8*n))-1),(tick,'global feedback',k)
 w.arm.mem[0x2001eb0]=z['settings'];w.arm.mem[0x2001534]=z['submode']<<24;w.arm.mem[0x30053c0]=z['delta'];w.arm.mem[0x3006810]=0x087556cc;w.arm.mem[0x3006a9c]=0x8000b78;w.arm.mem[0x3006ad0]=0x8000a1c
 w.steps=0;w.arm.steps=0;w.reg[:2]=[0,w.base];w.reg[14]=0x10020001;w.run(entry=0x8011f80,stop=0x10020000)
 for o,k in {**wn['fields'],**wn['extra']}.items():assert w.read(w.base+o,wn['width'](o))==z['output_'+k]&((1<<(8*wn['width'](o)))-1),(tick,'world',k)
 for a,k in wn['globals_map'].items():
  n=4 if 0x2001540<=a<0x2001550 else 1
  assert w.read(a,n)==z['output_global_'+k]&((1<<(8*n))-1),(tick,'world global output',k)
 assert w.read(w.base+0x36c)==z['output_ground36c']&0xffffffff,(tick,'ground36c')
 assert w.read(w.base+0xe60,2)==z['output_material_e60']&65535,(tick,'material_e60')
 for o in range(0xea0):p.write(p.base+o,w.read(w.base+o,1),1)
 # Copy source-produced actor state into original camera probe, never native camera outputs.
 for o in range(0xea0):c.write(cb+o,p.read(p.base+o,1),1)
 case=camera_rows[tick][:19];expected=camera_rows[tick][19:]
 ch,h,cp,bp,off,target,zoom,scale,flags,delta,freeze,ay,az,cam_base,x,y,z,active,mode=case
 for o,v,n in [(0x654,ch,4),(0x20,h,4),(0x650,cp,4),(0x18,bp,2),(0xe84,scale,4),(0x4ac,flags,1),(0x680,cam_base,4),(0,x,4),(4,y,4),(8,z,4),(0x64c,mode,2)]:
  assert c.read(cb+o,n)==v&((1<<(8*n))-1),(tick,'world-camera persistent handoff',hex(o),c.read(cb+o,n),v);camera_checks+=1
 for a,v in [(0x2002618,off),(0x2002628,zoom)]:
  assert c.arm.mem[a]==v&0xffffffff,(tick,'camera global feedback',hex(a),c.arm.mem[a],v);camera_checks+=1
 for a,v in {0x2005f20:freeze,0x2001708:0,0x20034d4:1,0x20034d8:1,0x2001674:0,0x2001678:ay,0x200167c:az,0x2001694:(active&255)<<16,0x2003b00:delta,0x200261c:target,0x2002624:0,0x3006ac4:0x8000a40,0x3006a90:0x8000718,0x3006ab8:0x80009bc,0x3006a94:0x80009d4,0x3006ab4:0x80009ec}.items():c.arm.mem[a]=v
 c.steps=0;c.arm.steps=0;c.reg[:2]=[cb+0x498,cb];c.reg[14]=0x10020001;c.run(entry=0x8014dd8,stop=0x10020000)
 signed=cn['signed']
 got=[signed(c.read(cb+o)) for o in [0x654,0x650]]+[signed(c.arm.mem[0x2002618]),signed(c.arm.mem[0x2002628])]+[signed(c.read(cb+o)) for o in [0x76c,0x770,0x498,0x49c,0x4a0]]+[c.read(cb+o,2) if c.read(cb+o,2)<32768 else c.read(cb+o,2)-65536 for o in [0x4a4,0x4a6,0x4a8]]
 assert got==expected,(tick,'camera output',got,expected)
 for o in range(0xea0):p.write(p.base+o,c.read(cb+o,1),1)
report=dict(status='PASS',frames=len(drows),persistent_camera_handoff_checks=camera_checks,persistent_input_checks=checks,persistent_driver_context_checks=context_checks,scope='Original11A3C->11F80->14DD8 chain carries allEA0actorbytes without reseeding; originalworld height globals retained, originalgear/powercontextglobals retained; captured externalflags/command/action-query contexts replayed; source startup seed and original input producer remain unproved, retained camera fields654/650 and globals2618/2628; separate relocatedworld probe/explicit arithmetic contracts; not whole game frame')
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'reports/ordinary-physics-startup-driver.txt',root/'reports/ordinary-physics-startup-world.txt',root/'reports/ordinary-full-world-state-schema.json',root/'reports/ordinary-physics-startup-camera.txt',root/'tools/compare_live_ordinary_driver_rom.py',root/'tools/compare_live_ordinary_world_rom.py',root/'tools/probe_full_camera_view_rom.py',root/'bin/scene-viewer-ordinary-physics-capture-check.exe']}
(root/'reports/ordinary-startup-persistent-camera-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
