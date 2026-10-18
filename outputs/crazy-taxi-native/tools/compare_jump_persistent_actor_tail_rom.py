"""Original driver/world persistent actor-byte chain; explicit external input fixtures."""
import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native')
dn={};wn={}
dsource=(root/'tools/compare_jump_legal_driver_loop_rom.py').read_text();wsource=(root/'tools/compare_jump_legal_world_rom.py').read_text()
exec(dsource.split('schema=json.loads')[0],dn)
exec(wsource.split('schema=json.loads')[0],wn)
exec(wsource.split('fields={')[1].split('steps=armsteps=0')[0].join(['fields={','']),wn)
ds=json.loads((root/'reports/bound-route-dash-full-driver-state-schema.json').read_text())['fields'];ws=json.loads((root/'reports/bound-route-world-full-state-schema.json').read_text())['fields']
drows=[dict(zip(ds,map(int,l.split()[1:]))) for l in (root/'reports/jump-legal-full-driver-states.txt').read_text().splitlines()];wrows=[dict(zip(ws,map(int,l.split()[1:]))) for l in (root/'reports/jump-legal-full-world-states.txt').read_text().splitlines()];buttons=[list(map(int,l.split())) for l in (root/'reports/jump-legal-combined-effect-states.txt').read_text().splitlines()][203:]
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
for o,k in wn['extra'].items():p.write(p.base+o,wrows[0]['input_'+k],wn['width'](o))
p.write(p.base+0x36c,wrows[0]['ground36c'])
en={}
exec((root/'tools/persistent_effect_tail_support.py').read_text(),en)
for intro in range(203):en['tail'](intro,en['rows'][intro])
context_previous=None;context_checks=0
owned_context=[(0x2002630,4),(0x2001654,4),(0x20025d4,1),(0x20025d2,1),(0x20025dc,4)]
for tick,(d,z,b) in enumerate(zip(drows,wrows,buttons)):
 for prefix,mapping in [('power',dn['power_fields']),('motion',dn['motion_fields'])]:
  for k,(o,n) in mapping.items():
   got=p.read(p.base+o,n);want=d['input_'+prefix+'_'+k]&((1<<(8*n))-1);assert got==want,(tick,'persistent input',prefix,k,got,want);checks+=1
 setup=dsource.split(' probe.arm.mem[0x20034d4]=1')[1].split(' probe.actions=')[0]
 setup='probe.arm.mem[0x20034d4]=1'+setup
 # External input/global contexts are explicitly replayed; actor bytes never reseeded.
 scope=dict(d=d,probe=p,button_rows={tick:b[685:715]+b[715:720]},row=[tick]);exec('\n'.join(l[1:] if l.startswith(' ') else l for l in setup.splitlines()),scope)
 if context_previous is not None:
  for a,n in owned_context:
   got=context_previous[(a,n)];expected=p.read(a,n)
   assert got==expected,(tick,'driver context feedback',hex(a),got,expected)
   p.write(a,got,n);context_checks+=1
 p.steps=0;p.arm.steps=0;p.reg[14]=0x10020001;p.run(entry=0x8011a3c,stop=0x10020000)
 for prefix,mapping in [('power',dn['power_fields']),('motion',dn['motion_fields'])]:
  for k,(o,n) in mapping.items():assert p.read(p.base+o,n)==d['output_'+prefix+'_'+k]&((1<<(8*n))-1),(tick,'driver',prefix,k)
 context_previous={(a,n):p.read(a,n) for a,n in owned_context}
 for o in range(0xea0):w.write(w.base+o,p.read(p.base+o,1),1)
 for a,k in wn['globals_map'].items():
  n=4 if 0x2001540<=a<0x2001550 else 1
  if tick==0:w.write(a,z['input_global_'+k],n)
  else:assert w.read(a,n)==z['input_global_'+k]&((1<<(8*n))-1),(tick,'global feedback',k)
 w.arm.mem[0x2001eb0]=z['settings'];w.arm.mem[0x2001534]=z['submode']<<24;w.arm.mem[0x30053c0]=z['delta'];w.arm.mem[0x3006810]=0x087ebb90;w.arm.mem[0x3006a9c]=0x8000b78;w.arm.mem[0x3006ad0]=0x8000a1c
 w.steps=0;w.arm.steps=0;w.reg[:2]=[0,w.base];w.reg[14]=0x10020001;w.run(entry=0x8011f80,stop=0x10020000)
 for o,k in {**wn['fields'],**wn['extra']}.items():assert w.read(w.base+o,wn['width'](o))==z['output_'+k]&((1<<(8*wn['width'](o)))-1),(tick,'world',k)
 tail_actor=en['tail'](tick+203,en['rows'][tick+203],[w.read(w.base+o,1) for o in range(0xea0)],(p.read(0x20025d4,1),p.read(0x2002630,4)))
 for o,v in enumerate(tail_actor):p.write(p.base+o,v,1)
report=dict(frames=len(drows),persistent_input_checks=checks,persistent_driver_context_checks=context_checks,effect_updates=500,effect_action_queries=en['p'].action_calls,effect_height_queries=en['p'].height_calls,scope='Original11A3C->11F80 chain carries allEA0actorbytes without reseeding; originalworld height globals retained, originalgear/powercontextglobals retained; externalflags/buttoncontexts replayed, separate relocatedworld probe/explicit arithmetic contracts; original145E4/four emitters/globalpool added afterworld,500effectstates compared incl203intro; separateproberelocation/externalflags remain, notwholegameframe')
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [pathlib.Path(__file__),root/'reports/jump-legal-full-driver-states.txt',root/'reports/jump-legal-full-world-states.txt',root/'reports/jump-legal-combined-effect-states.txt']}
(root/'reports/jump-persistent-actor-tail-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
