"""Original contact wrapper with nested original ground/interpolation."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_ground_contact_rom.py').read().split('schema=')[0])
wrapper=json.loads((root/'reports/challenge-bound-contact-wrapper-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in wrapper['instructions']})
class ContactProbe(GroundProbe):
 def external(self,target):
  if target==0x8029664:self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
schema=json.loads((root/'reports/bound-launch-world-full-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-world-full-states.txt').read_text().splitlines()];steps=0
for row in rows:
 d=dict(zip(schema,row[1:]));probe=ContactProbe((0,0,0,0,0,1),0);point=probe.base;contact=point+0x100
 for off,key in zip([0,4,8],['query_x','query_y','query_z']):probe.write(point+off,d[key])
 for off in range(0,28,4):probe.write(contact+off,0)
 probe.arm.mem[0x3006810]=address;probe.arm.mem[0x3006940]=0x80029fc;probe.reg[:2]=[point,contact];probe.reg[14]=0x10020001;probe.run(entry=0x802aba0,stop=0x10020000)
 got=[probe.read(contact+off) for off in range(0,28,4)]
 expected=[d['output_snapshot_714_'+key]&MASK for key in ['material','height','normal_x','normal_y','normal_z','flags','contact_value']]
 assert probe.reg[0]==d['ground_hit'] and got==expected,dict(tick=row[0],original=got,native=expected);steps+=probe.steps
report=dict(cases=len(rows),thumb_instructions=steps,containment_entry_v_counts=entry_v_counts,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original2ABA0 wrapper nestsoriginal29664 groundselection and2A978 interpolation; all7packagedwords andreturn comparedtoactualnativeworld snapshot across161queries. Explicit zero contactbufferfixture, signeddivision and containmentcallee-saveabstraction; no wholeworld/GBAtrajectory.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/compare_bound_contact_wrapper_rom.py',root/'reports/challenge-bound-contact-wrapper-cfg.json',root/'reports/bound-launch-world-full-states.txt',root/'src/ground.c']}
(root/'reports/bound-contact-wrapper-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
