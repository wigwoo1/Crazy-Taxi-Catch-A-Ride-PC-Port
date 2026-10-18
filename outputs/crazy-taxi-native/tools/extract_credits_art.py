"""Exact credits descriptor layers and five caller palette sources; no new art."""
import pathlib,hashlib,struct,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();assert hashlib.sha256(r).hexdigest()==EXPECTED
out=pathlib.Path('outputs/crazy-taxi-native/assets/credits');out.mkdir(exist_ok=True)
b=0x344f8;layers=[]
for k in (0,1):
 mp,pal,src=[struct.unpack_from('<I',r,b+off+k*4)[0]-0x8000000 for off in (0,16,32)]
 pbytes,size,w,h=[struct.unpack_from('<H',r,b+off+k*2)[0] for off in (56,64,72,80)]
 assert (pbytes,w,h)==(32,32,32)
 compressed=r[b+124+k];assert compressed in (0,1)
 end=src;tiles=bytearray()
 if compressed:
  for tile in range(size//32):
   block=bytearray()
   while len(block)<32:
    command=r[end];end+=1;n=command&127;assert n and len(block)+n<=32
    if command&128:block.extend(r[end:end+n]);end+=n
    else:block.extend(bytes([r[end]])*n);end+=1
   tiles.extend(block)
  assert end<=mp
 else:tiles.extend(r[src:src+size]);end=src+size
 assert len(tiles)==size
 entries=struct.unpack_from('<1024H',r,mp);colors=struct.unpack_from('<16H',r,pal)
 rgba=bytearray(256*256*4);indices=bytearray(256*256)
 for y in range(256):
  for x in range(256):
   e=entries[(y//8)*32+x//8];tile=e&1023;assert tile<size//32
   xx=7-x%8 if e&1024 else x%8;yy=7-y%8 if e&2048 else y%8
   assert e>>12==0 # Initial map uses its embedded palette only.
   index=(tiles[tile*32+yy*4+xx//2]>>(4*(xx&1)))&15
   indices[y*256+x]=index;color=colors[index]
   rgba[(y*256+x)*4:(y*256+x)*4+4]=bytes([((color>>s)&31)*255//31 for s in (0,5,10)]+[255 if index else 0])
 prefix=f'layer{k}'
 blobs={'tiles':bytes(tiles),'map':r[mp:mp+2048],'rgb555':r[pal:pal+32],'indices':bytes(indices),'rgba':bytes(rgba)}
 for suffix,data in blobs.items():(out/f'{prefix}.{suffix}').write_bytes(data)
 write_png(out/f'{prefix}.png',256,256,rgba)
 layers.append({'layer':k,'map':hex(mp+0x8000000),'palette':hex(pal+0x8000000),'tiles':hex(src+0x8000000),'compressed':bool(compressed),'source_end':hex(end+0x8000000),'decoded_bytes':size,'tile_count':size//32,'sha256':{key:hashlib.sha256(v).hexdigest() for key,v in blobs.items()}})
palettes=[]
for k in range(5):
 off=0xbe7ec+32*k;raw=r[off:off+32];(out/f'caller-palette{k}.rgb555').write_bytes(raw)
 palettes.append({'call_order':k,'address':hex(0x8000000+off),'byte_count':32,'sha256':hashlib.sha256(raw).hexdigest()})
lines=[]
for i in range(750,854):
 base=0x49dfe0;off=base+struct.unpack_from('<I',r,base+4+i*4)[0];raw=r[off:r.index(0,off)];assert len(raw)==31
 lines.append(raw)
(out/'lines.bin').write_bytes(b''.join(lines))
# Original verified E388 order: two descriptor palettes, then five caller
# palettes. Credits prefix0/1 selects banks4/5, sources BE82C/BE84C.
font= (out/'layer0.tiles').read_bytes();font_rows=[]
for style,bank,pal in [(0,4,0xbe82c),(1,5,0xbe84c)]:
 colors=struct.unpack_from('<16H',r,pal);rgba=bytearray(128*64*4)
 for tile in range(128):
  for y in range(8):
   for x in range(8):
    idx=(font[tile*32+y*4+x//2]>>(4*(x&1)))&15;color=colors[idx]
    at=(((tile//16)*8+y)*128+(tile%16)*8+x)*4
    rgba[at:at+4]=bytes([((color>>sh)&31)*255//31 for sh in (0,5,10)]+[255 if idx else 0])
 (out/f'font-style{style}.rgba').write_bytes(rgba);write_png(out/f'font-style{style}.png',128,64,rgba)
 font_rows.append({'style':style,'bank':bank,'palette':hex(0x8000000+pal),'cell':8,'width':128,'height':64,'rgba_sha256':hashlib.sha256(rgba).hexdigest()})
report={'rom_sha256':EXPECTED,'descriptor':'080344F8','layers':layers,'caller_palettes':palettes,'font_styles':font_rows,'music_module':hex(struct.unpack_from('<I',r,0x7ec048)[0]),'music_start_order':r[0x7ec060+4],'line_count':104,'line_bytes':31,'lines_sha256':hashlib.sha256(b''.join(lines)).hexdigest(),'scope':'Exact descriptor/raw or tile-RLE payloads/maps/embedded palettes and original104strings. PNGs decode initial maps with embedded palettes and index0 transparency; NOT proof of live palette registration/compositing/full live palette contents. Font bank allocation established separately in credits-palette-bank-rom-audit.json under clean-registry contract. Credits dynamic font rows, callerE388 palette offsets, source setup/scroll/wrap/BGpriority/fade and nativehost composition remain separate.'}
(out/'credits.json').write_text(json.dumps(report,indent=2));print('PASS credits source assets',[(x['layer'],x['tile_count']) for x in layers],'music order',report['music_start_order'])
