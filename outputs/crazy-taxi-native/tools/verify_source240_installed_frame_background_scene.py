"""Original installed background callback and mutable frameblock addresses."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_installed_all_returns_scene.py').read_text().split('\np=FullSpriteScene();')[0])
installed_rows=json.loads((root/'reports/renderer-installed-targets-verification.json').read_text())['installed_slots']
actual={int(row['slot'],16):int(row['installed_value'],16) for row in installed_rows}
frameblock=actual[0x030069a8];background_target=actual[0x03006a34];installed_reverse[background_target]=0x03006a34
class BoundScene(FullSpriteScene):
 def __init__(self):
  super().__init__();self.background_calls=0
  self.write(0x030069a8,frameblock);self.write(0x03006a34,background_target);self.write(0x03006960,actual[0x03006960])
 def read(self,a,size=4):
  if frameblock<=a<a+size<=frameblock+16:a=0x02009100+a-frameblock
  return super().read(a,size)
 def write(self,a,value,size=4):
  if frameblock<=a<a+size<=frameblock+16:a=0x02009100+a-frameblock
  return super().write(a,value,size)
 def raster_request(self,callback):
  if callback==background_target:
   self.background_calls+=1;assert self.reg[1]==background_target
   return self.background()
  return super().raster_request(callback)
p=BoundScene();p.run(0x0802b6a0)
assert p.background_calls==1 and len(p.requests)==37 and p.counts==[8660,8006,73,2121]
assert [p.read(frameblock+i*4) for i in range(4)]==[0x10080000,0x03006810,0x0202baec,0x03006960]
assert p.read(0x03006960)==0
assert all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'installed_frameblock':f'{frameblock:08x}','installed_background_target':f'{background_target:08x}','shared6960_cold_value':p.read(0x03006960),'intermediate_framebuffer_bytes_compared':1420800,'counts':p.counts,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Originalconsumer loads actualcoldbootstrapinstalledframeblock030016CC/background030028E8 targets, writesmutableframefirstwordonly anddispatchesactualbackgroundtarget throughverifiedROMrelocation bridge. Cold6960zero retained; meaning notinferred. All37drawintermediates nativeexact withfullnormal/sprite/nearreturns. Data alias backingfixture/outercalleecontext-stackbridges stillexplicit; excludesactors/palette/fullframe/cycles/cadence.'}
(root/'reports/source240-installed-frame-background-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
