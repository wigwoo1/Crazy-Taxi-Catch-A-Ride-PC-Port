"""Original ordinary pause/restart/quit art with caller bank11 palettes."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(r).hexdigest()==EXPECTED
out=pathlib.Path('outputs/crazy-taxi-native/assets/pause');out.mkdir(exist_ok=True)
b=0x70e50;g=b+struct.unpack_from('<I',r,b+4+7*4)[0];count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
assert (count,bits,tiles,pals)==(3,16,48,1)
metrics=[]
for frame in range(3):
 a=b+struct.unpack_from('<I',r,g+20+frame*4)[0];m=struct.unpack_from('<9H',r,a);assert m[:2]==(4,4) and m[8]==16*frame;metrics.append(m)
rows=[]
for mode,table,palette in [('main',0x7ec36c,0x290a44),('restart',0x7ec380,0x290a68),('quit',0x7ec394,0x290a8c)]:
 source=struct.unpack_from('<I',r,table)[0]-0x8000000;colors=struct.unpack_from('<16H',r,palette);blob=bytearray();indices=bytearray();sheet=bytearray(96*32*4)
 for frame in range(3):
  rgba=bytearray();indexed=bytearray()
  for y in range(32):
   for x in range(32):
    a=source+frame*512+(y//8*4+x//8)*32+(y%8)*4+(x%8)//2
    index=(r[a]>>(4*(x&1)))&15;indexed.append(index);color=colors[index]
    pixel=bytes([((color>>s)&31)*255//31 for s in [0,5,10]]+[255 if index else 0]);rgba.extend(pixel)
    at=(y*96+frame*32+x)*4;sheet[at:at+4]=pixel
  blob.extend(rgba);indices.extend(indexed);write_png(out/f'{mode}-{frame}.png',32,32,rgba)
 (out/f'{mode}.rgba').write_bytes(blob);(out/f'{mode}.indices').write_bytes(indices);(out/f'{mode}.rgb555').write_bytes(r[palette:palette+32]);write_png(out/f'{mode}-sheet.png',96,32,sheet)
 rows.append({'mode':mode,'language':0,'source':hex(source+0x8000000),'caller_palette_bank':11,'palette':hex(palette+0x8000000),'metrics':[list(m) for m in metrics],'rgba_sha256':hashlib.sha256(blob).hexdigest(),'indices_sha256':hashlib.sha256(indices).hexdigest()})
texts=[];base=0x49dfe0
for index in [451,452,453,457,458]:
 a=base+struct.unpack_from('<I',r,base+4+4*index)[0];text=r[a:r.index(0,a)].decode('latin1');texts.append({'id':index,'source':hex(a+0x8000000),'text':text})
(out/'pause.crs').write_bytes(b'CRS1'+struct.pack('<I',3)+b''.join(struct.pack('<9H',*m) for m in metrics))
(out/'pause.json').write_text(json.dumps({'rom_sha256':EXPECTED,'hud_container':'08070E50','frames':rows,'texts':texts,'scope':'Original language0 ordinaryHUDgroup7 three32x32 replacements and explicitcallerOBJbank11 palettes; index0transparent extraction convention. Original28288 placement/blend/OAM pixel equivalence and otherlanguages unverified. Main labels Resume/Restart/Quit; confirmations Yes/No.'},indent=2));print('Recovered9 original32x32 pause/restart/quit frames and5source labels')
