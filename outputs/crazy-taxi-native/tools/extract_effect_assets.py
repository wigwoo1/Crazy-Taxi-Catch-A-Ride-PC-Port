"""Decode original effect groups, preserving both containers and palette banks."""
import pathlib,struct,hashlib,json
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
sha=hashlib.sha256(r).hexdigest()
assert sha=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
out=pathlib.Path('outputs/crazy-taxi-native/assets/effects');out.mkdir(exist_ok=True)
rows=[];blob=bytearray()
native_records=bytearray()
for name,root in [('normal',0x70e50),('attract',0x5a338)]:
 palette_bases=[];palette_base=0
 for registered_group in range(struct.unpack_from('<I',r,root)[0]):
  registered_header=root+struct.unpack_from('<I',r,root+4+registered_group*4)[0]
  palette_bases.append(palette_base)
  palette_base+=struct.unpack_from('<I',r,registered_header+16)[0]
 for group in (8,9,10):
  g=root+struct.unpack_from('<I',r,root+4+group*4)[0]
  count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g)
  assert 0<count<256 and bits==16 and 0<pals<=16
  pal=g+20+count*4;pix=pal+32*pals
  for frame in range(count):
   a=root+struct.unpack_from('<I',r,g+20+frame*4)[0]
   m=struct.unpack_from('<9H',r,a);tx,ty,ox,oy,cx,cy,w,h,tile=m
   assert 0<tx<=8 and 0<ty<=8 and tile+tx*ty<=tiles
   for bank in range(pals):
    colors=struct.unpack_from('<16H',r,pal+bank*32);rgba=bytearray()
    for y in range(ty*8):
     for x in range(tx*8):
      p=pix+(tile+y//8*tx+x//8)*32+y%8*4+x%8//2
      idx=(r[p]>>(4*(x&1)))&15;c=colors[idx]
      rgba.extend([((c>>s)&31)*255//31 for s in (0,5,10)]+[255 if idx else 0])
    write_png(out/f'{name}-g{group}-f{frame}-p{bank}.png',tx*8,ty*8,rgba)
    # Source e4ec..e56a scans nontransparent indices1..15, strict ties
    # retain the first entry. RGB555 bit15 is not a color component.
    brightness={i:sum((colors[i]>>s)&31 for s in (0,5,10)) for i in range(1,16)}
    darkest=min(brightness,key=brightness.get);brightest=max(brightness,key=brightness.get)
    rows.append(dict(container=name,root=hex(root+0x8000000),group=group,header=[count,bits,tiles,start,pals],frame=frame,bank=bank,registered_palette_bank=palette_bases[group]+bank,darkest_index=darkest,brightest_index=brightest,metrics=list(m),descriptor=hex(a+0x8000000),palette=hex(pal+bank*32+0x8000000),offset=len(blob),size=len(rgba),sha256=hashlib.sha256(rgba).hexdigest()))
    native_records.extend(struct.pack('<4B9H2H2xII',name=='attract',group,frame,bank,*m,tx*8,ty*8,len(blob),len(rgba)))
    blob.extend(rgba)
(out/'sprites.rgba').write_bytes(blob)
native=b'CTE1'+struct.pack('<II',len(rows),len(blob))+native_records+blob
assert len(native_records)==36*len(rows)
(out/'sprites.cte').write_bytes(native)
assert (out/'sprites.cte').read_bytes()==native
(out/'sprites.json').write_text(json.dumps(dict(rom_sha256=sha,frames=rows,rgba_sha256=hashlib.sha256(blob).hexdigest(),qualification='Original container palettes only; runtime palette overrides/registration remain to verify. Native drawing not integrated.'),indent=2)+'\n')
print(f'Decoded {len(rows)} frames/banks, {len(blob)} RGBA bytes')
print(f'Native CTE1 bundle {len(native)} bytes SHA256 {hashlib.sha256(native).hexdigest()}')
