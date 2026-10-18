"""Original full driver loop diagnostic with explicit replay command/action fixtures."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_acceleration_rom.py').read().split('steps=actions=0')[0])
power_fields={('drag_528' if k=='drag_520' else 'drag_590' if k=='drag_560' else k):((0x528,n) if k=='drag_520' else (0x590,n) if k=='drag_560' else (o,n)) for k,(o,n) in fields.items()}
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
schema=json.loads((root/'reports/ordinary-drag-corrected-full-driver-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-current-retained-analog-driver.txt').read_text().splitlines()];analog_rows=[list(map(int,l.split())) for l in (root/'reports/bound-current-retained-analog-command-actions.txt').read_text().splitlines()];analog={row[0]:row[46]|(row[47]<<8) for row in analog_rows};steps=armsteps=0
assert len(rows)>0 and all(len(row)==len(schema)+1 for row in rows)
for row in rows:
 d=dict(zip(schema,row[1:]));probe=DriverProbe((0,0,0,0,0,1,0,0,0,0));probe.base=0x2002634
 for a in range(probe.base,probe.base+0xea0,4):probe.arm.mem[a]=0
 old_arm_write=probe.arm.write
 def arm_write(a,v):
  if probe.base<=a<probe.base+0xea0:probe.arm.mem[a]=v&MASK
  else:old_arm_write(a,v)
 probe.arm.write=arm_write
 for key,(off,n) in power_fields.items():probe.write(probe.base+off,d['input_power_'+key],n)
 for key,(off,n) in motion_fields.items():probe.write(probe.base+off,d['input_motion_'+key],n)
 probe.write(probe.base+0x57a,d['input_steering_rate_57a'],2)
 probe.arm.mem[0x20034d4]=1;probe.arm.mem[0x20034d8]=1;probe.arm.mem[0x30053c0]=d['control_delta'];probe.arm.mem[0x2003b00]=d['control_frame_delta'];probe.arm.mem[0x30053d0]=0;probe.arm.mem[0x30053d4]=d['input_motion_command4'];probe.arm.mem[0x30053dc]=0
 probe.arm.mem[0x2002630]=d['input_context_gear_state_2630'];probe.arm.mem[0x2001654]=d['input_context_gear_state_1654'];probe.arm.mem[0x20025d4]=d['input_context_gear_state_25d4'];probe.arm.mem[0x20025d0]=d['input_context_power_state_25d2']<<16;probe.arm.mem[0x20025dc]=d['input_context_power_state_25dc'];probe.arm.mem[0x2002620]=d['input_context_freeze_2620'];probe.arm.mem[0x200262c]=d['input_motion_latch262c']
 probe.arm.mem[0x2001eb0]=d['control_settings'];probe.arm.mem[0x2001534]=d['input_motion_submode']<<24;probe.arm.mem[0x200153c]=d['control_flag_153c'];probe.arm.mem[0x2001694]=(d['input_motion_flag1696']<<16)|(d['control_flag_1697']<<24);probe.arm.mem[0x2001778]=d['control_freeze_177b']<<24;probe.arm.mem[0x2007600]=d['control_sound_enabled']<<16
 for a in range(0x20034e0,0x20034f0,4):probe.arm.mem[a]=0
 for i in range(10):
  a=0x20034e2+i;probe.arm.mem[a&~3]|=d['control_held_'+str(i)]<<((a&3)*8)
 probe.arm.mem[0x30053dc]=analog[row[0]];probe.actions={i:d['control_action'+str(i)] for i in [2,3,4,5]};probe.reg[14]=0x10020001;probe.run(entry=0x8011a3c,stop=0x10020000)
 for prefix,mapping in [('power',power_fields),('motion',motion_fields)]:
  for key,(off,n) in mapping.items():
   got=probe.read(probe.base+off,n);want=d['output_'+prefix+'_'+key]&((1<<(8*n))-1);assert got==want,dict(tick=row[0],field=prefix+'_'+key,original=got,native=want)
 for off,key,n in [(0xc,'displacement_x',4),(0x10,'displacement_y',4),(0x14,'displacement_z',4),(0x57c,'response_57c',2)]:assert probe.read(probe.base+off,n)==d['output_'+key]&((1<<(8*n))-1),(row[0],key)
 steps+=probe.steps;armsteps+=probe.arm.steps
report=dict(cases=len(rows),thumb_instructions=steps,arm_instructions=armsteps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original11A3C on current HUD candidate actual native driver states with captured1697; outputs retained into native world stage; nested original drivingroutines; power/motion/displacementoutputs compared. Explicitcommandpacker/actions fixtures andsourceinitializerglobals/arithmetic/logcontracts; render/effectwrites fixtureonly, notcompared/fullGBAtrajectory.')
(root/'reports/bound-current-retained-analog-driver-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
