"""Native title pixels versus decoded source art/palette composition, no device oracle."""
from pathlib import Path
import struct,subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');rom='Crazy Taxi - Catch a Ride (USA).gba';r=Path(rom).read_bytes();indices=(root/'assets/title/background.indices').read_bytes();bgpalette=struct.unpack_from('<16H',r,0xb70f8)
u32=lambda n:struct.unpack_from('<I',r,n)[0]
g=0x34634+u32(0x34634+44);objpalette=struct.unpack_from('<16H',r,g+20+4*4)
results=[]
for language in range(4):
 source=u32(0x7ebf48+language*4)-0x8000000
 for scroll in [0,272]:
  for brightness in [0,8,16]:
   for prompt in [0,1]:
    expected=[bgpalette[indices[y*512+x+scroll]] for y in range(160) for x in range(240)]
    if prompt:
     for frame in range(4):
      m=0x34634+u32(g+20+frame*4);width,height,ox,oy=struct.unpack_from('<HHhh',r,m);left,top=56+frame*32-ox,134-oy
      for y in range(height*8):
       for x in range(width*8):
        offset=source+frame*256+((y//8)*width+x//8)*32+(y%8)*4+(x%8)//2;index=(r[offset]>>(4*(x&1)))&15
        dx,dy=left+x,top+y
        if index and 0<=dx<240 and 0<=dy<160:expected[dy*240+dx]=objpalette[index]
    def dark(v):return sum((((v>>shift)&31)-((v>>shift)&31)*brightness//16)<<shift for shift in [0,5,10])
    expected=struct.pack('<38400H',*(dark(v) for v in expected))
    capture=root/'reports/title-pixels-native.bin'
    subprocess.run([str(root/'bin/title-pixels-check.exe'),rom,str(language),str(scroll),str(brightness),str(prompt),str(capture)],check=True)
    actual=capture.read_bytes();assert actual==expected,(language,scroll,brightness,prompt,next((i,a,b) for i,(a,b) in enumerate(zip(actual,expected)) if a!=b))
    results.append({'language':language,'scroll':scroll,'brightness':brightness,'prompt':prompt,'sha256':hashlib.sha256(actual).hexdigest()})
report={'status':'PASS','cases':len(results),'compared_pixels':len(results)*38400,'scope':'Native title compositor from loaded map/tile/palette/published4bppOBJ versus independent source-art composition: all4languages/scroll0,272/brightness0,8,16/promptshown-hidden. Sourcepalette group10bank15. Ordinaryopaquepriority0 title requests only; affine8bppsemiwindow rejected. No original hardware pixel oracle/wholeoriginaltitleframe/livewindow/parent proof.','cases_detail':results,'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [root/'src/title_pixels.c',root/'src/title_pixels.h',root/'tests/title-pixels-check.c',root/'bin/title-pixels-check.exe',Path(__file__)]}}
(root/'reports/title-pixels-native-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS native title source-art pixels',len(results),'cases',len(results)*38400,'pixels')
