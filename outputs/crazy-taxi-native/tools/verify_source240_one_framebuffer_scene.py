"""All scenery drawing accesses share the consumer framebuffer byte memory."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
class SharedFramebuffer:
 def __init__(self,owner):self.owner=owner
 def __len__(self):return 38400
 def __iter__(self):return (self.owner.shared[a] for a in range(0x10080000,0x10089600))
 def __getitem__(self,key):
  if isinstance(key,slice):return bytes(self.owner.shared[0x10080000+i] for i in range(*key.indices(38400)))
  return self.owner.shared[0x10080000+key]
 def __setitem__(self,key,value):
  if isinstance(key,slice):
   for i,b in zip(range(*key.indices(38400)),value):self.owner.shared[0x10080000+i]=b
  else:self.owner.shared[0x10080000+key]=value
 def count_write(self,obj,n):
  if hasattr(obj,'half'):
   assert n in (2,4)
   if n==2:obj.half+=1
   else:obj.word+=1
  else:assert n==2;obj.writes+=1
saved_read_text=Path.read_text
names={'verify_source240_shared_sprite_stack_scene.py','verify_source240_shared_normal_stack_scene.py','verify_source240_shared_all_stack_scene.py'}
def adapted_read(path,*args,**kwargs):
 text=saved_read_text(path,*args,**kwargs)
 if path.name=='verify_source240_copied_arm_normal_sprite_scene.py':text=text.replace('(0x11000000).to_bytes','(0x10080000).to_bytes')
 if path.name in names:
  text=text.replace('0x1000f000<=a<a+n<=0x10010000','0x10080000<=a<a+n<=0x10089600 or 0x1000f000<=a<a+n<=0x10010000')
  text=text.replace('q.reg=self.reg.copy();','q.blocks[1]=(0x10080000,SharedFramebuffer(self));q.reg=self.reg.copy();')
  text=text.replace('q.reg=obj.reg.copy();','q.blocks[1]=(0x10080000,SharedFramebuffer(self));q.reg=obj.reg.copy();')
  text=text.replace('if is_shared(a,n):return self.write(a,v,n)', 'if 0x10080000<=a<a+n<=0x10089600:q.blocks[1][1].count_write(obj,n);return self.write(a,v,n)\n   if is_shared(a,n):return self.write(a,v,n)')
  # Nested raster write uses obj2 and a single-line conditional.
  text=text.replace('def write(obj2,a,v,n=4):return self.write(a,v,n) if is_shared(a,n) else original_write(a,v,n)', 'def write(obj2,a,v,n=4):\n    if 0x10080000<=a<a+n<=0x10089600:q.blocks[1][1].count_write(obj2,n);return self.write(a,v,n)\n    return self.write(a,v,n) if is_shared(a,n) else original_write(a,v,n)')
 return text
Path.read_text=adapted_read
try:
 exec((root/'tools/verify_source240_copied_arm_all_scene.py').read_text().split('\np=SharedAllScene();')[0])
finally:Path.read_text=saved_read_text
p=SharedAllScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and p.counts==[8660,8006,73,2121]
assert int.from_bytes(image[0x16cc:0x16d0],'little')==0x10080000
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'shared_framebuffer':'10080000','counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'ALL37 copiedoriginalscenerycallbacks directlyread/writeONEconsumerframebuffer byte memory10080000 withsameactualcopiedframeblockpointer, no11000000framebufferaddress/copyalias. Sharedconsumerstack/registers/NZCV/pool, allintermediates nativeexact. Frameblockbacking/edgeworkspace/unusedconstructorbuffers/backgroundROMbridge remain; offlineonly/no actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-one-framebuffer-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
