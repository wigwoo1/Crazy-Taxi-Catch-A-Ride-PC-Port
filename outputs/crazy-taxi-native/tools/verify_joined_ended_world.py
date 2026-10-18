"""Combined original world update on recorded native inputs."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_movement_world_rom.py').read().split('schema=')[0])
exec(open('outputs/crazy-taxi-native/tools/verify_world_fill_rom.py').read().split('root=pathlib.Path')[0])
source=source.replace('self.steps<1000','self.steps<50000');exec(source)
CombinedMovementProbe.run=MotionProbe.run
allowed.append((0xa1c,0xa40));allowed_thumb={}
for name in ['world','fill','movement','obstacle','ground','contact-interpolation','contact-wrapper','gravity','decay','springs','align']:
 cfg=json.loads((root/('reports/challenge-bound-'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
for ins in md.disasm(r[0x2dd94:0x2dda6],0x802dd94):allowed_thumb[ins.address]=ins.bytes
class WorldProbe(CombinedMovementProbe):
 def write(self,a,v,n=4):
  if 0x200153c<=a<0x2001550 or a==0x2001697:
   old=self.arm.read(a&~3);shift=(a&3)*8;mask=((1<<(n*8))-1)<<shift;self.arm.mem[a&~3]=((old&~mask)|((v<<shift)&mask))&MASK
  else:super().write(a,v,n)
 def external(self,target):
  if target in [0x8033f2c,0x802a27c,0x802aba0,0x8029664,0x802a978,0x8015b00,0x8015b54,0x801488c,0x8014968,0x802dd94]:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032454 and self.reg[2]==0x8000b78 or target==0x8032458:
   entry=self.reg[2 if target==0x8032454 else 3];assert entry in [0x8000b78,0x8000a1c];self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
  elif target==0x8032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  else:super().external(target)
schema=json.loads((root/'reports/ordinary-full-world-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/joined-ended-world-states.txt').read_text().splitlines()]
fields={0:'x',4:'y',8:'z',0x10:'dy',0x698:'gravity698',0x28:'flags28',0x544:'speed544',0x18:'pitch',0x1a:'roll',0x704:'response704',0x70c:'response70c',0x730:'velocity730',0x732:'velocity732'}
extra={0x74c:'impulse_74c',0x750:'impulse_750',0x34:'impulse_34',0x1c:'heading_1c',0x20:'heading_20',0x69c:'drift_69c',0x40:'state_40',0x44:'state_44',0x2c:'state_2c',0xe84:'height_scale_e84',0x374:'red_374',0x375:'green_375',0x376:'blue_376',0x378:'contact_378',0x37c:'previous_37c',0x38:'angle_38',0x3a:'angle_3a',0x3c:'height_3c'}
for off,key in [(0x734,'position_734'),(0x740,'position_740'),(0x73c,'velocity_73c'),(0x748,'velocity_748'),(0x754,'denominator_754'),(0x758,'denominator_758'),(0x6f4,'orientation_6f4_x'),(0x6f8,'orientation_6f4_y'),(0x6fc,'orientation_6f4_z')]:extra[off]='height_body_'+key
for j,key in enumerate(['material','height','normal_x','normal_y','normal_z','flags','contact_value']):extra[0x714+j*4]='snapshot_714_'+key
width=lambda off:1 if off in [0x28,0x374,0x375,0x376] else (2 if off in [0x18,0x1a,0x730,0x732,0x2c,0x38,0x3a] else 4)
globals_map={0x2001544:'takeoff_1544_x',0x2001548:'takeoff_1544_y',0x200154c:'takeoff_1544_z',0x2001540:'distance_1540',0x200153e:'flag_153e',0x200153c:'flag_153c',0x200153d:'flag_153d',0x2001697:'flag_1697'}
address=0x087556cc # Ordinary city1 capture; never use inherited Bound course descriptor.
driver_schema=json.loads((root/'reports/ordinary-drag-corrected-full-driver-schema.json').read_text())['fields']
driver_rows=[list(map(int,l.split())) for l in (root/'reports/joined-ended-driver-states.txt').read_text().splitlines()]
assert len(driver_rows)==len(rows)==120
for driver_row,world_row in zip(driver_rows,rows):
 assert driver_row[0]==world_row[0]
 driver=dict(zip(driver_schema,driver_row[1:]));world=dict(zip(schema,world_row[1:]))
 for source,dest in [('output_displacement_x','dx'),('output_displacement_y','dy'),('output_displacement_z','dz')]:assert driver[source]==world[dest],(world_row[0],source,dest)
steps=armsteps=0
assert len(rows)==120 and all(len(row)==len(schema)+1 for row in rows)
for row in rows:
 d=dict(zip(schema,row[1:]));assert d['input_global_flag_1697']==1;probe=WorldProbe((0,0,0,0,0,0,0,1));base=probe.base
 for off,key in fields.items():
  ik='input_'+key if off in [0,4,8] else key;probe.write(base+off,d[ik],width(off))
 for off,key in extra.items():probe.write(base+off,d['input_'+key],width(off))
 for off,key in [(0xc,'dx'),(0x14,'dz'),(0x36c,'ground36c')]:probe.write(base+off,d[key])
 for a in [0x200153c,0x2001540,0x2001544,0x2001548,0x200154c,0x2001694]:probe.arm.mem[a]=0
 for a,key in globals_map.items():probe.write(a,d['input_global_'+key],4 if a>=0x2001540 and a<0x2001550 else 1)
 probe.write(base+0xe60,d['input_material_e60'],2);probe.arm.mem[0x2001eb0]=d['settings'];probe.arm.mem[0x2001534]=d['submode']<<24;probe.arm.mem[0x30053c0]=d['delta'];probe.arm.mem[0x3006810]=address;probe.arm.mem[0x3006a9c]=0x8000b78;probe.arm.mem[0x3006ad0]=0x8000a1c
 probe.reg[:2]=[0,base];probe.reg[14]=0x10020001;probe.run(entry=0x8011f80,stop=0x10020000)
 for off,key in {**fields,**extra}.items():
  expected=d['output_'+key]&((1<<(8*width(off)))-1);got=probe.read(base+off,width(off));assert got==expected,dict(tick=row[0],field=key,original=got,native=expected)
 for a,key in globals_map.items():
  n=4 if a>=0x2001540 and a<0x2001550 else 1;assert probe.read(a,n)==d['output_global_'+key]&((1<<(8*n))-1),(row[0],key,probe.read(a,n),d['output_global_'+key])
 assert probe.read(base+0x36c)==d['output_ground36c']&MASK,(row[0],'ground36c')
 assert probe.read(base+0xe60,2)==d['output_material_e60']&65535,(row[0],'material_e60')
 steps+=probe.steps;armsteps+=probe.arm.steps
report=dict(cases=len(rows),thumb_instructions=steps,arm_instructions=armsteps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original11F80 world update and recovered dependencies on120 copied synthetic native prestates with flag1697 forced1; mapped actor fields and eight globals compared. Inputs use separately compared ended-driver displacement; world outputs are diagnostic copies restored before normal world update. Not an original continuous ended trajectory. Ground36c and material_e60 outputs compared; completion_result and stage effects not compared. All120 driver/world displacement links asserted. Explicit BIOSsqrt/division/trigglobalinitializer/containmentcallee-save contracts. Independent fullGBAtrajectory, crash, fullgame notclaimed.')
(root/'reports/joined-ended-world-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)


