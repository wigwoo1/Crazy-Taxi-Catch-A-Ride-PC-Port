"""Original final vehicle response, no external contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_bound_motion_steering.py').read().split('\ncases=[')[0])
source=source.replace("'bgt':not z and n==v}","'bgt':not z and n==v,'blt':n!=v}");exec(source)
cfg=json.loads(pathlib.Path('outputs/crazy-taxi-native/reports/challenge-bound-response-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
import random
rng=random.Random(20261005)
values=[-2147483648,-2147483647,-10241,-10240,-10239,-5,-4,-3,-1,0,1,3,4,5,10239,10240,10241,2147483647]
cases=[(f,r,flag) for f in values for r in values for flag in [0,16]]+[(rng.randint(-2147483648,2147483647),rng.randint(-2147483648,2147483647),rng.choice([0,16])) for _ in range(2000)]
res=subprocess.run(['outputs/crazy-taxi-native/bin/response-rom-compare.exe'],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 probe=MotionProbe((0,0,0,0,0,1),0);probe.reg[14]=0x10020001
 for off,value,n in [(0x540,case[0],4),(0x744,case[1],4),(0x4ac,case[2],1)]:probe.write(probe.base+off,value,n)
 probe.run(entry=0x8015bb8,stop=0x10020000);got=[signed(probe.read(probe.base+x)) for x in [0x540,0x744]];assert got==row,dict(case=case,original=got,native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original response08015BB8 execution vs native response; no external calls. Includes INT_MIN,clamp and rounding edges. Not full vehicle/world frame.')
pathlib.Path('outputs/crazy-taxi-native/reports/response-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
