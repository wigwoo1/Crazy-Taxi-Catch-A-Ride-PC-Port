"""Original vector-angle Thumb helper vs native lookup."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_motion_steering.py').read().split('\ncases=[')[0])
source=source.replace("'bgt':not z and n==v}","'bgt':not z and n==v,'blt':n!=v}")
exec(source)
cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-vector-angle-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
values=[-100000,-8192,-4096,-257,-256,-1,0,1,256,257,4096,8192,100000]
cases=[(x,z) for x in values for z in values]+[(x,256) for x in range(-256,257)]+[(256,z) for z in range(-256,257)]
res=subprocess.run(['outputs/crazy-taxi-native/bin/vector-angle-rom-compare.exe'],input=''.join(f'{x} {z}\n' for x,z in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 assert row[0]==0
 probe=MotionProbe((0,0,0,0,0,1),0);probe.reg[0]=case[0]&MASK;probe.reg[1]=case[1]&MASK;probe.reg[14]=0x10020001;probe.run(entry=0x802e00c,stop=0x10020000);assert signed(probe.reg[0])==row[1],dict(case=case,original=signed(probe.reg[0]),native=row[1]);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original vector-angle helper and direct original ROM lookup table, signed division scalar contract; compares native extracted angle table. Not whole traction/frame.')
pathlib.Path('outputs/crazy-taxi-native/reports/vector-angle-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
