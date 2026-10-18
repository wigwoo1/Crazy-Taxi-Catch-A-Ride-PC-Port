"""Original full scene consumer owns the near material callback selection."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_installed_normal_returns.py').read_text().split('\np=InstalledDrawingProbe();')[0])
class OwnerProbe(InstalledDrawingProbe):
 def __init__(self):
  self.owner_writes=[];self.near_selections=[];self.record_owner=False;super().__init__();self.record_owner=True
 def write(self,a,value,size=4):
  if a==0x03006920 and self.record_owner:self.owner_writes.append({'instruction_pc_register':f'{self.reg[15]:08x}','target':f'{value:08x}'})
  return super().write(a,value,size)
 def raster_request(self,callback):
  idx=(self.reg[1]-16-0x02009eec)//144;packet=packets[idx]
  if packet['raster_flags']&1:
   actual=self.read(0x03006920);expected=installed[int(packet['callback'],16)];assert actual==expected,(idx,hex(actual),hex(expected))
   assert self.owner_writes and int(self.owner_writes[-1]['target'],16)==actual
   self.near_selections.append({'source_index':packet['source_index'],'material_target':f'{actual:08x}','owner_write':self.owner_writes[-1]})
  return super().raster_request(callback)
p=OwnerProbe();p.run(0x0802b6a0)
assert len(p.near_selections)==3 and len(p.requests)==37 and p.counts==[8660,8006,73,2121]
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','near_selections':p.near_selections,'all_owner_writes':p.owner_writes,'scope':'Original full Thumb scene consumer writes03006920 with actualinstalledmaterialaddress before each3nearwrappercalls; exactnativefinalcallbackselection, all37 intermediateimages/work still exact. Materialcallee andnearreturn remain separatelyqualified bridges; no fullcontinuousABI/fullframe/cycles/cadence proof.'}
(root/'reports/source240-near-material-owner-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
