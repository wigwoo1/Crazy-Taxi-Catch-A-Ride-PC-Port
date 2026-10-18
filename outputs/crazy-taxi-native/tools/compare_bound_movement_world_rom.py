"""Original movement plus original obstacle query, actual native launch inputs."""
exec(open('outputs/crazy-taxi-native/tools/compare_movement_rom.py').read().split('cases=')[0])
exec(open('outputs/crazy-taxi-native/tools/compare_bound_ground_contact_rom.py').read().split('contactcfg=')[0])
allowed_thumb={}
for name in ['challenge-bound-movement-cfg.json','challenge-bound-obstacle-cfg.json']:
 cfg=json.loads((root/'reports'/name).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
MovementProbe.run=MotionProbe.run
class CombinedMovementProbe(MovementProbe):
 def external(self,target):
  if target==0x8029e84:
   self.queries.append(tuple(signed(self.read(self.reg[0]+i)) for i in [0,4,8]));self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x802df0c:GroundProbe.external(self,target)
  else:super().external(target)
schema=json.loads((root/'reports/bound-launch-world-full-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-world-full-states.txt').read_text().splitlines()];cases=[]
for row in rows:
 d=dict(zip(schema,row[1:]));dx=signed((d['dx']+(d['input_impulse_74c']>>12))&MASK);dz=signed((d['dz']+(d['input_impulse_750']>>12))&MASK);cases.append((d['input_x'],d['input_y'],d['input_z'],dx,d['dy'],dz,1))
res=subprocess.run([str(root/'bin/movement-world-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);native=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(native)==len(cases);steps=0;queries=0;collisions=0
for case,row,trace in zip(cases,native,rows):
 probe=CombinedMovementProbe(case[:6]+(0,case[6]));probe.arm.mem[0x3006810]=address;probe.run(entry=0x802a27c,stop=0x10020000);got=probe.outputs();assert row[0]==0 and got==row[1:],dict(tick=trace[0],case=case,original=got,native=row);steps+=probe.steps;queries+=len(probe.queries);collisions+=got[-1]
report=dict(cases=len(cases),thumb_instructions=steps,obstacle_queries=queries,collisions=collisions,containment_entry_v_counts=entry_v_counts,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original2A27C movement position path nestedoriginal29E84 obstacle selection with originalROMBound geometry; actual161 native launch preworld inputs including impulse displacements. Native fresh isolatedpositionpath comparison; doesnotcompare fullworldpostoutputs, impact sideeffects or independent GBA trajectory. sqrt/divisioncontracts and containmentcallee-saveabstraction persist.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/compare_bound_movement_world_rom.py',root/'tests/movement_world_rom_compare.c',root/'src/movement.c',root/'src/ground.c',root/'src/world_asset.c',root/'bin/movement-world-rom-compare.exe',root/'reports/bound-launch-world-full-states.txt']}
(root/'reports/bound-movement-world-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)

