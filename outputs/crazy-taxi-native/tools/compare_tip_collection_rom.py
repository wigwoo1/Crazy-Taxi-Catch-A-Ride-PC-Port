"""Original complete2939C collection with nestedC540/10830 vs native."""
exec(open('outputs/crazy-taxi-native/tools/compare_reward_update_rom.py').read().split('\nimport itertools')[0])
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/tip-collection-cfg.json').read_text())['instructions']})
class CollectProbe(UpdateProbe):
 def external(self,target):
  if target==0x800c540:
   assert self.reg[:2]==[2,MASK];self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
rows=[list(map(int,l.split())) for l in (root/'reports/current-reward-updates.txt').read_text().splitlines()]
cases=[(row[5],row[6],row[4],row[16:576]) for row in rows]
# Signed ages and fractions; inert bytes must survive and sums wrap32.
for pattern in range(4):
 tips=[165]*560
 for i in range(10):
  tips[i*56+2]=[0,64,128,255][pattern];tips[i*56+1]=(i*23)&255
  for j in range(4):tips[i*56+4+j]=((i+1)>>(j*8))&255
 cases.append((123,95,77,tips))
data=''.join(' '.join(map(str,[w,f,c]+t))+'\n' for w,f,c,t in cases)
result=subprocess.run([str(root/'bin/tip-collection-compare.exe')],input=data,text=True,capture_output=True,check=True);native=[list(map(int,l.split())) for l in result.stdout.splitlines()];assert len(native)==len(cases);steps=collected=0
for case,want in zip(cases,native):
 w,f,c,tips=case;p=CollectProbe((0,0,0,0,0,1,0,0,0,0))
 for a,v,n in [(0x20016a0,w,4),(0x20016a4,f,4),(0x2009aa7,c,1)]:p.write(a,v,n)
 for i in range(9):p.write(0x2005374+i*2,65535,2);p.write(0x2005360+i*2,65535,2)
 for i,v in enumerate(tips):p.write(0x2009abc+i,v,1)
 p.run(entry=0x802939c,stop=0x10020000)
 actual=[signed(p.read(0x20016a0)),signed(p.read(0x20016a4)),p.read(0x2009aa7,1)]+[p.read(a+j*2,2) for a in [0x2005374,0x2005360] for j in range(9)]+[p.read(0x2009abc+i,1) for i in range(560)]
 assert actual==want,dict(case=(w,f,c),mismatch=[(i,a,b) for i,(a,b) in enumerate(zip(actual,want)) if a!=b][:10]);steps+=p.steps;collected+=sum(bool(tips[i*56+2]) for i in range(10))
report=dict(cases=len(cases),collected_records=collected,thumb_instructions=steps,match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Completeoriginal2939C+nestedC540mode2/10830: pendingcash/count/560tipbytes/current+target9glyphs vsnative.296actualactive states plus4controlledsignedage fixtures. Explicit formatting/math contracts; invocation/frameorder/pixels notproved.')
(root/'reports/tip-collection-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
