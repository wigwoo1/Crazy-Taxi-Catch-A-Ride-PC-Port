"""Original driver postmovement timer/boost tail; synthetic boundary inputs."""
exec(open('outputs/crazy-taxi-native/tools/compare_action_rom.py').read().split('\ncases=')[0])
for name in ['driver-loop']:
 cfg=json.loads((root/('reports/challenge-bound-'+name+'-cfg.json')).read_text());allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
class TailProbe(ActionProbe):
 def external(self,target):
  assert target==0x8015d98;self.run(entry=target,stop=self.reg[14]&~1)
cases=[(a,b,c,scale,delta,x,y) for a,b,c in [(0,0,0),(1,2,3),(100,50,10),(-1,-10,0)] for scale in [0,4095,4096,4097,4710,5324] for delta in [1,2,4] for x,y in [(0,0),(1,0),(0,1),(1,1)]]
res=subprocess.run([str(root/'bin/driver-tail-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 a,b,c,scale,delta,x,y=case;probe=TailProbe((0,0,0,0,0,1),0);probe.initialize((0,0,0,0,0,1,(x<<2)|(y<<3),0));probe.arm.mem[0x2003b00]=delta;probe.reg[7]=probe.base+0xe78
 for off,val in [(0xe78,a),(0xe7c,b),(0xe80,c),(0xe84,scale)]:probe.write(probe.base+off,val)
 probe.run(entry=0x8011ea6,stop=0x8011f32);got=[signed(probe.read(probe.base+off)) for off in [0xe78,0xe7c,0xe80,0xe84]];assert got==row,dict(case=case,original=got,native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalreachable11EA6..11F30 plusoriginalactionquery overridebranch vsnativepostmovementtail; synthetictimer/scale/clamp/actions. Not yet integrated into playable driver or fulltrajectory.')
(root/'reports/driver-tail-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
