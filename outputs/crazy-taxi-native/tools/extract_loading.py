"""Original normal startup loading sprites/text; not a completed screen."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
out=pathlib.Path('outputs/crazy-taxi-native/assets/loading');out.mkdir(exist_ok=True)
b=0x70e50;rows=[];blob=bytearray()
for group,frames in ((7,(0,1,2)),(2,(0,))):
 g=b+struct.unpack_from('<I',r,b+4+group*4)[0]
 count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
 assert bits==16 and 0<pals<=4
 pal=g+20+count*4;pix=pal+32*pals;assert pix+tiles*32<=len(r)
 colors=struct.unpack_from('<16H',r,pal)
 for frame in frames:
  assert frame<count
  a=b+struct.unpack_from('<I',r,g+20+frame*4)[0];m=struct.unpack_from('<9H',r,a)
  tx,ty,ox,oy,cx,cy,w,h,tile=m;assert 0<tx<=8 and 0<ty<=8 and tile+tx*ty<=tiles
  rgba=bytearray()
  for y in range(ty*8):
   for x in range(tx*8):
    p=pix+(tile+(y//8)*tx+x//8)*32+(y%8)*4+x%8//2;i=(r[p]>>(4*(x&1)))&15;c=colors[i]
    rgba.extend([((c>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0])
  rows.append(dict(group=group,frame=frame,metrics=list(m),descriptor=hex(a+0x8000000),palette=hex(pal+0x8000000),byte_offset=len(blob),byte_length=len(rgba)))
  blob.extend(rgba);write_png(out/f'group-{group}-frame-{frame}.png',tx*8,ty*8,rgba)
base=0x49dfe0;texts=[]
for ident in (148,149):
 a=base+struct.unpack_from('<I',r,base+4+ident*4)[0];text=r[a:r.index(0,a)];assert 0<len(text)<128
 texts.append(dict(id=ident,address=hex(a+0x8000000),text=text.decode('latin1')))
(out/'sprites.rgba').write_bytes(blob);(out/'sprites.crs').write_bytes(b'CRS1'+struct.pack('<I',len(rows))+b''.join(struct.pack('<9H',*x['metrics']) for x in rows))
(out/'sprites.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,container='08070e50',frames=rows,texts=texts,sha256=hashlib.sha256(blob).hexdigest(),qualification='Source0800d764 normal mode loading presentation. Group7 anchors72/104/136,y120; group2frame0 at120/24 updated palette source080fd5fc. Scene palette/image31ef8 and fonts08415bbc not yet integrated.'),indent=2)+'\n')
print('Loading assets:',len(rows),'frames',len(blob),'bytes;',texts)

# 0800d86a/27fb8: five source 64x32 4bpp banks, not palette animation.
g=b+struct.unpack_from('<I',r,b+12)[0];count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
assert (count,bits,tiles,pals)==(1,16,32,1)
assert 0x2001790+5*20+16==0x2001790+0x74
colors=struct.unpack_from('<16H',r,0x290bac);animation=bytearray()
for bank in range(5):
 rgba=bytearray()
 for y in range(32):
  for x in range(64):
   p=0xfd5fc+bank*1024+((y//8)*8+x//8)*32+(y%8)*4+x%8//2
   i=(r[p]>>(4*(x&1)))&15;c=colors[i]
   rgba.extend([((c>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0])
 animation.extend(rgba);write_png(out/f'animation-{bank}.png',64,32,rgba)
assert len(animation)==40960
(out/'animation.rgba').write_bytes(animation)
(out/'animation.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,source='080fd5fc',frames=5,width=64,height=32,source_stride=1024,anchor=[120,24],offset=[33,17],palette='08290bac',sha256=hashlib.sha256(animation).hexdigest(),qualification='Frame resets0 in b6ec;227ac/229de increment modulo5 before d764 once per initialization record whose index modulo5 is0. No fixed fps invented.'),indent=2)+'\n')
print('Original five64x32 loading banks:',len(animation),'RGBA bytes.')

# ad68..ad7a registers selected scene palette; e154 stores its pointer at
# palette-entry+16. Mode1/flags1 e5a8 reads that pointer at ebac..ebd8.
# 1770 only affects initial DMA/fade mode2, not this mode1 source.
scene_table=struct.unpack_from('<I',r,0x75579c+12)[0]-0x8000000
image=r[0xeb884:0xeb884+4096];assert len(image)==4096
for city in range(2):
 address=struct.unpack_from('<I',r,scene_table+city*104+24)[0]
 pal=address-0x8000000;raw=r[pal:pal+512];assert len(raw)==512
 colors=struct.unpack('<256H',raw)
 clear=min(range(256),key=lambda i:sum((colors[i]>>shift)&31 for shift in (0,5,10)))
 (out/'backdrop-index.u8').write_bytes(bytes([clear]))
 indices=bytearray([clear])*38400
 for y0 in (0,60,120):
  for y in range(min(64,160-y0)):
   for x in range(64):
    i=image[y*64+x]
    if i:indices[(y0+y)*240+x]=i
 rgba=bytearray()
 for i in indices:
  c=colors[i];rgba.extend([((c>>s)&31)*255//31 for s in (0,5,10)]+[255])
 (out/f'city-{city}-palette.rgb555').write_bytes(raw)
 write_png(out/f'city-{city}-image-layer.png',240,160,rgba)
 (out/f'city-{city}-palette.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,city=city,scene_table=hex(scene_table+0x8000000),palette=hex(address),clear_index=clear,clear_rgb555=hex(colors[clear]),sha256=hashlib.sha256(raw).hexdigest(),qualification='Source ad7a/e154 entry+16 and e5a8 mode1 flags1. Image layer only;2ac6c->2af26 calls2c558 RGB0/0/0 nearestpalette search strict-less ties first. Clearindexwritten03006810+f0=03006900. Foreground composition still pending. Initial1770 DMA selector unresolved; no claim for mode2.'),indent=2)+'\n')
 print('Loading city palette',city,hex(address),len(raw))

# Original227ac first pass/229c6 second pass both traverse header+14,
# record stride32. Loading helper occurs BEFORE processing every fifth index.
work=[]
for city in range(2):
 header=scene_table+city*104
 count=struct.unpack_from('<H',r,header+14)[0]
 records=struct.unpack_from('<I',r,header+48)[0]
 raw=r[records-0x8000000:records-0x8000000+count*32]
 assert len(raw)==count*32 and count<=128
 bank=0;events=[]
 for phase in range(2):
  for index in range(count):
   if index%5==0:
    bank=(bank+1)%5;events.append(dict(pass_index=phase,index=index,bank=bank))
 work.append(dict(city=city,count=count,records=hex(records),stride=32,records_sha256=hashlib.sha256(raw).hexdigest(),presentation_events=events,final_bank=bank))
(out/'work-records.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,cities=work,qualification='Original loading calls before records in both passes, includes skipped/noncustomer records. Not work duration or completed traffic/actor port.'),indent=2)+'\n')

# adf6/ae06/ae16 queue source080fcff8 banks0/1/2 into group7frames0/1/2.
# 27fb8 flag0 queues raw copies (NOT a compressed decoder).
# acb2 clears palette entries;27994 registers group palettes in order.
# Sum groups0..6 palettes=11; entry11+16=0200187c, overriddenade8.
g=b+struct.unpack_from('<I',r,b+4+7*4)[0]
count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
assert count==3 and bits==16
palette_counts=[struct.unpack_from('<I',r,b+struct.unpack_from('<I',r,b+4+i*4)[0]+16)[0] for i in range(7)]
assert sum(palette_counts)==11 and 0x2001790+11*20+16==0x200187c
pal=0x290b88;colors=struct.unpack_from('<16H',r,pal);override=bytearray();rows=[]
for frame in range(3):
 a=b+struct.unpack_from('<I',r,g+20+frame*4)[0];m=struct.unpack_from('<9H',r,a)
 tx,ty=m[:2];assert (tx,ty)==(4,4)
 raw=r[0xfcff8+frame*512:0xfcff8+(frame+1)*512];assert len(raw)==512
 rgba=bytearray()
 for y in range(32):
  for x in range(32):
   v=raw[((y//8)*4+x//8)*32+(y%8)*4+x%8//2];i=(v>>(4*(x&1)))&15;c=colors[i]
   rgba.extend([((c>>shift)&31)*255//31 for shift in (0,5,10)]+[255 if i else 0])
 override.extend(rgba);write_png(out/f'startup-frame-{frame}.png',32,32,rgba)
 rows.append(dict(frame=frame,source=hex(0x80fcff8+frame*512),metrics=list(m),source_sha256=hashlib.sha256(raw).hexdigest()))
(out/'startup-sprites.rgba').write_bytes(override)
(out/'startup-sprites.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,frames=rows,palette=hex(pal+0x8000000),startup_palette_pointer_override='0200187c <-08290b88',qualification='Sourceacb2 clears entries;27994 registers groups0..6 palette counts4/1/1/2/1/1/1=11; ade8 overrides entry11+16=0200187c with08290b88. Raw replacementsqueuedadf6/ae06/ae16 and DMAcopied28ee0. Replaces earlier unmodified container digit preview.'),indent=2)+'\n')
