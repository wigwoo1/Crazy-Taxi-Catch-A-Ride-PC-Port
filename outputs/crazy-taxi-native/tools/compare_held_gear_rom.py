"""Actual gear-only normal-frame hold updates vs original130AC."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_gear_rom.py').read().split('\nschema=')[0])
class HeldProbe(AccelerationProbe):
 def external(self,target):
  if target==0x80267c8:
   assert self.reg[1:3]==[0,0];self.sounds.append(self.reg[0])
  else:super().external(target)
schema=json.loads((root/'reports/held-gear-schema.json').read_text());fields=schema['fields'];inputs=schema['inputs'];glob=schema['globals'];n=len(fields)+len(glob);rows=[list(map(int,l.split())) for l in (root/'reports/held-gear-states.txt').read_text().splitlines()];assert len(rows)==179;steps=actions=sounds=0
for index,row in enumerate(rows):
 assert len(row)==len(inputs)+2*n+4,(index,len(row));d=dict(zip(inputs,row[:len(inputs)]));pre=row[len(inputs):len(inputs)+n];post=row[len(inputs)+n:len(inputs)+2*n];effects=row[-4:]
 p=HeldProbe((0,0,0,0,0,d['delta']),0);p.invalid=0;p.actions=0;p.sounds=[];p.action_values={i:d['action'+str(i)] for i in [2,3,4,5]}
 for f,v in zip(fields,pre):p.write(p.base+f['offset'],v,f['bytes'])
 for (a,size),v in zip([(0x2002630,4),(0x2001654,4),(0x20025d4,1)],pre[len(fields):]):p.arm.mem[a&~3]=0;p.write(a,v,size)
 p.arm.mem[0x2001eb0]=d['settings'];p.arm.mem[0x2002620]=d['freeze_2620'];p.arm.mem[0x2001778]=d['freeze_177b']<<24;p.arm.mem[0x2007600]=d['sound_enabled']<<16;p.arm.mem[0x2003b00]=d['frame_delta'];p.arm.mem[0x30053d4]=0
 for a in range(0x20034e0,0x20034f0,4):p.arm.mem[a]=0
 for i in range(10):a=0x20034e2+i;p.arm.mem[a&~3]|=d['held[%d]'%i]<<((a&3)*8)
 p.reg[0]=p.base;p.reg[1]=0;p.reg[9]=0;p.reg[14]=0x10020001;p.run(entry=0x80130ac,stop=0x10020000)
 for f,want in zip(fields,post):assert p.read(p.base+f['offset'],f['bytes'])==want&((1<<(f['bytes']*8))-1),dict(index=index,field=f['name'],source=p.read(p.base+f['offset'],f['bytes']),native=want)
 for (a,size),want in zip([(0x2002630,4),(0x2001654,4),(0x20025d4,1)],post[len(fields):]):assert p.read(a,size)==want&((1<<(size*8))-1),(index,hex(a))
 assert p.invalid==effects[3] and len(p.sounds)==effects[0]
 expected=[struct.unpack_from('<I',r,0x7ec65c+i*4)[0] for i in effects[1:1+effects[0]]];assert p.sounds==expected,(index,p.sounds,expected)
 steps+=p.steps;actions+=p.actions;sounds+=len(p.sounds)
report=dict(cases=len(rows),mapped_gear_fields=len(fields),mapped_globals=len(glob),thumb_instructions=steps,action_queries=actions,sound_requests=sounds,match=True,scope='Completeoriginal130AC withactualnested13610/136D4 vsALL179currentnativegear-onlyboarding/deliveryhold updates.35gearfields/3globals/effects exact. Explicitaction2-5/contracts/settings/held/deltafixtures; nativeworld/contextduplicateprojection/fullframe notproved.',rom_sha256=hashlib.sha256(r).hexdigest())
report['sha256']={f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['src/startup_world.c','src/gears.c','src/scene_viewer.c','bin/scene-viewer.exe','reports/held-gear-schema.json','reports/held-gear-states.txt','reports/held-gear-route.log','tools/compare_held_gear_rom.py']}
(root/'reports/held-gear-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',{k:v for k,v in report.items() if k!='sha256'})
