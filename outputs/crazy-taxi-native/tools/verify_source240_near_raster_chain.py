"""Original clipping/drawing/return sharing the wrapper stack and register state."""
from pathlib import Path
import json,re,struct,hashlib,types
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_near_dispatch_return.py').read_text().split('\nj=json')[0])
j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures']
requests=json.loads((root/'reports/source240-joined-render-requests-verification.json').read_text())['requests'];args={r['payload']:r['arguments'] for r in requests}
producer={};exec((root/'tools/verify_source240_continuous_scene_producer.py').read_text().split('steps=0;j=json')[0],producer);producer['steps']=0
o=producer['original']([0,[0]*425,*j['camera'],j['yaw'],j['pitch']]);pool=bytes.fromhex(o['packet'])
probes={}
for name,end,oldend in [('flat',0x1f94,0x1f88),('perspective',0x1044,0x1038)]:
 scope={};exec((root/'tools'/('verify_source240_full_'+name+'_polygon.py')).read_text().split('\nj=json')[0],scope);cls=scope['Probe'];s=scope['s'];s,n=re.subn(r'while pc!=BASE\+0x[0-9a-f]+-0x700:', 'while pc!=self.return_pc:',s);assert n==1
 s=s.replace('<'+hex(oldend),'<'+hex(end));exec(s,scope);scope['arm'].detail=True;probes[name]=(cls,scope['MarkerRasterProbe'].run)
class JoinedNear(NearReturn):
 def __init__(self,raw,target,packet):super().__init__(raw,target);self.packet=packet;self.raster=None
 def material_call(self):
  self.calls.append({'arguments':self.reg[:4].copy(),'sp':self.reg[13],'lr':self.reg[14]})
  name='flat' if self.packet['callback']=='03006950' else 'perspective';cls,run=probes[name]
  value=int.from_bytes(ns['r'][self.reg[3]-0x08000000:self.reg[3]-0x08000000+2],'little') if name=='flat' else textures[self.packet['texture']]
  q=cls(self.packet,value);q.blocks=[b for b in q.blocks if b[0]!=0x13000000]+[b for b in self.blocks if b[0]==0x13000000]
  q.reg=self.reg.copy();q.return_pc=self.reg[14];q.n,q.z,q.c,q.v=self.n,self.z,self.c,self.v
  q.descriptor_byte_reads=0
  original_read=q.read;descriptor=self.reg[2]
  def counted_read(obj,a,n=4):
   if n==1 and descriptor<=a<descriptor+12:obj.descriptor_byte_reads+=1
   return original_read(a,n)
  q.read=types.MethodType(counted_read,q)
  before=q.reg[4:15].copy();pixels=run(q);assert q.reg[4:15]==before
  self.reg=q.reg.copy();self.n,self.z,self.c,self.v=q.n,q.z,q.c,q.v;self.raster=q;self.pixels=pixels
  return q.return_pc
bindings=json.loads((root/'reports/renderer-installed-targets-verification.json').read_text());rows=[]
for idx,packet in enumerate(j['packets']):
 if not packet['raster_flags']&1:continue
 target=next(int(x['installed_value'],16) for x in bindings['installed_slots'] if x['slot']==packet['callback'])
 q=JoinedNear(pool[idx*144:(idx+1)*144],target,packet);saved=[0xd6100000+k*0x10203 for k in range(9)];q.reg[4:13]=saved;sp=q.reg[13];q.reg[2:4]=args[idx][2:4];q.run()
 assert q.reg[4:13]==saved and q.reg[13]==sp and q.reg[14]==0xffffffff
 name='flat' if packet['callback']=='03006950' else 'perspective';cls,run=probes[name]
 value=int.from_bytes(ns['r'][args[idx][3]-0x08000000:args[idx][3]-0x08000000+2],'little') if name=='flat' else textures[packet['texture']]
 expected=cls(packet,value);expected.return_pc=0xffffffff;pixels=run(expected);assert pixels==q.pixels
 counts=[q.raster.half,q.raster.word] if name=='flat' else [q.raster.reads-q.raster.descriptor_byte_reads,q.raster.writes]
 expected_counts=[expected.half,expected.word] if name=='flat' else [expected.reads,expected.writes];assert counts==expected_counts,(packet['source_index'],counts,expected_counts)
 rows.append({'source_index':packet['source_index'],'wrapper_instructions':q.steps,'raster_instructions':q.raster.steps,'counts':counts,'descriptor_byte_reads':q.raster.descriptor_byte_reads,'framebuffer_sha256':hashlib.sha256(pixels).hexdigest(),'arguments':q.calls[0]['arguments']})
assert len(rows)==3
report={'status':'PASS','packets':rows,'scope':'Actual near wrapper4424 materialdispatch and full original normalrasterbody/epilogue thenwrapper46F8return, shared originalstack and actualregister/flag scratchresults. Originalconsumer arguments, actualROMtexture/material, seededcallee registers/SP/LR restored. Pixels/work exact previously native-qualified prepared-raster path. Raster code/frame/bounds remain fixture relocated; fullconsumer sharedmemory integration/actors/palette/cycles/cadence not proved.'}
(root/'reports/source240-near-raster-chain-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
