"""Copied original background and scenery share one mutable frameblock owner."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_one_workspace_scene.py').read_text().split('\np=OneWorkspaceScene();')[0])
start=0x2fe0-0x700+8;end=0x31ec-0x700+8
arm_scope['code']={i.address:i for i in arm_scope['md'].disasm(bytes(image[start:end]),0x03000000+start)}
assert 'pc=0x08002fe0' in fn
copied_fn=fn.replace('pc=0x08002fe0','pc=0x030028e8');exec(copied_fn,arm_scope)
class OneFrameblockScene(OneWorkspaceScene):
 background=arm_scope['background']
 def read(self,a,size=4):
  if 0x02009100<=a<a+size<=0x02009110:a=0x030016cc+a-0x02009100
  if 0x03000000<=a<a+size<=0x03006810:return int.from_bytes(image[a-0x03000000:a-0x03000000+size],'little')
  return super().read(a,size)
 def write(self,a,value,size=4):
  if 0x02009100<=a<a+size<=0x02009110:a=0x030016cc+a-0x02009100
  if 0x03000000<=a<a+size<=0x03006810:image[a-0x03000000:a-0x03000000+size]=(value&((1<<(8*size))-1)).to_bytes(size,'little');return
  return super().write(a,value,size)
p=OneFrameblockScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and p.counts==[8660,8006,73,2121]
assert [p.read(0x030016cc+i*4) for i in range(4)]==[0x10080000,0x03006810,0x0202baec,0x03006960]
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'copied_background_entry':'030028e8','background_arm_instructions':p.background_steps,'mutable_frameblock':'030016cc','selected_framebuffer':'10080000','counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'OriginalThumbframehandoff/copiedARMbackground/all37copiedscenerydraws nowshareONEactualcopiedmutableframeblock030016CC andONEframebuffer/edgeworkspace/consumerstack/registers/NZCV. Background executesoriginalcopied030028E8patchedcode/data, replacingROMbridge. Fixture02009100addressadapter retainedonlyforprefixinitialization; backingdata actualimage. Allintermediateimages/work nativeexact. Bootstrap/mathbridges/initialedgezero/unusedbuffers/actors/palette/fullframe/cycles/cadence remainunproved.'}
(root/'reports/source240-one-frameblock-background-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
