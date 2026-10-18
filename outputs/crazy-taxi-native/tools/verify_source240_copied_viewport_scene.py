"""Near viewport references use original cold-bootstrap-patched literal and data."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native')
observed=json.loads((root/'reports/renderer-copied-viewport-owner-observation.json').read_text());assert observed['status']=='OBSERVED'
words={int(row['copied_address'],16):int(row['copied_word'],16) for row in observed['copied_words']}
viewport_slot=next(x for x in observed['slots'] if x['slot']=='0300692c');viewport=int(viewport_slot['value'],16);bounds=[int(v,16) for v in viewport_slot['pointed_words']]
assert words[0x030042f0]==viewport==0x030016b8 and bounds==[0,0,240,160]
source=(root/'tools/verify_source240_shared_all_stack_scene.py').read_text().split('\np=SharedAllScene();')[0]
assert 'if a==0x080049e8:return 0x03006818' in source
source=source.replace('if a==0x080049e8:return 0x03006818 # Explicit viewport data literal relocation; do not overwrite callback slots.','if a==0x080049e8:return self.read(0x030042f0) # Original bootstrap-patched copied literal.')
source=source.replace('0x03006810<=a<a+n<=0x03006b00','0x03006810<=a<a+n<=0x03006b00 or viewport<=a<a+n<=viewport+16')
exec(source)
class CopiedViewportScene(SharedAllScene):
 def __init__(self):super().__init__();self.write(0x0300692c,viewport)
 def read(self,a,size=4):
  if a==0x030042f0:return words[a]
  if viewport<=a<a+size<=viewport+16:
   raw=b''.join(v.to_bytes(4,'little') for v in bounds);return int.from_bytes(raw[a-viewport:a-viewport+size],'little')
  return super().read(a,size)
p=CopiedViewportScene();p.run(0x0802b6a0)
assert len(p.requests)==37 and len(p.shared_near_calls)==3 and p.counts==[8660,8006,73,2121]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'original_patched_viewport_literal_address':'030042f0','original_patched_viewport_pointer':f'{viewport:08x}','original_viewport_words':bounds,'counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Actualcoldbootstrapcopied49E8literal=030016B8 andoriginalmutableviewportdata replaceengineboundsfixturealias fornearwrapper; all37sharedstack/registerdrawintermediates nativeexact. Copiedcode execution stillrelocated/read-alias bridge; otherrasterframe/bounds/edge aliases remain. Bootstrap helpercontracts inherited; excludesactors/palette/fullframe/cycles/cadence.'}
(root/'reports/source240-copied-viewport-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
