"""Original body spring scalar state vs native, diagnostic only."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-springs-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
cases=[(p,v,-p,-v,a,b) for p in [-1000,-5,-1,0,1,5,1000] for v in [-100,-1,0,1,100] for a,b in [(4096,8192),(8192,16384),(16384,32768)]]
res=subprocess.run(['outputs/crazy-taxi-native/bin/springs-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 assert row[0]==0;probe=MotionProbe((0,0,0,0,0,1),0)
 for off,value in zip([0x734,0x73c,0x740,0x748,0x754,0x758],case):probe.write(probe.base+off,value)
 probe.reg[1]=probe.base;probe.reg[14]=0x10020001;probe.run(entry=0x801488c,stop=0x10020000);got=[signed(probe.read(probe.base+off)) for off in [0x734,0x73c,0x740,0x748,0x704,0x70c]];assert got==row[1:],dict(case=case,original=got,native=row[1:]);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalsprings1488C complete vsnative, syntheticcrossing/deadzone fixtures; signeddivisioncontract. Not actualfullpose/GBAframe.')
(root/'reports/springs-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
