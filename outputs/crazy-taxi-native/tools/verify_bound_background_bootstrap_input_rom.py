from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/audit_graphics_bootstrap_cold_source.py').read_text().split('\np=BootstrapProbe')[0],ns)
class Probe(ns['BootstrapProbe']):
 def write(self,a,v,size=4):
  if getattr(self,'observing',False):assert not(a<0x200351e and a+size>0x20034dc),('bootstrap input store',hex(a),size)
  super().write(a,v,size)
rows=[]
for seed in [0,165,255]:
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768),0x4000000:bytearray(1024)};p.scalars={};p.stores=[];p.published_copy=False;p.calls=[];p.contracts=[]
 for off in range(66):p.write(0x20034dc+off,(seed+off*13)&255,1)
 before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.observing=True;p.write(0x2003899,165,1);p.reg[:4]=[0x87ebd98,0,0,240];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 for j,v in enumerate([160,256,96<<8]):p.write(0x10000400+j*4,v)
 p.run(entry=0x802ac6c,stop=0x10020000)
 assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]);assert not p.contracts;assert p.read(0x3006810)==0x30069c0
 rows.append({'seed':seed,'phase':'cold','instructions':p.steps,'calls':[hex(t) for t,args in p.calls]})
 start=p.steps;p.calls=[];p.contracts=[];before=bytes(p.regions[0x2000000][0x34dc:0x351e])
 p.reg[:4]=[0x87ebd98,0,0,240];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 for j,v in enumerate([160,256,96<<8]):p.write(0x10000400+j*4,v)
 p.run(entry=0x802ac6c,stop=0x10020000)
 assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]) and not p.contracts
 rows.append({'seed':seed,'phase':'second-call-with-retained-first-call-state','instructions':p.steps-start,'calls':[hex(t) for t,args in p.calls]})
report={'status':'PASS_ORIGINAL_BOUND_BACKGROUND_BOOTSTRAP_INPUT_RETAINED','cases':rows,'rom_sha256':hashlib.sha256(ns['r']).hexdigest(),'scope':'Whole2AC6C actualBoundscene087EBD98 andfar96<<8, coldallocator/otherRAMzero exceptinputsentinel, actualscene registration/copy/frame/viewport/table/clear paths;66inputretained/nooverlappingCPUstores. DMAIWRAMrawcopy logicalcontract; originalARM quotient via32454 dispatch boundedROMdiagnostic, signeddivide192registercontract. Secondcall reusesfirstcall state withoutreseeding. Arbitrary gameplaywarmowners/backgroundbitmap4/nonzeroshade/completebootstrap excluded.'}
(root/'reports/bound-background-bootstrap-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS Boundbackgroundinput',rows)
