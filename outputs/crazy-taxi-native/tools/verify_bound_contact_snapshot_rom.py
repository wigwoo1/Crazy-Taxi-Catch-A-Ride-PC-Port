"""Original contact snapshot LDM/STM sequence on actual captured contact values."""
exec(open('outputs/crazy-taxi-native/tools/verify_world_fill_rom.py').read().split('root=pathlib.Path')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-world-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']};schema=json.loads((root/'reports/bound-launch-world-full-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-world-full-states.txt').read_text().splitlines()];steps=0
for row in rows:
 d=dict(zip(schema,row[1:]));probe=MotionProbe((0,0,0,0,0,1),0);probe.reg[5]=probe.base;probe.reg[13]-=256;src=probe.reg[13]+0x50;dest=probe.base+0x714
 values=[d['material'],d['ground_height'],d['normal_x'],d['normal_y'],d['normal_z'],1,d['ground_contact_value'] if d['ground_contact_valid'] else 0]
 for j,value in enumerate(values):probe.write(src+4*j,value)
 probe.write(dest-4,0xabcdef12);probe.write(dest+28,0x1234abcd);probe.run(entry=0x8012536,stop=0x8012548)
 got=[probe.read(dest+4*j) for j in range(7)];expected=[d['output_snapshot_714_'+f]&MASK for f in ['material','height','normal_x','normal_y','normal_z','flags','contact_value']];assert got==expected,dict(tick=row[0],original=got,native=expected)
 assert probe.read(dest-4)==0xabcdef12 and probe.read(dest+28)==0x1234abcd;steps+=probe.steps
report=dict(cases=len(rows),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalworld2536..2546sevenwordcontactsnapshotcopy usingactualgroundcontactfixtures; matchescapturedsnapshot714andadjacentguards. ExercisesLDM/STMwriteback, not wholeworld/GBAframe.')
(root/'reports/bound-contact-snapshot-rom-verification.json').write_text(json.dumps(report,indent=2));print('PASS',report)
