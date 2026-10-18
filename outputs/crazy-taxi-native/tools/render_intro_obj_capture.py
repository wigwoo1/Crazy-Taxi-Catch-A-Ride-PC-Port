"""Sprite-only diagnostic from native publication; not complete GBA display proof."""
from pathlib import Path
import struct,sys,json,hashlib
root=Path('outputs/crazy-taxi-native');sys.path.insert(0,str(root/'tools'))
from png_writer import write_png
tiles=(root/'assets/startup-animation/sprites.tiles').read_bytes()
dimensions=[[(8,8),(16,16),(32,32),(64,64)],[(16,8),(32,8),(32,16),(64,32)],[(8,16),(8,32),(16,32),(32,64)]]
files=[]
for frame in [60,120,200,300]:
 capture=root/f'reports/intro-native-frame-{frame}.bin';data=capture.read_bytes();assert len(data)==1536
 obj=[struct.unpack_from('<4H',data,i*8) for i in range(128)];palette=struct.unpack_from('<256H',data,1024)
 rgba=bytearray(240*160*4);drawn=0
 for a,b,c,_ in reversed(obj):
  affine=bool(a&0x100)
  if not affine and a&0x200:continue
  shape=a>>14
  if shape>=3:continue
  w,h=dimensions[shape][b>>14];double=affine and bool(a&0x200);fw=w*(2 if double else 1);fh=h*(2 if double else 1)
  left=b&511;top=a&255;left=left-512 if left>=256 else left;top=top-256 if top>=160 else top
  slot=(b>>9)&31
  matrix=[obj[4*slot+j][3] for j in range(4)];matrix=[v-65536 if v&32768 else v for v in matrix]
  for y in range(fh):
   for x in range(fw):
    dx,dy=left+x,top+y
    if not(0<=dx<240 and 0<=dy<160):continue
    if affine:
     px=x-fw//2;py=y-fh//2
     sx=(matrix[0]*px+matrix[1]*py+w//2*256)//256;sy=(matrix[2]*px+matrix[3]*py+h//2*256)//256
    else:sx=w-1-x if b&0x1000 else x;sy=h-1-y if b&0x2000 else y
    if not(0<=sx<w and 0<=sy<h):continue
    offset=((c&1023)//2+(sy//8)*(w//8)+sx//8)*64+(sy%8)*8+sx%8
    if offset>=len(tiles):raise AssertionError(('tile outside resource',offset))
    index=tiles[offset]
    if not index:continue
    color=palette[index];pos=(dy*240+dx)*4
    rgba[pos:pos+4]=bytes([(((color>>s)&31)<<3)|(((color>>s)&31)>>2) for s in [0,5,10]]+[255]);drawn+=1
 out=root/f'reports/intro-native-objects-{frame}.png';write_png(out,240,160,rgba);files.append({'frame':frame,'pixels_written':drawn,'capture_sha256':hashlib.sha256(data).hexdigest(),'png':str(out),'png_sha256':hashlib.sha256(out.read_bytes()).hexdigest()})
(root/'reports/intro-object-render-diagnostic.json').write_text(json.dumps({'scope':'Sprite-only native-buffer diagnostic. No BG, priority interleaving, semitransparency blending, fades or original hardware pixel equivalence.','files':files},indent=2)+'\n')
print('Rendered four native sprite publication captures')
