"""Controlled native reward producer comparison against original2901C."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
saved=allowed_thumb.copy();source=source.replace("'bne':not z,","'bne':not z,'bls':not c or z,");exec(source);allowed_thumb.update(saved);IdleProbe.run=MotionProbe.run
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/reward-cfg.json').read_text())['instructions']})
class RewardProbe(IdleProbe):
 def external(self,target):
  if target==0x8032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK
  elif target==0x80267c8:assert self.reg[1:3]==[0,0];self.sound=self.reg[0]
  else:super().external(target)
import itertools
cases=[]
for i,(kind,active,gate,combo,pattern,money) in enumerate(itertools.product([0,1],[0,1],[0,1],[0,1,254,255],range(4),[(0,0),(0,25),(0,26),(0,50),(0,51),(0,76),(1,99),(-2,-1)])):
 whole,fraction=money;cases.append((kind,active,gate,combo,128*((i//4)%2),(i//2)%2,0 if i%3==0 else (12345*i)&MASK,7,whole,fraction,123,45,pattern))
res=subprocess.run([str(root/'bin/reward-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases)
steps=created=0
for case,row in zip(cases,rows):
 kind,active,gate,combo,mode,sound,seed,voice,whole,fraction,total,frac,pattern=case
 p=RewardProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;p.sound=None
 for a,v,n in [(0x2001694,active,1),(0x2009aa4,gate,1),(0x2009aa6,combo,1),(0x2001eb0,mode,1),(0x2007602,sound,1),(0x20025d6,voice,1),(0x2003b04,seed,4),(0x2009aa8,total,4),(0x2009aac,frac,4)]:p.write(a,v,n)
 for i in range(10):
  for j in range(56):p.write(0x2009abc+i*56+j,0xa5,1)
  age=0 if pattern==0 else (0 if i==4 else 64) if pattern==1 else 64-i if pattern==2 else 128+i;p.write(0x2009abc+i*56+2,age,1)
 p.reg[:3]=[kind,whole&MASK,fraction&MASK];p.run(entry=0x802901c,stop=0x10020000)
 got=[p.read(0x2009aa6,1),p.read(0x20025d6,1),p.read(0x2003b04),signed(p.read(0x2009aa8)),signed(p.read(0x2009aac))]
 assert got==row[:5],dict(case=case,original=got,native=row[:5])
 assert [p.read(0x2009abc+i,1) for i in range(560)]==row[8:],case
 assert p.calls==row[7]
 expected_sound=None if row[6]<0 else struct.unpack_from('<I',r,0x7ec658+row[6]*4)[0];assert p.sound==expected_sound,case
 steps+=p.steps;created+=row[5]>=0
report=dict(cases=len(cases),created=created,thumb_instructions=steps,match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Originalcomplete2901C throughreturn, nestedoriginal1E72C vsnative reward module: combo/totals/voice/RNG/all560tipbytes/audio request pointer/callcount exact oncontrolledfixtures. Gates,combo saturation,free/full/signed-age slots,settings/sound,money thresholds. Signeddivision contract; original audio submission not executed. No gameplay integration/framegate/tip animation proof.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['tools/compare_reward_rom.py','src/reward.c','src/reward.h','tests/reward_rom_compare.c','bin/reward-rom-compare.exe','reports/reward-cfg.json']}
(root/'reports/reward-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
