"""Compare native taxi pixels directly with original captured final screens."""
from pathlib import Path
import struct,json,subprocess,sys,hashlib
sys.path.insert(0,'work/python-packages');from PIL import Image
root=Path('outputs/crazy-taxi-native');ref=root/'reports/original-core-reference';results=[]
for label in ['boot3300','boot4200']:
 raw=(ref/(label+'.07000000.bin')).read_bytes();selected=bytearray(raw);indices=[]
 for i in range(128):
  a,b,c,d=struct.unpack_from('<4H',raw,i*8)
  if (a,b,c) in [(0x341,0xc017,0x426d),(0x341,0xc057,0x42ad)]:indices.append(i)
  else:struct.pack_into('<H',selected,i*8,a&~0x100)
 assert len(indices)==2
 for i in indices:
  tile=struct.unpack_from('<H',raw,i*8+4)[0]&1023;assert 0x10000+tile*32+2048<=0x18000
 vram=(ref/(label+'.06000000.bin')).read_bytes();palette=(ref/(label+'.05000000.bin')).read_bytes()
 cp=subprocess.run([str(root/'bin/taxi-oam-layer-probe.exe')],input=selected+vram[0x14000:0x1c000].ljust(32768,b'\0')+palette[512:1024],capture_output=True,check=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
 layer=cp.stdout;assert len(layer)==153600;screen=Image.open(ref/(label+'.bmp')).convert('RGB');pixels=screen.tobytes();mismatches=[];opaque=0;legacy_mismatches=0
 metadata=json.loads((ref/(label+'.json')).read_text());assert metadata['pixel_format']==2
 io=(ref/(label+'.04000000.bin')).read_bytes()
 display={hex(a):hex(struct.unpack_from('<H',io,a)[0]) for a in [0,0x50,0x52,0x54]}
 for i in range(240*160):
  if not layer[i*4+3]:continue
  opaque+=1;actual=layer[i*4:i*4+3];expected=pixels[i*3:i*3+3]
  source5=[round(c*31/255) for c in actual]
  if source5!=[round(c*31/255) for c in expected]:legacy_mismatches+=1
  # Observed reference RGB565 mapping: five-bit green occupies upper five
  # bits of the six-bit channel. Require exact BMP bytes, without tolerance.
  capture_rgb=[source5[0]*255//31,(source5[1]<<1)*255//63,source5[2]*255//31]
  if capture_rgb!=list(expected):mismatches.append({'xy':[i%240,i//240],'native':list(actual),'capture_mapping':capture_rgb,'original_capture':list(expected)})
 Image.frombytes('RGBA',(240,160),layer).save(root/'reports'/(label+'-native-taxi-layer.png'))
 results.append({'label':label,'taxi_oam_indices':indices,'opaque_pixels':opaque,'exact_capture_rgb_mismatches':len(mismatches),'legacy_5bit_normalization_mismatches':legacy_mismatches,'display_registers':display,'first_mismatches':mismatches[:12],'layer_sha256':hashlib.sha256(layer).hexdigest()})
report={'status':'PASS' if all(x['exact_capture_rgb_mismatches']==0 for x in results) else 'MISMATCH_FOUND','cases':results,'scope':'Nativeaffinetaxi compositor actualcapturedOAM/VRAM/OBJpalette comparedatALLopaqueoutputpixels toindependentoriginalfinal240x160BMP, exact BMP bytes after observed RGB565 channel mapping (green5 shifted left once, not bit replicated); mapping inferred from captures and pixel-format metadata, not independently established from core source. Isolateddriver0frame0twohalves, no fullsceneotheractors/transparency/priority/blend/generalpose/4Kproof.'}
(root/'reports/taxi-original-final-pixel-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report),flush=True)
