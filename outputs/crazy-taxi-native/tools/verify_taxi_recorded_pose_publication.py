"""Recorded taxi pose through original/native graphics and final OAM publication."""
from pathlib import Path
import json,struct
root=Path('outputs/crazy-taxi-native')
pose=json.loads((root/'reports/taxi-recorded-pose-graphics-verification.json').read_text());assert pose['status']=='PASS'
case=(0xffffffff,pose['frame'],pose['driver_fixture'],1,0,pose['scale'],pose['body_roll'],pose['actual_original_sine'],pose['actual_original_cosine'],0,256,102,0)
source=(root/'tools/compare_taxi_effect_publish_rom.py').read_text();start=source.index('cases=[');end=source.index("capture=root/",start);source=source[:start]+'cases=[recorded_case]\n'+source[end:]
source=source.replace('taxi-effect-publish-native.bin','taxi-recorded-pose-publish-native.bin').replace('taxi-effect-publish-rom-comparison.json','taxi-recorded-pose-publish-rom-comparison.json')
ns={'recorded_case':case,'__file__':__file__};exec(source,ns)
data=(root/'reports/taxi-recorded-pose-publish-native.bin').read_bytes();oam=data[-1024:];sprites=[]
for i in range(128):
 a,b,c,d=struct.unpack_from('<4H',oam,i*8)
 if a&0x100 and (c&1023)>=512:sprites.append({'index':i,'attributes':[a,b,c,d],'matrix_slot':(b>>9)&31})
assert len(sprites)==2,sprites
matrices={str(x['matrix_slot']):[struct.unpack_from('<h',oam,(x['matrix_slot']*4+k)*8+6)[0] for k in range(4)] for x in sprites}
(root/'reports/taxi-recorded-pose-published-oam.bin').write_bytes(oam)
report={'status':'PASS','recorded_pose':pose,'published_taxi_sprites':sprites,'matrices':matrices,'scope':'Recordedtaxipose framecache/copy/affine/palette andactualoriginal27CD0publication helperchain exactnativeincluding1024OAMbytes. Extraeffect x256 isoffscreenfixture; driver0. TaxiOAMpublicationverified, notscenerycompositor/sourcecamera-equivalentwholeframe/device/cycles/cadence.'}
(root/'reports/taxi-recorded-pose-publication-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
