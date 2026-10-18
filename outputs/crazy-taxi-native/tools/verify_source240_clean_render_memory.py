"""Reject unused synthetic code/geometry/descriptor/stack memory during renderer checks."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');original_read_text=Path.read_text
removed=[]
def clean_buffers(probe,wrapper=False):
 kept=[(0x03000000,image)]
 if not wrapper:
  kept.append(next(x for x in probe.blocks if x[0]==0x10080000))
  kept.extend(x for x in probe.blocks if x[0]==0x020344f0)
 removed.append({'wrapper':wrapper,'removed_addresses':[f'{a:08x}' for a,b in probe.blocks if a not in {0x03000000,0x10080000,0x020344f0}]})
 probe.blocks=kept
prefix=original_read_text(root/'tools/verify_source240_one_frameblock_background_scene.py').split('\np=OneFrameblockScene();')[0]
def cleanup_loader(path,*args,**kwargs):
 text=original_read_text(path,*args,**kwargs)
 if path.name=='verify_source240_one_workspace_scene.py':
  needle=' return text\nPath.read_text=edge_adapt'
  replacement=""" if path.name in names:
  text=text.replace('q.reg=self.reg.copy();','clean_buffers(q);q.reg=self.reg.copy();')
  text=text.replace('q.reg=obj.reg.copy();','clean_buffers(q);q.reg=obj.reg.copy();')
  text=text.replace('wrapper.reg=self.reg.copy();','clean_buffers(wrapper,True);wrapper.reg=self.reg.copy();')
 return text
Path.read_text=edge_adapt"""
  assert needle in text;text=text.replace(needle,replacement)
 return text
Path.read_text=cleanup_loader
try:exec(prefix)
finally:Path.read_text=original_read_text
p=OneFrameblockScene();p.run(0x0802b6a0)
assert len(removed)==40 and len(p.requests)==37 and p.counts==[8660,8006,73,2121]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'cleaned_call_contexts':len(removed),'removed_buffers':removed,'counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'ALL37copiedrenderer callbacks plus3nearwrappers executewithsyntheticcode/geometry/descriptor/isolatedstack/enginebuffers removedbeforeinstructions; onlyactualcopiedimage/sharedframebuffer/sharededges remain mappedinARM contexts. Actualconsumerpacket/stack/globals/ROM readowners explicit, allintermediates nativeexact. Constructorsstillallocate discardedbuffers; boot/mathcontracts andfullactors/HUD/palette/service/cadence excluded.'}
(root/'reports/source240-clean-render-memory-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS clean contexts',len(removed),'draws',len(p.requests),flush=True)
