"""Original final world pose rotation on actual captured post-body states."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
allowed.append((0xa1c,0xa40))
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-world-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
assert int.from_bytes(r[0x2b094:0x2b098],'little')==0x8000b78 and int.from_bytes(r[0x2b074:0x2b078],'little')==0x8000a1c
class PoseProbe(MotionProbe):
 def external(self,target):
  assert target in [0x8032454,0x8032458]
  entry=self.reg[2 if target==0x8032454 else 3];assert entry in [0x8000b78,0x8000a1c]
  self.arm.reg[13]=self.reg[13];self.arm.run(entry,self.reg[:4]);self.reg[:4]=self.arm.reg[:4]
schema=json.loads((root/'reports/bound-launch-world-full-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-world-full-states.txt').read_text().splitlines()];steps=armsteps=0
for row in rows:
 d=dict(zip(schema,row[1:]));probe=PoseProbe((0,0,0,0,0,1),0);base=probe.base;probe.reg[5]=base;probe.reg[13]-=256
 for off,key in [(0x1c,'output_heading_1c'),(0x69c,'output_drift_69c'),(0x6f4,'output_height_body_orientation_6f4_x'),(0x6f8,'output_height_body_orientation_6f4_y'),(0x6fc,'output_height_body_orientation_6f4_z'),(0x704,'output_response704'),(0x70c,'output_response70c')]:probe.write(base+off,d[key])
 probe.write(probe.reg[13]+0x54,d['ground_height']);probe.arm.mem[0x3006a9c]=0x8000b78;probe.arm.mem[0x3006ad0]=0x8000a1c;probe.run(entry=0x80124ee,stop=0x8012536)
 got=[probe.read(base+off,2) for off in [0x38,0x3a,0x18,0x1a]]+[signed(probe.read(base+0x3c))];expected=[d[k]&65535 for k in ['output_angle_38','output_angle_3a','output_pitch','output_roll']]+[d['output_height_3c']];assert got==expected,dict(tick=row[0],original=got,native=expected);steps+=probe.steps;armsteps+=probe.arm.steps
report=dict(cases=len(rows),thumb_instructions=steps,arm_instructions=armsteps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalworld24EE..2534 plusoriginalARM B78 Ymatrix/A1C pointtransform on actualpostbodystates; angle38/3A,pitch/roll,height3C compare. Inputpostbodyfixture, snapshotcopy/movement/fullworld excluded.')
(root/'reports/bound-final-pose-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
