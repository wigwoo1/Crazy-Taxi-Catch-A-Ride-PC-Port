"""One shared framebuffer and edge workspace across original scenery callbacks."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_one_framebuffer_scene.py').read_text().split('Path.read_text=adapted_read')[0])
framebuffer_adapt=adapted_read
edge_patch_count=0
def edge_adapt(path,*args,**kwargs):
 global edge_patch_count
 text=framebuffer_adapt(path,*args,**kwargs)
 if path.name in names:
  for assignment in ['q.reg=self.reg.copy();','q.reg=obj.reg.copy();']:
   if assignment in text:
    edge_patch_count+=1
    text=text.replace(assignment,"q.blocks=[(a,self.shared_edge_buffer if a==0x020344f0 else b) for a,b in q.blocks];"+assignment)
  text=text.replace('if address==0x020344f0:buf[:]=self.edges[:len(buf)]','if address==0x020344f0:pass # Shared workspace already current.')
  text=text.replace('if address==0x020344f0:self.edges=bytes(buf)+self.edges[len(buf):]','if address==0x020344f0:assert buf is self.shared_edge_buffer')
 return text
Path.read_text=edge_adapt
try:exec((root/'tools/verify_source240_copied_arm_all_scene.py').read_text().split('\np=SharedAllScene();')[0])
finally:Path.read_text=saved_read_text
class OneWorkspaceScene(SharedAllScene):
 def __init__(self):self.shared_edge_buffer=bytearray(161*32);super().__init__()
p=OneWorkspaceScene();p.run(0x0802b6a0)
assert edge_patch_count>=3 and len(p.requests)==37 and p.counts==[8660,8006,73,2121]
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'shared_framebuffer':'10080000','shared_edge_workspace':'020344f0','shared_edge_workspace_bytes':len(p.shared_edge_buffer),'edge_workspace_sha256':hashlib.sha256(p.shared_edge_buffer).hexdigest(),'counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'All37 originalcopiedscenerycalls shareONEdirectframebuffer andONEedgebytearray atoriginalworkspaceaddress, no percallbackedgecopy-in/out. Sameconsumerstack/pool/regs/NZCV; allintermediates nativeexact andcleanup exact. Edgeinitialzero isfixture, notfullbootstrapallocationlifecycle proof. Frameblockbacking/unusedconstructorbuffers/backgroundROMbridge remain; no actors/palette/fullframe/cycles/cadence.'}
(root/'reports/source240-one-workspace-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
