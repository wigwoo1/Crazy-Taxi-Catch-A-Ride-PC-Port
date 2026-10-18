"""Independent tile-address-to-texture verification; no extractor imports."""
from pathlib import Path
import struct,hashlib,json
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
checks=[]
for driver in range(4):
 sources=struct.unpack_from('<III',rom,0x4a5514+driver*12);palette=struct.unpack_from('<16H',rom,sources[2]-0x8000000);asset=root/('assets/taxis/taxi-'+str(driver)+'.rgba');pixels=asset.read_bytes();assert len(pixels)==64*128*64*4;count=0
 for frame in range(64):
  for half in range(2):
   for tile in range(64):
    start=sources[half]-0x8000000+frame*2048+tile*32
    for texel in range(64):
     packed=rom[start+texel//2];index=(packed>>(4*(texel%2)))&15;color=palette[index];expected=bytes([(color>>(5*c)&31)*255//31 for c in range(3)]+[255 if index else 0]);x=half*64+(tile%8)*8+texel%8;y=(tile//8)*8+texel//8;offset=(frame*128*64+y*128+x)*4;assert pixels[offset:offset+4]==expected,(driver,frame,half,tile,texel);count+=1
 checks.append({'driver':driver,'pixels':count,'asset_sha256':hashlib.sha256(pixels).hexdigest()})
report={'status':'PASS','drivers':checks,'scope':'Every RGBA pixel/frame/half/row maps directly to source4bpp ROM tiles and sourceRGB555 palette. No frame reversal, half swap or row flip in extracted texture assets. GPU upload and raster require separate evidence.','tool_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()};(root/'reports/taxi-texture-orientation-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',sum(c['pixels'] for c in checks),'pixels')
