"""Execute original cold bootstrap, observe installed renderer pointer identities."""
from pathlib import Path
import json,hashlib,struct
root=Path('outputs/crazy-taxi-native')
source=(root/'tools/audit_graphics_bootstrap_cold_source.py').read_text().split("report={'status'")[0]
exec(source)
slots=[0x03006914,0x0300691c,0x03006934,0x03006948,0x03006950,0x03006960,0x030069a4,0x030069a8,0x030069ac,0x030069b0,0x03006a28,0x03006a34]
rows=[]
# Cold owner copies ARM image source08000700 to destination03000008.
base=0x03000008
for slot in slots:
 value=p.read(slot);rom_address=0x08000700+value-base if base<=value<0x03006810 else None
 matched=False
 if rom_address is not None:
  matched=bytes(p.regions[0x03000000][value-0x03000000:value-0x03000000+16])==r[rom_address-0x08000000:rom_address-0x08000000+16]
  assert matched,(hex(slot),hex(value),hex(rom_address))
 rows.append({'slot':f'{slot:08x}','installed_value':f'{value:08x}','relocated_rom_address':f'{rom_address:08x}' if rom_address is not None else None,'first16bytes_match_rom':matched})
expected={0x03006914:0x08001448,0x0300691c:0x08004424,0x03006934:0x08000c5c,0x03006948:0x08001730,0x03006950:0x08001dd4,0x030069a4:0x080049f8,0x030069a8:0x08001dc4,0x030069ac:0x08001048,0x030069b0:0x08004704,0x03006a28:0x08004a94,0x03006a34:0x08002fe0}
for slot,rom_address in expected.items():assert p.read(slot)==base+rom_address-0x08000700,(hex(slot),hex(p.read(slot)))
report={'status':'PASS','installed_slots':rows,'original_bootstrap_thumb_instructions':p.steps,'scope':'Executed actual cold bootstrap owner and logical DMA ARM image copy. Eleven installed renderer code/data pointer identities validated against immutable ROM plus copied bytes. 03006960 observed only; semantics not inferred. Bootstrap helper divide contracts inherited. No warmboot/shaded mode/full callback execution/runtime timing proof.'}
(root/'reports/renderer-installed-targets-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
