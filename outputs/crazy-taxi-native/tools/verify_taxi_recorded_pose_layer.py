from pathlib import Path
import json,struct,subprocess,hashlib,sys
sys.path.insert(0,'work/python-packages');from PIL import Image
root=Path('outputs/crazy-taxi-native');data=(root/'reports/taxi-recorded-pose-publish-native.bin').read_bytes();oam=data[-1024:];objects=data[788:788+32768];palette=data[788+32768+2560:788+32768+2560+512]
cp=subprocess.run([str(root/'bin/taxi-oam-layer-probe.exe')],input=oam+objects+palette,capture_output=True,check=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW);assert len(cp.stdout)==240*160*4
expected=bytearray(240*160*4);publication=json.loads((root/'reports/taxi-recorded-pose-publication-verification.json').read_text())
for sprite in reversed(publication['published_taxi_sprites']):
 a,b,c,d=sprite['attributes'];matrix=publication['matrices'][str(sprite['matrix_slot'])];left=(b&511);top=a&255;left=left-512 if left>=256 else left;top=top-256 if top>=160 else top
 field=128 if a&512 else 64;tile=(c&1023)*32-0x4000;bank=c>>12
 for py in range(max(0,top),min(160,top+field)):
  for px in range(max(0,left),min(240,left+field)):
   dx=px-left-field//2;dy=py-top-field//2;u=(matrix[0]*dx+matrix[1]*dy+8192)//256;v=(matrix[2]*dx+matrix[3]*dy+8192)//256
   if not (0<=u<64 and 0<=v<64):continue
   byte=objects[tile+((v//8)*8+u//8)*32+(v%8)*4+(u%8)//2];index=(byte>>(4*(u%2)))&15
   if not index:continue
   color=struct.unpack_from('<H',palette,(bank*16+index)*2)[0];offset=(py*240+px)*4;expected[offset:offset+4]=bytes([((color>>(5*k))&31)*255//31 for k in range(3)]+[255])
assert cp.stdout==expected
pixels=[(i%240,i//240) for i in range(240*160) if expected[i*4+3]];assert pixels
bounds=[min(x for x,y in pixels),min(y for x,y in pixels),max(x for x,y in pixels)+1,max(y for x,y in pixels)+1]
(root/'reports/taxi-recorded-pose-layer.rgba').write_bytes(cp.stdout);Image.frombytes('RGBA',(240,160),cp.stdout).save(root/'reports/taxi-recorded-pose-layer.png')
report={'status':'PASS','opaque_pixels':len(pixels),'visible_bounds':bounds,'rgba_bytes':len(expected),'sha256':hashlib.sha256(expected).hexdigest(),'scope':'NativeCPUtaxi_oam_layer usesactualpublishedOAM/OBJtiles/palette/affinematrix fromoriginal-vs-nativequalifiedrecordedpose; exactindependentPython rawtile/palette/coordinateformula. Targetedopaque64x64affinetaxi only, transparentlayer; no independentGBAfinalframe/compositor/hardwareblend/priority/4K/runtimeclaim.'}
(root/'reports/taxi-recorded-pose-layer-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
