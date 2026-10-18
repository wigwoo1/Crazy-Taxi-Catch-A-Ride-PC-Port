"""Original near-wrapper dispatch/return with explicit material-call boundary."""
from pathlib import Path
import json,struct
root=Path('outputs/crazy-taxi-native')
ns={};exec((root/'tools/verify_source240_near_consumer.py').read_text().split('steps=0;j=json')[0],ns)
s=ns['src'].replace('pc!=0x080046d8','pc!=0xffffffff')
s=s.replace('   off=pc-BASE+0x700','   if pc==self.material_target:\n    pc=self.material_call();continue\n   off=pc-BASE+0x700')
exec(s,ns);ns['BASE']=0x08000700;ns['arm'].detail=True
old=ns['NearClip'];oldrun=old.run
class NearReturn(old):
 run=ns['MarkerRasterProbe'].run
 def __init__(self,raw,target):
  super().__init__(raw);self.material_target=target;self.calls=[];self.write(0x03006920,target)
 def material_call(self):
  self.calls.append({'target':f'{self.material_target:08x}','arguments':self.reg[:4].copy(),'sp':self.reg[13],'lr':self.reg[14]})
  return self.reg[14] # Explicit material call contract; original wrapper returns still executed.
j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());producer={};exec((root/'tools/verify_source240_continuous_scene_producer.py').read_text().split('steps=0;j=json')[0],producer);producer['steps']=0
o=producer['original']([0,[0]*425,*j['camera'],j['yaw'],j['pitch']]);pool=bytes.fromhex(o['packet'])
rows=[]
for idx,packet in enumerate(j['packets']):
 if not packet['raster_flags']&1:continue
 raw=pool[idx*144:(idx+1)*144]
 bindings=json.loads((root/'reports/renderer-installed-targets-verification.json').read_text());slot=packet['callback'].lower()
 target=next(int(x['installed_value'],16) for x in bindings['installed_slots'] if x['slot']==slot)
 before=old(raw);oldrun(before);expected_n=before.reg[8];ptr=before.reg[13]+0x94;expected=[before.read(ptr+i*4) for i in range(expected_n*8)]
 q=NearReturn(raw,target);saved=[0xc5100000+k*0x20304 for k in range(9)];q.reg[4:13]=saved;sp=q.reg[13];lr=q.reg[14];q.reg[2]=struct.unpack_from('<I',raw,12)[0];q.reg[3]=struct.unpack_from('<I',raw,8)[0]
 q.run();assert len(q.calls)==1 and q.reg[4:13]==saved and q.reg[13]==sp and q.reg[14]==lr
 call=q.calls[0];assert call['arguments'][:2]==[expected_n,ptr]
 assert [q.read(ptr+i*4) for i in range(expected_n*8)]==expected
 assert call['arguments'][2:]==[struct.unpack_from('<I',raw,12)[0],struct.unpack_from('<I',raw,8)[0]]
 rows.append({'source_index':packet['source_index'],'call':call,'original_arm_instructions':q.steps,'stack_and_saved_registers_restored':True})
assert len(rows)==3
report={'status':'PASS','near_packets':rows,'scope':'Original4424 wrapper through46D8 argument setup/indirect46F4 material call and46F8 epilogue/BXLR. Actualcoldinstalledmaterialtargets, originalproducerrawgeometry, count/vertex pointer/texture/materialargs andstack/R4-R12/LR exact. Materialcallee explicit no-mutation boundary contract, no joined raster pixels or exact callee scratch effects; relocationfixtures, no fullframe/cycles/cadence proof.'}
(root/'reports/source240-near-dispatch-return-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)

