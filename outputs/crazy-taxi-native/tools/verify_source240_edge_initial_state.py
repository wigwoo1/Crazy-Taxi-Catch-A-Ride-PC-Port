"""Check actual scenery edge-workspace reads do not require zero-filled scratch."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_one_frameblock_background_scene.py').read_text().split('\np=OneFrameblockScene();')[0])
results=[]
for pattern in [0,0xa5,0xff]:
 p=OneFrameblockScene();p.shared_edge_buffer[:]=bytes([pattern])*len(p.shared_edge_buffer)
 try:
  p.run(0x0802b6a0)
  assert len(p.requests)==37 and p.counts==[8660,8006,73,2121]
  fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
  results.append({'initial_scratch_byte':pattern,'status':'PASS','matched_intermediate_images':37,'framebuffer_sha256':hashlib.sha256(fb).hexdigest()})
 except AssertionError as error:
  results.append({'initial_scratch_byte':pattern,'status':'FAIL','assertion':str(error),'completed_draw_requests':len(p.requests)})
report={'status':'PASS' if all(x['status']=='PASS' for x in results) else 'DEPENDENCY_FOUND','cases':results,'scope':'Actualsource240 baseline37draws withzero/A5/FF entire5152byteedgeworkspace before originalcopiedbackground/fullconsumer; all37intermediatefb/work comparednative ineachrun. Limitedonepose/materialset, doesnotprovegeneralallocationlifecycle/othergeometry/uninitializedbehavior.'}
(root/'reports/source240-edge-initial-state-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
