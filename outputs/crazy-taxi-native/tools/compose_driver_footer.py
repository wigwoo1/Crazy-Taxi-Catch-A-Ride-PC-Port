"""Settled BG0 and source semi-transparent footer; foreground excluded."""
import pathlib,hashlib,struct,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
out=pathlib.Path('outputs/crazy-taxi-native/assets/driver-selection')
bg=(out/'background.indices').read_bytes();assert len(bg)==512*512
bgpal=struct.unpack_from('<16H',r,0xa92f0)
b=0x49c0c;g=b+struct.unpack_from('<I',r,b+4+3*4)[0]
count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g);assert bits==16 and pals==2
pal=g+20+count*4;pix=pal+32*pals;objpal=struct.unpack_from('<16H',r,pal)
frames=[]
for frame in (1,2):
 a=b+struct.unpack_from('<I',r,g+20+frame*4)[0];m=struct.unpack_from('<9H',r,a)
 tx,ty,ox,oy,cx,cy,w,h,tile=m
 assert (tx*8,ty*8)==((32,16) if frame==1 else (8,16)) and ox==oy==0 and tile+tx*ty<=tiles
 indices=[]
 for y in range(ty*8):
  for x in range(tx*8):
   p=pix+(tile+(y//8)*tx+x//8)*32+(y%8)*4+(x%8)//2
   indices.append((r[p]>>(4*(x&1)))&15)
 frames.append(indices)
(out/'background.rgb555').write_bytes(struct.pack('<16H',*bgpal))
(out/'footer.rgb555').write_bytes(struct.pack('<16H',*objpal))
(out/'footer.indices').write_bytes(bytes(frames[0]+frames[1]))
records=[]
for driver in range(4):
 sx=(driver&1)*256;sy=(driver//2)*256
 layers=[(bgpal[bg[(sy+y)*512+sx+x]],0,False) for y in range(160) for x in range(240)]
 mixed=opaque=0
 for x0,width,frame in [(i*32,32,0) for i in range(5)]+[(160,8,1)]:
  for y in range(16):
   for x in range(width):
    index=frames[frame][y*width+x]
    if not index:continue
    bgindex=bg[(sy+144+y)*512+sx+x0+x];blend=bool(bgindex)
    layers[(144+y)*240+x0+x]=(objpal[index],bgpal[bgindex],blend)
    mixed+=blend;opaque+=not blend
 rgba=bytearray()
 for a,c,blend in layers:
  rgba.extend([(((a>>s)&31)+((c>>s)&31))//2*255//31 if blend else ((a>>s)&31)*255//31 for s in (0,5,10)]+[255])
 assert len(rgba)==153600
 (out/f'panel-prompts-{driver}.rgba').write_bytes(rgba)
 write_png(out/f'panel-prompts-{driver}.png',240,160,rgba)
 phases=bytearray()
 for cap in range(32):
  for a,c,blend in layers:
   phases.extend([((min((a>>s)&31,cap)+min((c>>s)&31,cap))//2 if blend else min((a>>s)&31,cap))*255//31 for s in (0,5,10)]+[255])
 assert len(phases)==4915200 and phases[-153600:]==rgba
 assert not any(phases[i] for i in range(153600) if i%4!=3)
 (out/f'panel-fade-{driver}.rgba').write_bytes(phases)
 records.append(dict(driver=driver,blended=mixed,opaque_over_backdrop=opaque,rgba_sha256=hashlib.sha256(rgba).hexdigest()))
(out/'footer.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,container='08049c0c',palette=hex(pal+0x8000000),callback_qualification='normal driver callback, excludes172e1/19b7d',blend_control='1f40',coefficients=[8,8],records=records,qualification='Settled BG0/footer only; foreground, dynamic scroll and palette fade composition pending.'),indent=2)+'\n')
print('Original driver footer composition:',records)
