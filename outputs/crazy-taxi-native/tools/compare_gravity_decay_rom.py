"""Original gravity/contact-decay against native launch inputs, diagnostic only."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
allowed_thumb={}
for name in ['gravity','decay']:
 cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-'+name+'-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
root=pathlib.Path('outputs/crazy-taxi-native');schema=json.loads((root/'reports/bound-launch-world-state-schema.json').read_text())['fields'];rows=[list(map(int,l.split())) for l in (root/'reports/bound-launch-world-states.txt').read_text().splitlines()];cases=[]
for row in rows:
 d=dict(zip(schema,row[1:]));assert d['ground_hit']==1;cases.append((d['input_y'],d['gravity698'],d['speed544'],d['flags28'],d['delta']))
cases += [(y,g,122309,f,d) for y in [-100,0,3704] for g in [-12345,0,27720] for f in [0,8,16,24] for d in [1,2,4]]
res=subprocess.run(['outputs/crazy-taxi-native/bin/gravity-decay-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);outputs=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(outputs)==len(cases);steps=0
for case,expected in zip(cases,outputs):
 y,g,s,f,delta=case;probe=MotionProbe((0,0,0,0,0,delta),0)
 for off,value,n in [(4,y,4),(0x698,g,4),(0x544,s,4),(0x28,f,1),(0x40,123456,4),(0x44,-654321,4),(0x2c,123,2)]:probe.write(probe.base+off,value,n)
 for entry in [0x8015b00,0x8015b54]:
  probe.reg[0]=probe.base;probe.reg[14]=0x10020001;probe.run(entry=entry,stop=0x10020000)
 got=[signed(probe.read(probe.base+off)) for off in [4,0x698,0x544,0x40,0x44]]+[probe.read(probe.base+0x2c,2)]
 assert got==expected,dict(case=case,original=got,native=expected);steps+=probe.steps
report=dict(cases=len(cases),actual_launch_input_cases=len(rows),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original fullgravity15B00/contactdecay15B54 vsnative using161actuallaunchinputstates plus108syntheticedges. Division scalar contract; state40/44 fixtures. Not fullworld/height/groundqueryoracle.')
(root/'reports/gravity-decay-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
