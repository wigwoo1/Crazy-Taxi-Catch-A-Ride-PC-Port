"""Complete original1E72C random update, signed remainder explicit contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_smoke_rom.py').read().split('\nclock=')[0])
source=source.replace("['muls','ands','orrs']","['muls','ands','orrs','eors']").replace("a|b if m=='orrs' else a&b","a|b if m=='orrs' else (a^b if m=='eors' else a&b)")
exec(source);VehicleProbe.run=MotionProbe.run
allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/random-range-cfg.json').read_text())['instructions']}
class RandomProbe(VehicleProbe):
 def write(self,a,v,n=4):
  if a==0x2003b04:self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
 def external(self,target):
  assert target==0x8032520; a,b=map(signed,self.reg[:2]);self.reg[0]=(a-trunc(a,b)*b if b else 0)&MASK
cases=[(seed,lo,hi) for seed in [0,1,0x4652414e,0x3fffffff,0x40000000,0x7fffffff,0x80000000,0xffffffff] for lo,hi in [(0,65535),(-5,5),(4,8),(0,0),(-2147483648,2147483647),(2147483647,-2147483648),(-100,-200)]]
res=subprocess.run([str(root/'bin/random-range-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 seed,lo,hi=case;p=RandomProbe((0,0,0,0,0,1,0,0,0,0));p.arm.mem[0x2003b04]=seed;p.reg[0]=lo&MASK;p.reg[1]=hi&MASK;p.run(entry=0x801e72c,stop=0x10020000);got=[signed(p.reg[0]),p.read(0x2003b04)];assert got==row,dict(case=case,original=got,native=row);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal1E72C vsnative random_range value andupdatedstate; zero seed/polynomial/sign/spanwrap/zero divisor; signedremainder explicitcontract, not fullframe RNGownership.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/random.c',root/'tests/random_range_rom_compare.c',root/'tools/compare_random_range_rom.py',root/'bin/random-range-rom-compare.exe',root/'reports/random-range-cfg.json']}
(root/'reports/random-range-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
