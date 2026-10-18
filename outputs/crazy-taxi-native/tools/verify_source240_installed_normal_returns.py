"""Installed dispatch comparison with full original normal raster returns."""
from pathlib import Path
import json,re
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/verify_source240_installed_render_bodies.py').read_text().split('p=InstalledDrawingProbe();')[0])
for f,name,end,oldend in [(0,'flat',0x1f94,0x1f88),(1,'affine',0x198c,0x1980),(2,'clipped',0x1730,0x1724),(3,'perspective',0x1044,0x1038),(4,'column',0x1440,0x1434)]:
 scope={};exec((root/'tools'/('verify_source240_full_'+name+'_polygon.py')).read_text().split('\nj=json')[0],scope)
 cls=scope['Probe'];s=scope['s'];s,n=re.subn(r'while pc!=BASE\+0x[0-9a-f]+-0x700:', 'while pc!=0xffffffff:',s);assert n==1
 s=s.replace('<'+hex(oldend),'<'+hex(end));exec(s,scope);scope['arm'].detail=True;cls.run=scope['MarkerRasterProbe'].run;probes[f]=(cls,scope['FB'])
p=InstalledDrawingProbe();p.run(0x0802b6a0)
assert len(p.requests)==37 and all(p.read(0x02009cec+i*4)==0 for i in range(128)) and p.read(0x03006910)==0
assert p.counts==[8660,8006,73,2121] and p.raster_steps==244563
fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==(root/'reports/source240-background-scenery-indexed.bin').read_bytes()
report={'status':'PASS','packets':37,'normal_raster_complete_returns':33,'intermediate_framebuffer_bytes_compared':1420800,'counts':p.counts,'original_raster_arm_instructions':p.raster_steps,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'scope':'Installed original renderer dispatch plus full33 normal raster bodies/epilogues; all37 intermediate native images/work exact. Normal stack/register restoration separately seeded and qualified. Context relocation and bridge caller preservation remain; near materialdispatch and billboard return ABI not yet joined; excludes actors/palette/fullframe/cycles/cadence.'}
(root/'reports/source240-installed-normal-returns-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
