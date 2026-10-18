"""Original RPM helper with diagnostic-only logging capture."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-rpm-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
class RpmProbe(MotionProbe):
 def external(self,target):
  if target==0x802c880:self.invalid=1
  else:super().external(target)
cases=[(speed,ratio,mode,boost,delta) for speed in [0,1,-1,34139,122309,-122309,1138000] for ratio in [0,2048,4096,-8192] for mode in [0,1,2,3] for boost in [-1,0,1,100,10000] for delta in [1,2]]
res=subprocess.run([str(root/'bin/rpm-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 speed,ratio,mode,boost,delta=case;probe=RpmProbe((0,0,0,0,0,delta),0);probe.invalid=0
 for off,v,n in [(0x544,speed,4),(0x4ac,mode<<2,1),(0x75c,boost,4),(0x5b8,4096,4),(0x5cc,8192,4)]:probe.write(probe.base+off,v,n)
 probe.reg[:4]=[probe.base,probe.base+0x4ac,ratio&MASK,0];probe.reg[14]=0x10020001;probe.run(entry=0x8013610,stop=0x10020000)
 rpm=probe.read(probe.base+0x538,2);rpm=rpm-65536 if rpm&32768 else rpm;got=[rpm,signed(probe.read(probe.base+0x75c)),probe.invalid];assert row[0]==0 and got==row[1:],dict(case=case,original=got,native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Fulloriginal13610 RPM r3=0 vsnative selectedratio helper; signed speed/ratio, drive modes, boostfloor/release/decay andhalfwordRPMwrap. Divisioncontract and2C880diagnosticlogcaptured; syntheticinputsinvaliddivisorsexcluded, nofullacceleration/frameclaim.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/compare_rpm_rom.py',root/'tests/rpm_rom_compare.c',root/'src/gears.c',root/'bin/rpm-rom-compare.exe',root/'reports/challenge-bound-rpm-cfg.json']}
(root/'reports/rpm-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
