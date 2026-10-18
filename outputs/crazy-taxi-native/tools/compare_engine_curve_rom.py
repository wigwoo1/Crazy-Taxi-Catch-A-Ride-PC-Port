"""Original inline acceleration engine-curve interpolation."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-acceleration-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
cases=[(rpm,gear,up,scale) for rpm in [0,1,999,1000,1001,5499,14000,14999] for gear in [1,2,7] for up in [2000,6500,15000] for scale in [3584,4096]]
res=subprocess.run([str(root/'bin/engine-curve-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 rpm,gear,up,scale=case;probe=MotionProbe((0,0,0,0,0,1),0);probe.reg[5]=probe.base
 for off,v,n in [(0x538,rpm,2),(0x4dc,gear,1),(0x534,up,2),(0x4b0,scale,4)]:probe.write(probe.base+off,v,n)
 for j in range(16):probe.write(probe.base+0x4e8+j*4,20+j*100)
 probe.run(entry=0x8013a38,stop=0x8013ab4);assert row[0]==0 and probe.reg[6]==row[1],dict(case=case,original=probe.reg[6],native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalinline137DC curvepath13A38..13AB2 againstnative enginecurve; gear1 upshift bucket and signedrpm/fixedpoint interpolation/scale with explicitdivisioncontract. Synthetic16pointcurve and validbucketinputs; notfullacceleration/frame.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/compare_engine_curve_rom.py',root/'tests/engine_curve_rom_compare.c',root/'src/engine_curve.c',root/'bin/engine-curve-rom-compare.exe',root/'reports/challenge-bound-acceleration-cfg.json']}
(root/'reports/engine-curve-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
