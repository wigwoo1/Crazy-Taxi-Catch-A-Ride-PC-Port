from pathlib import Path
import itertools,json,struct,subprocess
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/verify_bound_result_resource_input_shadow_rom.py').read_text().split('\nrows=[]')[0],ns)
class Probe(ns['Probe']):
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   assert v>>16==0x8400
   src,dst=self.read(0x40000d4),self.read(0x40000d8);data=bytes(self.read(src+j,1) for j in range((v&65535)*4))
   for j,b in enumerate(data):super().write(dst+j,b,1)
cases=list(itertools.product((0,165,255),range(4),(0,1)));out=subprocess.run([str(root/'bin/hud-start-owned-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True,timeout=30).stdout.splitlines();assert len(out)==len(cases)
bases=[struct.unpack_from('<I',(root/'reports/ordinary-native-assets.bin').read_bytes(),j*8)[0] for j in range(11)];steps=0
for case,row in zip(cases,out):
 seed,driver,demo=case;p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray([seed])*262144,0x3000000:bytearray([seed])*65536,0x6000000:bytearray([seed])*131072};p.scalars={};p.requests=[]
 for a,v in ((0x200388d,driver),(0x2001539,driver),(0x2001690,driver),(0x2001708,demo),(0x2001eb0,128)):p.write(a,v,1)
 p.write(0x2007e38,0x8070e50);p.write(0x20094ec,0,2)
 for j,b in enumerate(bases):p.write(0x2007e3c+4*j,b)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x801ee28,stop=0x10020000)
 expected=b''.join(bytes(p.read(a+j,1) for j in range(n)) for a,n in ((0x20016c0,32),(0x2005300,256),(0x3005490,8),(0x2001790,2560),(0x6014000,32768)))
 parts=row.split();actual=bytes.fromhex(parts[0]);assert actual==expected,(case,len(actual),len(expected),next((j for j,(a,b) in enumerate(zip(actual,expected)) if a!=b),None))
 got=[p.read(0x20094ec,2)]+[p.read(a) for a in (0x40000d4,0x40000d8,0x40000dc)];assert got==list(map(int,parts[1:])),(case,got,parts[1:]);steps+=p.steps
report=dict(status='PASS',cases=len(cases),instructions=steps,owned_bytes=len(expected),scope='Whole original1EE28 and27FB8 plus1E784 versus compiled hud_start_owned using extracted CTG.24 Bound driver/demo/prior-byte fixtures; real groupbases from previously original-compared ordinary asset setup. All scalar/cache/descriptor/2560palette-record/32768objectbytes and final DMA request/copycount exact. DMA logical bytecopy; priorobjects deliberately seeded, not full live bootstrap/pixels/hardware.')
(root/'reports/hud-start-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
