"""Original BC04 replacement and full27FB8/28EE0 against native owned graphics."""
from pathlib import Path
import json,itertools,subprocess,struct,hashlib
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/verify_bound_result_resource_input_shadow_rom.py').read_text().split('\nrows=[]')[0],ns)
base=ns['ns'] if 'ns' in ns else ns
for a,b in [(0xbc04,0xbca6),(0x28ee0,0x28f20)]:
 for i in base['md'].disasm(base['r'][a:b],0x8000000+a):base['allowed_thumb'][i.address]=bytes(i.bytes)
class Probe(ns['Probe']):
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   src,dst=self.read(0x40000d4),self.read(0x40000d8)
   if v==0x80000010:n=32
   else:assert v>>16==0x8400; n=(v&65535)*4
   data=bytes(self.read(src+j,1) for j in range(n))
   for j,b in enumerate(data):super().write(dst+j,b,1)
cases=list(itertools.product(range(3),range(5)))
rows=subprocess.run([str(root/'bin/start-art-owned-compare.exe')],input=''.join(f'{a} {b}\n' for a,b in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==15
bases=[struct.unpack_from('<I',(root/'reports/ordinary-native-assets.bin').read_bytes(),j*8)[0] for j in range(11)];steps=0
for (phase,lang),row in zip(cases,rows):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray([165])*262144,0x3000000:bytearray([165])*65536,0x5000000:bytearray([165])*1024,0x6000000:bytearray([165])*131072};p.scalars={};p.requests=[]
 p.write(0x2007e38,0x8070e50);p.write(0x20094ec,0,2);p.write(0x20016b4,phase,2);p.write(0x200388d,lang,1)
 for j,b in enumerate(bases):p.write(0x2007e3c+j*4,b)
 p.reg[7]=0x20016b4;p.reg[6]=0;p.reg[13]=0x10000400;p.run(entry=0x800bc04,stop=0x800bca6)
 assert p.read(0x20094ec,2)==3
 assert bytes(p.read(0x6014000+j,1) for j in range(32768))==bytes([165])*32768
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8028ee0,stop=0x10020000)
 assert p.read(0x20094ec,2)==0
 pixels,objects,palette,pointer=map(bytes.fromhex,row.split())
 assert objects==bytes(p.read(0x6014000+j,1) for j in range(32768)),(phase,lang,'objects')
 assert palette==bytes(p.read(0x5000360+j,1) for j in range(32)),(phase,lang,'palette')
 assert pointer==bytes(p.read(0x200187c+j,1) for j in range(4)),(phase,lang,'registry')
 steps+=p.steps
report=dict(status='PASS_ORIGINAL_START_ART_OWNED_BYTES',cases=15,thumb_instructions=steps,object_bytes_compared=15*32768,scope='OriginalBC04 plus full27FB8 and28EE0 execute to returns; allnative32768objectbytes,32bank11palettebytes andregistry11pointer exact. Original queue3 deferred/noobjectmutation thenflush0 asserted. DMA logical CPUbytecopies; suppliedcallerframe/groupbases/A5priorfixture; OAMshader/display/fullstartup excluded.')
(root/'reports/start-art-owned-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
