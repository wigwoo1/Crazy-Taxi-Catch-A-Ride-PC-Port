from pathlib import Path
import struct,json,subprocess,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports'
oam=(reports/'bound-third-frame-oam-after.bin').read_bytes()[5898:6922]
objects=(reports/'bound-third-frame-sprite-flush-after.bin').read_bytes()[770:]
palette=(reports/'bound-third-frame-palette-after.bin').read_bytes()[4664:5176]
assert len(oam)==1024 and len(objects)==32768 and len(palette)==512
cp=subprocess.run([str(root/'bin/published-obj-layer-probe.exe')],input=oam+objects+palette,capture_output=True)
assert cp.returncode==0 and len(cp.stdout)==153600,(cp.returncode,len(cp.stdout))
attrs=[struct.unpack_from('<4H',oam,i*8) for i in range(128)]
expected=bytearray(153600);count=0
for i in range(127,-1,-1):
 a,b,c,_=attrs[i];affine=bool(a&256)
 if not affine and a&512:continue
 assert not a&0xfc00 and not c&0xc00
 count+=1;side=8<<(b>>14);field=side*(2 if affine and a&512 else 1)
 left=b&511;top=a&255;left=left-512 if left>=256 else left;top=top-256 if top>=160 else top
 matrix=[attrs[((b>>9)&31)*4+k][3] for k in range(4)];matrix=[v-65536 if v&32768 else v for v in matrix]
 for y in range(field):
  for x in range(field):
   dx=left+x;dy=top+y
   if not(0<=dx<240 and 0<=dy<160):continue
   if affine:
    u=(matrix[0]*(x-field//2)+matrix[1]*(y-field//2)+side*128)//256
    v=(matrix[2]*(x-field//2)+matrix[3]*(y-field//2)+side*128)//256
   else:u=side-1-x if b&4096 else x;v=side-1-y if b&8192 else y
   if not(0<=u<side and 0<=v<side):continue
   offset=(c&1023)*32+((v//8)*(side//8)+u//8)*32+(v%8)*4+u%8//2
   assert 0<=offset<len(objects)
   index=objects[offset]>>(4*(u&1))&15
   if not index:continue
   color=struct.unpack_from('<H',palette,((c>>12)*16+index)*2)[0]
   pos=(dy*240+dx)*4;expected[pos:pos+4]=bytes([((color>>(k*5))&31)*255//31 for k in range(3)]+[255])
assert cp.stdout==expected
(reports/'bound-third-frame-native-published-obj-layer.rgba').write_bytes(cp.stdout)
report={'status':'NATIVE_PUBLISHED_OBJ_LAYER_CONTRACT_PASS','objects':count,'rgba_bytes':153600,'opaque_pixels':sum(expected[i+3]!=0 for i in range(0,len(expected),4)),'sha256':hashlib.sha256(expected).hexdigest(),'scope':'Actual source-published third-frame OAM, tiles and palette vs native square opaque priority0 4bpp layer and independent tile/coordinate arithmetic. Hardware display, blend/window/BG priority, complete original visible-frame equivalence and live integration unproven.'}
(reports/'bound-third-frame-published-obj-layer-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
