"""Recover source marker bitmaps and embedded palettes; runtime coloring unproved."""
from pathlib import Path
import hashlib,json,struct
from png_writer import write_png
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();digest=hashlib.sha256(rom).hexdigest();assert digest=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
root=Path('outputs/crazy-taxi-native');out=root/'assets/passenger-markers';out.mkdir(exist_ok=True)
def region(a,n):
 off=a-0x8000000;assert 0<=off<=len(rom)-n;return rom[off:off+n]
rows=[];atlas=bytearray(640*64*4)
ring_anim=struct.unpack('<I',region(0x87ec5a8+4,4))[0]
for bank,start in [('ring',0x87ec59c),('class',0x87ec5d8)]:
 for index in range(5):
  a=start+12*index;descriptor=region(a,12);assert descriptor[:4]==bytes([64,64,6,6]);ptr=struct.unpack_from('<I',descriptor,4)[0]
  owner=ring_anim if bank=='ring' and index else ptr
  width,height,colors,reserved=struct.unpack('<4H',region(owner-520,8));assert (width,height,colors,reserved)==(64,256 if bank=='ring' and index else 64,256,0),(bank,index,width,height,colors,reserved)
  if bank=='ring' and index:assert ptr==owner+(index-1)*4096
  palette=region(owner-512,512);indices=region(ptr,4096);rgba=bytearray()
  for ix in indices:
   color=struct.unpack_from('<H',palette,ix*2)[0];cs=[color&31,(color>>5)&31,(color>>10)&31];rgba.extend([(v<<3)|(v>>2) for v in cs]+[255 if ix else 0])
  name=f'{bank}-{index}';(out/(name+'.indexed')).write_bytes(indices);(out/(name+'.rgb555')).write_bytes(palette);(out/(name+'.rgba')).write_bytes(rgba);write_png(out/(name+'.png'),64,64,rgba)
  column=index+(5 if bank=='class' else 0)
  for y in range(64):atlas[(y*640+column*64)*4:(y*640+(column+1)*64)*4]=rgba[y*256:(y+1)*256]
  rows.append({'bank':bank,'index':index,'descriptor':hex(a),'pixels':hex(ptr),'palette':hex(owner-512),'width':64,'height':64,'rgba_sha256':hashlib.sha256(rgba).hexdigest(),'indexed_sha256':hashlib.sha256(indices).hexdigest(),'palette_sha256':hashlib.sha256(palette).hexdigest()})
write_png(out/'source-preview.png',640,64,atlas)
(out/'markers.json').write_text(json.dumps({'rom_sha256':digest,'status':'10 original64x64 source frames; embedded RGB555 preview and index0 transparent convention only. Original runtime palette/keying and31A50 projection not yet qualified. Ring1..4 share one palette and256-high source bank; ring0/classes have separate64-high headers. Class upper-bound4 derives existing category code, original caller itself accepts byte class.','frames':rows},indent=2))
print('Recovered10 frames/40960 source indexed pixels; ring palette ownership/header dimensions verified.')
