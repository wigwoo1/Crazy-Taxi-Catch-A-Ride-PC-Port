"""Original BC04..BCA4 replacement/write requests vs compiled native helper."""
from pathlib import Path
import json,itertools,subprocess,types,hashlib
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/verify_bound_result_resource_input_shadow_rom.py').read_text().split('\nrows=[]')[0],ns)
base=ns['ns'] if 'ns' in ns else ns
# Probe's inherited instruction map belongs to its defining namespace.
for i in base['md'].disasm(base['r'][0xbc04:0xbca6],0x800bc04):base['allowed_thumb'][i.address]=bytes(i.bytes)
rompath='Crazy Taxi - Catch a Ride (USA).gba'
cases=list(itertools.product(range(3),range(5)))
cp=subprocess.run([str(root/'bin/start-art-replace-rom-compare.exe'),rompath],input=''.join(f'{a} {b}\n' for a,b in cases),text=True,capture_output=True,check=True)
outputs=cp.stdout.split('END\n');assert len(outputs)==16;steps=0
for case,out in zip(cases,outputs):
 phase,language=case;p=ns['Probe']((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(65536)};p.scalars={};events=[]
 p.write(0x20016b4,phase,2);p.write(0x200388d,language,1)
 old=p.write
 def write(self,a,v,n=4):
  if a in [0x40000d4,0x40000d8,0x40000dc,0x200187c]:events.append(['W',a,v&0xffffffff,n])
  old(a,v,n)
 def external(self,t):
  assert t==0x8027fb8,hex(t);events.append(['C',*self.reg[:4],self.read(self.reg[13])])
 p.write=types.MethodType(write,p);p.external=types.MethodType(external,p);p.reg[7]=0x20016b4;p.reg[6]=0;p.reg[13]=0x10000400
 p.run(entry=0x800bc04,stop=0x800bca6)
 native=[[l.split()[0],*map(int,l.split()[1:])] for l in out.splitlines()]
 assert events==native,(case,events,native);steps+=p.steps
report=dict(status='PASS_ORIGINAL_START_ART_REPLACEMENT_REQUESTS',cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(Path(rompath).read_bytes()).hexdigest(),scope='Original BC04..BCA4 exact ordered three deferred27FB8 requests, DMA source/destination/control and palette registry pointer writes across3phases/5languages. Caller r6=0 and phase pointer explicit fixtures; actual27FB8 effects/28EE0 flush/palette DMA hardware/pixels/full caller excluded.')
(root/'reports/start-art-replace-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
