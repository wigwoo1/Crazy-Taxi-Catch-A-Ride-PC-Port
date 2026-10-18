"""Original full11A3C ordinary driver, captured actual native actor inputs. Offline only."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_acceleration_rom.py').read().split('steps=actions=0')[0])
power_fields=fields.copy()
source=source.replace("'blt':n!=v}","'blt':n!=v,'bls':not c or z}").replace('self.steps<1000','self.steps<50000');exec(source);VehicleProbe.run=MotionProbe.run
allowed_thumb={}
for name in ['driver-loop','gear','clamp','rpm','acceleration','free-rev','brake','vehicle','steering','orientation','traction','vector-angle','response']:
 cfg=json.loads((root/('reports/challenge-bound-'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
for ins in md.disasm(r[0x2dd74:0x2dd94],0x802dd74):allowed_thumb[ins.address]=ins.bytes
motion_fields={'x':(0,4),'y':(4,4),'z':(8,4),'pitch':(0x18,2),'roll':(0x1a,2),'heading1c':(0x1c,4),'heading20':(0x20,4),'drift69c':(0x69c,4),'force540':(0x540,4),'state564':(0x564,4),'speed544':(0x544,4),'timer2a':(0x2a,2),'flags358':(0x358,1),'flags28':(0x28,1),'phasee6a':(0xe6a,1),'steer578':(0x578,2),'phasee6b':(0xe6b,1),'limit7b0':(0x7b0,4),'scale7c8':(0x7c8,4),'scalee84':(0xe84,4),'dimension594':(0x594,4),'force53c':(0x53c,4),'capacity570':(0x570,4),'capacity574':(0x574,4),'response744':(0x744,4),'flags4ac':(0x4ac,1)}
class DriverProbe(VehicleProbe):
 def write(self,a,v,n=4):
  if 0x200153c<=a<0x2001550 or 0x200164c<=a<0x2001660 or 0x20025d0<=a<0x20034d4 or a==0x30053d4 or a==0x500029e:
   old=self.arm.mem.get(a&~3,0);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x80130ac,0x80136d4,0x8013610,0x80137dc,0x8013dc8,0x801403c,0x80126dc,0x802dd74]:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8011734:assert self.reg[0]==0 # command alreadyprovidedexplicitfixture
  elif target==0x8015d98:
   action=self.reg[0];assert action in [2,3,4,5];self.reg[0]=self.actions[action]
  elif target==0x80325f0:assert self.reg[1];self.reg[0]=self.reg[0]//self.reg[1]
  elif target==0x802c880:pass # diagnosticlogcapture
  else:super().external(target)
import sys
label=sys.argv[1];binary=pathlib.Path(sys.argv[2]);trace=root/('reports/'+label+'.txt')
schema=json.loads((root/'reports/bound-corrected-full-driver-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in trace.read_text().splitlines()];assert rows and all(len(a)==len(schema)+1 for a in rows);steps=armsteps=0
source_writes={}
original_write=DriverProbe.write
def tracked_write(self,a,v,n=4):
 if getattr(self,'track_actor_writes',False) and a-self.base in (0x1c,0x20,0x358,0x564):
  key=(hex(a-self.base),hex(self.reg[15]-4));source_writes[key]=source_writes.get(key,0)+1
 return original_write(self,a,v,n)
DriverProbe.write=tracked_write
for row in rows:
 d=dict(zip(schema,row[1:]));assert d['control_settings']==0 and d['input_motion_phasee6a']==0;probe=DriverProbe((0,0,0,0,0,1,0,0,0,0));probe.base=0x2002634
 for a in range(probe.base,probe.base+0xea0,4):probe.arm.mem[a]=0
 old_arm_write=probe.arm.write
 def arm_write(a,v):
  if probe.base<=a<probe.base+0xea0:probe.arm.mem[a]=v&MASK
  else:old_arm_write(a,v)
 probe.arm.write=arm_write
 packed_inputs={}
 for prefix,mapping in [('power',power_fields),('motion',motion_fields)]:
  for key,(off,n) in mapping.items():
   value=d['input_'+prefix+'_'+key]&((1<<(8*n))-1)
   if (off,n) in packed_inputs:assert packed_inputs[(off,n)]==value,('native actor alias inputs disagree',row[0],prefix,key,packed_inputs[(off,n)],value)
   packed_inputs[(off,n)]=value;probe.write(probe.base+off,value,n)
 probe.write(probe.base+0x57a,d['input_steering_rate_57a'],2)
 probe.arm.mem[0x20034d4]=1;probe.arm.mem[0x20034d8]=1;probe.arm.mem[0x30053c0]=d['control_delta'];probe.arm.mem[0x2003b00]=d['control_frame_delta'];probe.arm.mem[0x30053d0]=0;probe.arm.mem[0x30053d4]=d['input_motion_command4'];probe.arm.mem[0x30053dc]=0
 probe.arm.mem[0x2002630]=d['input_context_gear_state_2630'];probe.arm.mem[0x2001654]=d['input_context_gear_state_1654'];probe.arm.mem[0x20025d4]=d['input_context_gear_state_25d4'];probe.arm.mem[0x20025d0]=d['input_context_power_state_25d2']<<16;probe.arm.mem[0x20025dc]=d['input_context_power_state_25dc'];probe.arm.mem[0x2002620]=d['input_context_freeze_2620'];probe.arm.mem[0x200262c]=d['input_motion_latch262c']
 probe.arm.mem[0x2001eb0]=d['control_settings'];probe.arm.mem[0x2001534]=d['input_motion_submode']<<24;probe.arm.mem[0x200153c]=d['control_flag_153c'];probe.arm.mem[0x2001694]=(d['input_motion_flag1696']<<16)|(d['control_flag_1697']<<24);probe.arm.mem[0x2001778]=d['control_freeze_177b']<<24;probe.arm.mem[0x2007600]=d['control_sound_enabled']<<16
 for a in range(0x20034e0,0x20034f0,4):probe.arm.mem[a]=0
 for i in range(10):
  a=0x20034e2+i;probe.arm.mem[a&~3]|=d['control_held_'+str(i)]<<((a&3)*8)
 probe.actions={i:d['control_action'+str(i)] for i in [2,3,4,5]};probe.track_actor_writes=True;probe.reg[14]=0x10020001;probe.run(entry=0x8011a3c,stop=0x10020000)
 for prefix,mapping in [('power',power_fields),('motion',motion_fields)]:
  for key,(off,n) in mapping.items():
   got=probe.read(probe.base+off,n);want=d['output_'+prefix+'_'+key]&((1<<(8*n))-1);assert got==want,dict(tick=row[0],field=prefix+'_'+key,original=got,native=want)
 for off,key,n in [(0xc,'displacement_x',4),(0x10,'displacement_y',4),(0x14,'displacement_z',4),(0x57c,'response_57c',2)]:assert probe.read(probe.base+off,n)==d['output_'+key]&((1<<(8*n))-1),(row[0],key)
 for address,key,n in [(0x2002630,'context_gear_state_2630',4),(0x2001654,'context_gear_state_1654',4),(0x20025d4,'context_gear_state_25d4',1),(0x2002630,'context_power_state_2630',4),(0x2001654,'context_power_state_1654',4),(0x20025dc,'context_power_state_25dc',4),(0x20025d2,'context_power_state_25d2',1),(0x2002620,'context_freeze_2620',1)]:
  assert probe.read(address,n)==d['output_'+key]&((1<<(8*n))-1),(row[0],key)
 steps+=probe.steps;armsteps+=probe.arm.steps
report=dict(status='PASS',cases=len(rows),mapped_power_fields=len(power_fields),mapped_motion_fields=len(motion_fields),shared_global_fields_compared=8,input_actor_aliases_checked=True,thumb_instructions=steps,arm_instructions=armsteps,rom_sha256=hashlib.sha256(r).hexdigest(),
 source_actor_writes=[dict(offset=k[0],pc=k[1],count=v) for k,v in sorted(source_writes.items())],
 scope='Actual native ordinary phase0/settings0 driver inputs/outputs vs entire original11A3C with nested original driving routines. All mapped power/motion/displacement fields and8 shared globals compared. Command/action-query results explicit actual-input contracts; source initializer trig globals, division/sqrt/log/reward contracts. Original source write owners for headings1C/20, flags358/state564 observed within this driver call. Does not prove original input producer, startup state lifecycle, collision/world/fullframe trajectory, GBA framebuffer or display60fps.',
 hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [pathlib.Path(__file__),trace,binary,root/'src/driver.c',root/'src/drive_world.c',root/'tests/ordinary_capture_full_driver.c',root/'tests/ordinary_capture_driver_world.c',root/'reports/bound-corrected-full-driver-state-schema.json']})
(root/('reports/'+label+'-rom-comparison.json')).write_text(json.dumps(report,indent=2)+'\n');print('PASS ordinary full driver',len(rows),'calls;',steps,'Thumb',armsteps,'ARM')
print('Source actor writes:',report['source_actor_writes'])
