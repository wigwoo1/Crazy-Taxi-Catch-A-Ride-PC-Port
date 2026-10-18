"""Replay actual captured native launch inputs through original vehicle routines."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');schema=json.loads((root/'reports/bound-launch-vehicle-state-schema.json').read_text());rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-vehicle-states.txt').read_text().splitlines()];assert len(rows)==161
fields={'x':(0,4),'y':(4,4),'z':(8,4),'pitch':(0x18,2),'roll':(0x1a,2),'heading1c':(0x1c,4),'heading20':(0x20,4),'drift69c':(0x69c,4),'force540':(0x540,4),'state564':(0x564,4),'speed544':(0x544,4),'timer2a':(0x2a,2),'flags358':(0x358,1),'flags28':(0x28,1),'phasee6a':(0xe6a,1),'steer578':(0x578,2),'phasee6b':(0xe6b,1),'limit7b0':(0x7b0,4),'scale7c8':(0x7c8,4),'scalee84':(0xe84,4),'dimension594':(0x594,4),'force53c':(0x53c,4),'capacity570':(0x570,4),'capacity574':(0x574,4),'response744':(0x744,4),'flags4ac':(0x4ac,1)}
steps=armsteps=0;phases={};airborne=0
for row in rows:
 assert len(row)==59;tick=row[0];inp=dict(zip(schema['input'],row[1:36]));expected=row[36:];probe=VehicleProbe((0,0,0,0,0,1,0,0,0,0))
 for name,(off,n) in fields.items():probe.write(probe.base+off,inp[name],n)
 probe.arm.mem[0x30053c0]=inp['delta'];probe.arm.mem[0x30053d4]=inp['command4'];probe.arm.mem[0x2001eb0]=inp['settings'];probe.arm.mem[0x2001534]=inp['submode']<<24
 probe.arm.mem[0x2001694]=(inp['flag1696']<<16)|(inp['flag1697']<<24);probe.arm.mem[0x20025d4]=inp['flag25d4'];probe.arm.mem[0x200262c]=inp['latch262c'];probe.arm.mem[0x2002630]=inp['state2630']
 probe.run(entry=0x80126dc,stop=0x10020000)
 def word(off):return signed(probe.read(probe.base+off))
 def half(off):
  v=probe.read(probe.base+off,2);return v-65536 if v&32768 else v
 got=[0]+[word(o) for o in [0,4,8,0x1c,0x20,0x69c,0x540,0x564,0x544]]+[half(0x2a),probe.read(probe.base+0x28,1),half(0x57c)]+[word(o) for o in [0xc,0x10,0x14,0x744]]+[probe.read(0x200262c,1),probe.read(0x20025d4,1)]+probe.reward
 assert got==expected,dict(tick=tick,differences=[(name,a,b) for name,a,b in zip(schema['output'],got,expected) if a!=b],input=inp)
 steps+=probe.steps;armsteps+=probe.arm.steps;phases[inp['phasee6a']]=phases.get(inp['phasee6a'],0)+1;airborne+=bool(inp['flags28']&8)
report=dict(cases=len(rows),ticks=[rows[0][0],rows[-1][0]],phases=phases,airborne_inputs=airborne,thumb_instructions=steps,matrix_arm_instructions=armsteps,angle_arm_instructions=angle_probe.steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual native launch inputs through original126DC and dependencies, all23 captured outputs compared. BIOSsqrt/divisioncontracts/trigfixtures. Not independent GBA trajectory or worldcontact.')
(root/'reports/bound-launch-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
