"""Original postvehicle effect counter decay versus native boundary fixtures."""
exec(open('outputs/crazy-taxi-native/tools/compare_action_rom.py').read().split('\ncases=')[0])
cfg=json.loads((root/'reports/challenge-bound-driver-loop-cfg.json').read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class EffectProbe(ActionProbe):
 def write(self,a,v,n=4):
  if a==0x500029e:self.palette=v;return
  super().write(a,v,n)
cases=[(a,b,d,f) for a,b in [(0,0),(1,128),(128,1),(255,255),(3,4)] for d in [0,1,2,4,128,255,256,-1,-256,-2147483648,2147483647] for f in [0,1]]
res=subprocess.run([str(root/'bin/driver-effect-decay-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 a,b,d,f=case;probe=EffectProbe((0,0,0,0,0,1),0);probe.arm.mem[0x2001778]=f<<24;probe.arm.mem[0x30053c0]=d&MASK;probe.reg[10]=0x30053c0;probe.reg[5]=probe.base+0x4ac
 probe.write(probe.base+0x6a9,a,1);probe.write(probe.base+0x6aa,b,1);probe.run(entry=0x8011e38,stop=0x8011ea6)
 got=[probe.read(probe.base+off,1) for off in [0x6a9,0x6aa]];assert got==row,dict(case=case,original=got,native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original 11E38..11EA4 byte counters compared, including freeze and signed wrap boundaries. Palette writes captured only; no render equivalence claim.')
report['sha256']={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/driver_tail.h',root/'src/driver.c',root/'tests/driver_effect_decay_rom_compare.c',root/'bin/driver-effect-decay-rom-compare.exe',root/'tools/compare_driver_effect_decay_rom.py']}
(root/'reports/driver-effect-decay-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
