"""Original city selection groups4..6 frame0 and localized headings."""
import pathlib,struct,hashlib,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
b=0x520f8
out=pathlib.Path('outputs/crazy-taxi-native/assets/city-selection')
rows=[];blob=bytearray()
for group in range(4,7):
 index=0;g=b+struct.unpack_from('<I',r,b+4+group*4)[0]
 count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
 assert bits==16 and pals in (1,2) and count==1
 pal=g+20+count*4;pix=pal+32*pals
 assert pix+tiles*32<=len(r)
 a=b+struct.unpack_from('<I',r,g+20+index*4)[0]
 metrics=struct.unpack_from('<9H',r,a);tx,ty,ox,oy,cx,cy,w,h,tile=metrics
 assert 0<tx<=8 and 0<ty<=8 and tile+tx*ty<=tiles
 for bank in range(min(pals,2)):
  colors=struct.unpack_from('<16H',r,pal+bank*32);rgba=bytearray()
  for y in range(ty*8):
   for x in range(tx*8):
    p=pix+(tile+(y//8)*tx+x//8)*32+(y%8)*4+(x%8)//2
    i=(r[p]>>(4*(x&1)))&15;c=colors[i]
    rgba.extend([((c>>s)&31)*255//31 for s in (0,5,10)]+[255 if i else 0])
  rows.append(dict(group=group,index=index,palette_bank=bank,metrics=list(metrics),byte_offset=len(blob),byte_length=len(rgba),descriptor=hex(a+0x8000000),palette=hex(pal+bank*32+0x8000000)))
  blob.extend(rgba);write_png(out/f'label-{group}-{bank}.png',tx*8,ty*8,rgba)
(out/'labels.rgba').write_bytes(blob)
(out/'labels.crs').write_bytes(b'CRS1'+struct.pack('<I',len(rows))+b''.join(struct.pack('<9H',*row['metrics']) for row in rows))
base=0x49dfe0;texts=[]
for driver in range(2):
 ident=200+driver;offset=struct.unpack_from('<I',r,base+4+ident*4)[0];assert offset
 a=base+offset;data=r[a:r.index(0,a)];assert 0<len(data)<128
 texts.append(dict(city=driver,id=ident,address=hex(a+0x8000000),text=data.decode('latin1')))
(out/'labels.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,container='080520f8',source_palette_count=pals,frames_sha256=hashlib.sha256(blob).hexdigest(),frames=rows,texts=texts,qualification='Original groups4/5 frame0 selected bank0/unselected bank1; group6 frame0 single locked-city icon. Source anchors172/206,y2. Source text IDs200/201 at4/6. Native composition pending.'),indent=2)+'\n')
print('Original city labels:',len(rows),'frames',len(blob),'bytes; headings',texts)
