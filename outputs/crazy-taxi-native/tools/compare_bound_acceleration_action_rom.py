"""Complete original acceleration on captured native pre/post inputs."""
exec(open('outputs/crazy-taxi-native/tools/compare_rpm_rom.py').read().split('\ncases=')[0])
allowed_thumb={}
for name in ['acceleration','rpm','acceleration-action-query']:
 cfg=json.loads((root/('reports/challenge-bound-'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class AccelerationProbe(RpmProbe):
 def write(self,a,v,n=4):
  if a in [0x2002630,0x2001654,0x20025dc,0x20025d2]:
   old=self.arm.read(a&~3);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target==0x8013610:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8015d98:
   assert self.reg[0]==2;self.actions+=1;self.run(entry=target,stop=self.reg[14]&~1);assert self.reg[0]==self.action2
  else:super().external(target)
schema=json.loads((root/'reports/bound-launch-acceleration-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-acceleration-states.txt').read_text().splitlines()]
explicit={'gears_max_gear':(0x4b4,4),'gears_reverse_ratio':(0x4d4,4),'gears_final_ratio':(0x4d8,4),'gears_gear':(0x4dc,1),'gears_next_gear':(0x4dd,1),'gears_shift_timer':(0x4de,2)}
byte=['flags_4ac','flags_28','state_e68','state_e69','phase_e6a','phase_e6b','state_6aa','state_6a9'];half=['min_530','max_532','up_534','down_536','rpm_538','steering_578']
fields={}
for name in schema:
 if not name.startswith('input_') or name.startswith('input_global_'):continue
 key=name[6:]
 if key in ['delta','frame_delta','player_index','settings','flag_1697','flag_153c','action2']:continue
 if key in explicit:fields[key]=explicit[key]
 elif key.startswith('gear_ratio_'):fields[key]=(0x4b8+int(key.split('_')[-1])*4,4)
 elif key.startswith('curve_'):fields[key]=(0x4e8+int(key.split('_')[-1])*4,4)
 else:
  short=key.removeprefix('gears_');tail=short.split('_')[-1]
  try:off=int(tail,16)
  except ValueError:continue
  if off<0x20:continue
  fields[key]=(off,1 if short in byte else (2 if short in half else 4))
globals_map={'state_2630':(0x2002630,4),'state_1654':(0x2001654,4),'state_25dc':(0x20025dc,4),'state_25d2':(0x20025d2,1)}
route=[list(map(int,l.split())) for l in pathlib.Path('work/bound-acceleration-flight-route.txt').read_text().splitlines()]
steps=actions=0
for row in rows:
 d=dict(zip(schema,row[1:]));probe=AccelerationProbe((0,0,0,0,0,d['input_delta']),0);probe.invalid=0;probe.actions=0;probe.action2=d['input_action2']
 for key,(off,n) in fields.items():probe.write(probe.base+off,d['input_'+key],n)
 for key,(a,n) in globals_map.items():probe.arm.mem[a&~3]=0;probe.write(a,d['input_global_'+key],n)
 probe.arm.mem[0x2001eb0]=d['input_settings'];probe.arm.mem[0x2001694]=d['input_flag_1697']<<24;probe.arm.mem[0x200153c]=d['input_flag_153c'];probe.arm.mem[0x2003b00]=d['input_frame_delta']
 ctr=(root/'assets/driving.ctr').read_bytes()
 for i in range(4096):probe.arm.mem[0x202c4f0+i*4]=(int.from_bytes(ctr[4+i*2:6+i*2],'little',signed=True)>>4)&MASK
 probe.arm.mem[0x2001708]=1;probe.arm.mem[0x30053d4]=route[row[0]][0]&15
 probe.reg[0]=probe.base;probe.reg[9]=d['input_player_index'];probe.reg[14]=0x10020001;probe.run(entry=0x80137dc,stop=0x10020000)
 for key,(off,n) in fields.items():
  got=probe.read(probe.base+off,n);expected=d['output_'+key]&((1<<(8*n))-1);assert got==expected,dict(tick=row[0],field=key,original=got,native=expected)
 for key,(a,n) in globals_map.items():assert probe.read(a,n)==d['output_global_'+key]&((1<<(8*n))-1),(row[0],key)
 assert probe.invalid==d['effects_invalid_mode'] and d['effects_count']==0;steps+=probe.steps;actions+=probe.actions
report=dict(cases=len(rows),mapped_state_fields=len(fields),thumb_instructions=steps,action_query_calls=actions,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal137DC andnested13610 compareactual124nativeaccelerationupdates state/global/effectoutputs. Original15D98 actionquery executescommand-override branch fromrecordedinput-only replay; division/trigglobalinitializercontracts anddiagnosticlogcapture. Notouterdriverordering orindependentGBAtrajectory.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/compare_bound_acceleration_action_rom.py',root/'reports/bound-launch-acceleration-states.txt',root/'reports/bound-launch-acceleration-state-schema.json',root/'src/acceleration.c',root/'src/gears.c',root/'reports/challenge-bound-acceleration-cfg.json']}
(root/'reports/bound-acceleration-action-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)


