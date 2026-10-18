"""Every original scenery call shares actual consumer stack and register state."""
from pathlib import Path
import json,struct,types
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_shared_normal_stack_scene.py').read_text().split('\np=SharedNormalScene();')[0])
near_namespace={};exec((root/'tools/verify_source240_near_dispatch_return.py').read_text().split('\nj=json')[0],near_namespace)
near_source=near_namespace['s'].replace('pc!=0xffffffff','pc!=self.return_pc').replace("elif m=='bx':nextpc=self.val(p[0])","elif m=='bx':nextpc=self.val(p[0])&~1")
exec(near_source,near_namespace['ns']);near_namespace['ns']['BASE']=0x08000700;near_namespace['ns']['arm'].detail=True
class SharedAllScene(SharedNormalScene):
 def __init__(self):
  super().__init__();self.shared_near_calls=[]
  for off,value in [(24,120),(28,80),(40,192),(44,256)]:self.write(0x03006810+off,value)
 def raster_request(self,callback):
  if callback==background_target:return super().raster_request(callback)
  idx=(self.reg[1]-16-0x02009eec)//144;packet=packets[idx]
  if not packet['raster_flags']&1:return super().raster_request(callback)
  self.actual_targets.append(callback);return_address=QueueProbe.raster_request(self,0x0300691c)
  target=self.read(0x03006920);assert target==installed[int(packet['callback'],16)]
  wrapper=near_namespace['NearReturn'](pool[idx*144:(idx+1)*144],target);wrapper.reg=self.reg.copy();wrapper.return_pc=wrapper.reg[14]&~1;wrapper.n,wrapper.z,wrapper.c,wrapper.v=self.n,self.z,self.c,self.v;before=wrapper.reg[4:15].copy()
  original_near_read=wrapper.read;original_near_write=wrapper.write
  def is_shared(a,n):return 0x1000f000<=a<a+n<=0x10010000 or 0x02009cec<=a<a+n<=0x02009eec+len(pool) or 0x03006810<=a<a+n<=0x03006b00
  def near_read(obj,a,n=4):
   if a==0x080049e8:return 0x03006818 # Explicit viewport data literal relocation; do not overwrite callback slots.
   return self.read(a,n) if is_shared(a,n) else original_near_read(a,n)
  def near_write(obj,a,v,n=4):return self.write(a,v,n) if is_shared(a,n) else original_near_write(a,v,n)
  wrapper.read=types.MethodType(near_read,wrapper);wrapper.write=types.MethodType(near_write,wrapper)
  def material_call(obj):
   f=families[packet['callback']];cls,run,base=normal_runs[f];pattern=struct.unpack_from('<H',data,mb+packet['material']*28+14)[0]
   q=cls(packet,pattern if f==0 else textures[packet['texture']]);q.reg=obj.reg.copy();q.return_pc=q.reg[14]&~1;q.n,q.z,q.c,q.v=obj.n,obj.z,obj.c,obj.v;saved=q.reg[4:15].copy()
   original_read=q.read;original_write=q.write;descriptor=q.reg[2];q.descriptor_byte_reads=0
   def read(obj2,a,n=4):
    if is_shared(a,n):return self.read(a,n)
    if n==1 and descriptor<=a<descriptor+12:obj2.descriptor_byte_reads+=1
    return original_read(a,n)
   def write(obj2,a,v,n=4):return self.write(a,v,n) if is_shared(a,n) else original_write(a,v,n)
   q.read=types.MethodType(read,q);q.write=types.MethodType(write,q)
   for address,buf in q.blocks:
    if address==base:buf[:]=bytes(self.shared[a] for a in range(0x10080000,0x10089600))
    if address==0x020344f0:buf[:]=self.edges[:len(buf)]
   obj.pixels=run(q);assert q.reg[4:15]==saved
   for address,buf in q.blocks:
    if address==0x020344f0:self.edges=bytes(buf)+self.edges[len(buf):]
   obj.reg=q.reg.copy();obj.n,obj.z,obj.c,obj.v=q.n,q.z,q.c,q.v;obj.raster=q
   return q.return_pc
  wrapper.material_call=types.MethodType(material_call,wrapper)
  near_namespace['ns']['MarkerRasterProbe'].run(wrapper);assert wrapper.reg[4:15]==before
  q=wrapper.raster;f=families[packet['callback']];counts=[idx,0,0,q.half,q.word] if f==0 else [idx,q.reads-q.descriptor_byte_reads,q.writes,0,0];seq=len(self.requests)-1
  assert wrapper.pixels+struct.pack('<5I',*counts)==cp.stdout[seq*38420:(seq+1)*38420],(seq,idx)
  for k in range(4):self.counts[k]+=counts[k+1]
  for i,b in enumerate(wrapper.pixels):self.shared[0x10080000+i]=b
  self.reg=wrapper.reg.copy();self.n,self.z,self.c,self.v=wrapper.n,wrapper.z,wrapper.c,wrapper.v;self.near_steps+=wrapper.steps;self.raster_steps+=q.steps;self.descriptor_reads+=q.descriptor_byte_reads
  self.shared_near_calls.append({'source_index':packet['source_index'],'caller_sp':f'{before[9]:08x}','saved_state_restored':True})
  assert wrapper.return_pc==return_address
  return wrapper.return_pc
p=SharedAllScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.shared_near_calls)==3 and p.counts==[8660,8006,73,2121]
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'shared_near_calls':p.shared_near_calls,'shared_normal_calls':30,'shared_sprite_calls':4,'counts':p.counts,'descriptor_byte_reads':p.descriptor_reads,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'All37 originalscenerycalls useactualconsumerstack/packetmemory/ROMdescriptors/registers/NZCV andactualcallee scratchreturns. Threenearwrappers useactualengine globals/materialowner andsameconsumerstack fornestedraster. No callerpreservingcontract. Code/frame/bounds/edgebackingfixture relocation remains; excludesactors/palette/fullframe/cycles/cadence.'}
(root/'reports/source240-shared-all-stack-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
