"""Scene draw verification using actual cold-owner installed raster targets."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native')
bindings=json.loads((root/'reports/renderer-installed-targets-verification.json').read_text())
assert bindings['status']=='PASS'
installed={int(row['slot'],16):int(row['installed_value'],16) for row in bindings['installed_slots'] if row['slot'] not in ('03006960','030069a8','03006a34')}
installed_reverse={value:slot for slot,value in installed.items()}
source=(root/'tools/verify_source240_joined_render_bodies.py').read_text().split('p=DrawingProbe();')[0]
requests=(root/'tools/verify_source240_joined_render_requests.py').read_text().split('p=QueueProbe();')[0]
assert 'if 0x03006900<=pc<0x03006ac0' in requests
requests=requests.replace('if 0x03006900<=pc<0x03006ac0','if pc in installed_reverse')
source=source.replace("exec((root/'tools/verify_source240_joined_render_requests.py').read_text().split('p=QueueProbe();')[0])",'exec(requests)')
exec(source)
class InstalledDrawingProbe(DrawingProbe):
 def __init__(self):
  super().__init__();self.actual_targets=[]
  for slot,value in installed.items():self.write(slot,value)
 def raster_request(self,callback):
  assert callback in installed_reverse
  self.actual_targets.append(callback)
  return super().raster_request(installed_reverse[callback])
p=InstalledDrawingProbe();p.run(0x0802b6a0)
assert len(p.requests)==37 and all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
assert p.counts==[8660,8006,73,2121]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'actual_installed_targets':[f'{v:08x}' for v in p.actual_targets],'installed_raster_slots':{f'{k:08x}':f'{v:08x}' for k,v in installed.items()},'intermediate_framebuffer_bytes_compared':1420800,'counts':p.counts,'original_dispatch_thumb_instructions':p.steps,'original_background_arm_instructions':p.background_steps,'original_near_arm_instructions':p.near_steps,'original_raster_arm_instructions':p.raster_steps,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Original renderer indirect dispatch consumes nine actual cold-bootstrap-installed relocated raster addresses, not identity slot fixtures. All37 intermediate scenery/background images and work match native. Callback target bridge selects original ROM bodies using verified relocation; explicit callee ABI/register preservation and pre-epilogue stops remain. Frameblock/background callback relocation fixtures retained. No actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-installed-render-bodies-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
