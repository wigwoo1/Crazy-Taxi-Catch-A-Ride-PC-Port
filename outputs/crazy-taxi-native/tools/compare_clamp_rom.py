"""Original full136D4 RPM/speed clamp vsnative."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-clamp-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
cases=[(gear,rpm,speed,wheel) for gear in [0,1,2,7] for rpm in [-32768,0,799,800,8500,8501,32767] for speed in [-1500000,-122309,0,122309,1500000] for wheel in [-1500000,0,1500000]]
res=subprocess.run([str(root/'bin/clamp-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 gear,rpm,speed,wheel=case;probe=MotionProbe((0,0,0,0,0,1),0)
 for off,val,n in [(0x4dc,gear,1),(0x538,rpm,2),(0x544,speed,4),(0x568,wheel,4),(0x530,800,2),(0x532,8500,2),(0x4d4,4096,4),(0x4d8,4096,4),(0x5cc,8192,4)]:probe.write(probe.base+off,val,n)
 for i in range(7):probe.write(probe.base+0x4b8+i*4,4096+i*256)
 probe.reg[0]=probe.base;probe.reg[14]=0x10020001;probe.run(entry=0x80136d4,stop=0x10020000);rpm=probe.read(probe.base+0x538,2);got=[rpm-65536 if rpm&32768 else rpm]+[signed(probe.read(probe.base+off)) for off in [0x544,0x568]];assert row[0]==0 and got==row[1:],dict(case=case,original=got,native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete136D4 clamp vsnative across validgear/signedRPM/road-wheelspeed/neutral/min-max boundaries with explicitdivisioncontract; syntheticratios/circumference, notfullgear/driver/frame.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/compare_clamp_rom.py',root/'tests/clamp_rom_compare.c',root/'src/gears.c',root/'bin/clamp-rom-compare.exe',root/'reports/challenge-bound-clamp-cfg.json']}
(root/'reports/clamp-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
