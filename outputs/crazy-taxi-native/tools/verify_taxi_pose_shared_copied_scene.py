"""Exact taxi pose complete copied scenery consumer with shared authoritative memory."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
_pose_read=Path.read_text
# Redirect only geometry input; retain all original code, bootstrap and pixel owners.
def pose_read(path,*args,**kwargs):
 if path.name in ['source240-geometry-work.json.source240.json','material-phase-live-120.json.source240.json']:
  return _pose_read(root/'reports/taxi-pose-source240-geometry.json',*args,**kwargs)
 return _pose_read(path,*args,**kwargs)
Path.read_text=pose_read
try:
 prefix=_pose_read(root/'tools/verify_source240_clean_render_memory.py').split('\np=OneFrameblockScene();')[0]
 exec(prefix)
 p=OneFrameblockScene();p.run(0x0802b6a0)
finally:Path.read_text=_pose_read
assert len(removed)==40 and len(p.requests)==37 and p.counts==[8676,8022,79,2108]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600))
assert fb==(root/'reports/taxi-pose-background-scenery-indexed.bin').read_bytes()
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
report=dict(status='PASS',camera=[20626,19666,-35593],yaw=-485,pitch=1,packets=37,cleaned_call_contexts=len(removed),counts=p.counts,framebuffer_sha256=hashlib.sha256(fb).hexdigest(),scope='Original Thumb dispatch, copied original ARM background and all scenery draw/near routines share actual copied image/frameblock/framebuffer/edges/stack/registers at exact recorded taxi camera; synthetic unused render buffers removed. Every native intermediate framebuffer/work exact. Actors/taxi/HUD/final palette/hardware priorities/full original final-frame oracle/cadence excluded; offline diagnostics only.')
(root/'reports/taxi-pose-shared-copied-scene-verification.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report),flush=True)
