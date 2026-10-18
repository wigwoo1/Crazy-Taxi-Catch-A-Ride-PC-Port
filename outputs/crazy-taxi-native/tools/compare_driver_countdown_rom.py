"""Original shared driving countdown11A4A..11A5A."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-driver-loop-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
class CountdownProbe(MotionProbe):
 def write(self,a,v,n=4):
  if a==0x2002630:self.arm.mem[a]=v&MASK
  else:super().write(a,v,n)
cases=[(state,delta) for state in [-2147483648,-1,0,1,2,45,2147483647] for delta in [0,1,2,4]]
res=subprocess.run([str(root/'bin/driver-countdown-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=list(map(int,res.stdout.split()));assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 state,delta=case;probe=CountdownProbe((0,0,0,0,0,1),0);probe.arm.mem[0x2002630]=state&MASK;probe.arm.mem[0x2003b00]=delta;probe.run(entry=0x8011a4a,stop=0x8011a5c);assert signed(probe.read(0x2002630))==row,(case,row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Original11A4A..11A5A shared2630 countdown with signedwrap/clamp; singleplayerhost ownership must runonce per drivingframe.')
(root/'reports/driver-countdown-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
