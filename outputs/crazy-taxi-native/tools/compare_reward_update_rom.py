"""Original29264 throughreturn with originalC540mode2/10830 nested."""
exec(open('outputs/crazy-taxi-native/tools/compare_pending_fare_glyph_live_rom.py').read().split('\nrows=')[0])
saved=allowed_thumb.copy();source=source.replace("'bne':not z,","'bne':not z,'bls':not c or z,");exec(source);allowed_thumb.update(saved);GlyphProbe.run=MotionProbe.run
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in json.loads((root/'reports/reward-update-cfg.json').read_text())['instructions']})
class UpdateProbe(GlyphProbe):
 def external(self,target):
  if target==0x800c540:
   assert self.reg[:2]==[2,MASK];self.expired+=1;self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x80267c8:
   assert self.reg[0]==struct.unpack_from('<I',r,0x7ec720)[0] and self.reg[1:3]==[0,0];self.sounds+=1
  else:super().external(target)
import itertools
cases=list(itertools.product([0,38,39,40,127,128,255],[-1,0,1,2,10,40,64,128,255],range(4),[0,1]))
res=subprocess.run([str(root/'bin/reward-update-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,l.split())) for l in res.stdout.splitlines()];assert len(rows)==len(cases)
steps=expirations=0;glyph_failures=[]
for case,row in zip(cases,rows):
 tick,delta,pattern,sound=case;p=UpdateProbe((0,0,0,0,0,1,0,0,0,0));p.expired=p.sounds=0
 for a,v,n in [(0x2009aa4,7,1),(0x2009aa5,tick,1),(0x2009aa7,77,1),(0x2003b00,delta,4),(0x2007602,sound,1),(0x20016a0,123,4),(0x20016a4,95,4)]:p.write(a,v,n)
 for i in range(9):p.write(0x2005374+2*i,65535,2)
 for i in range(10):
  a=0x2009abc+i*56
  for j in range(56):p.write(a+j,165,1)
  p.write(a+2,0 if pattern==0 else 64 if pattern==1 else i+1 if pattern==2 else 128+i,1);p.write(a+1,i*23,1)
  for off,v in [(4,i),(32,30720),(36,20480),(8,28672),(12,35840),(44,288),(48,272),(20,0),(24,-496)]:p.write(a+off,v,4)
 p.run(entry=0x8029264,stop=0x10020000)
 got=[p.read(0x2009aa4,1),p.read(0x2009aa5,1),p.read(0x2009aa7,1),signed(p.read(0x20016a0)),signed(p.read(0x20016a4)),p.expired,p.sounds]+[p.read(0x2005374+2*i,2) for i in range(9)]+[p.read(0x2009abc+i,1) for i in range(560)]
 assert got[:7]==row[:7] and got[16:]==row[16:],dict(case=case,mismatch=[(i,a,b) for i,(a,b) in enumerate(zip(got,row)) if a!=b][:12])
 if got[7:16]!=row[7:16]:glyph_failures.append(dict(case=case,original=got[7:16],native=row[7:16],pending_fraction=got[4]))
 steps+=p.steps;expirations+=p.expired
report=dict(cases=len(cases),tip_expirations=expirations,thumb_instructions=steps,match=not glyph_failures,glyph_mismatches=glyph_failures,numeric_and_tip_records_match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original29264 nestedcompleteC540mode2/10830: gate/counter/activecount/pendingmoney/9pendingtargets/all560tipbytes and soundrequests exact oncontrolledfixtures. Explicit sprintf/strlen/division contracts; no productionframe integration or spritepixel proof.')
report['sha256']={str(root/f):hashlib.sha256((root/f).read_bytes()).hexdigest() for f in ['src/reward_update.c','src/reward_update.h','src/fare.c','tests/reward_update_rom_compare.c','tools/compare_reward_update_rom.py','reports/reward-update-cfg.json','bin/reward-update-rom-compare.exe']}
(root/'reports/reward-update-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS' if report['match'] else 'PARTIAL: glyph mismatch', {k:v for k,v in report.items() if k!='glyph_mismatches'});print('Glyph mismatch count',len(glyph_failures))
