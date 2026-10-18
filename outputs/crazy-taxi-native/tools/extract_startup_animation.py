"""Original state3 startup resource and background assets; no invented art."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
import sys
sys.path.insert(0,str(root/'tools'))
from png_writer import write_png
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
out=root/'assets/startup-animation';out.mkdir(exist_ok=True)
base=0x82104;assert struct.unpack_from('<I',r,base)[0]==1
g=base+struct.unpack_from('<I',r,base+4)[0]
count,bits,tiles,sprite_start,pals=struct.unpack_from('<5I',r,g)
assert (count,bits,tiles,sprite_start,pals)==(33,256,228,0,1)
pal=g+20+count*4;pix=pal+512
palette=struct.unpack_from('<256H',r,pal)
(out/'sprites.palette.rgb555').write_bytes(r[pal:pal+512]);(out/'sprites.tiles').write_bytes(r[pix:pix+tiles*64])
frames=[];blob=bytearray();ib=bytearray()
for frame in range(count):
 a=base+struct.unpack_from('<I',r,g+20+frame*4)[0];metrics=struct.unpack_from('<9H',r,a)
 tx,ty,ox,oy,cx,cy,width,height,tile=metrics
 assert 0<tx<=8 and 0<ty<=8 and tile+tx*ty<=tiles,(frame,metrics)
 indices=bytearray(r[pix+(tile+(y//8)*tx+x//8)*64+(y%8)*8+x%8] for y in range(ty*8) for x in range(tx*8))
 rgba=bytearray()
 for index in indices:
  color=palette[index];rgba.extend([(((color>>s)&31)<<3)|(((color>>s)&31)>>2) for s in (0,5,10)]+[255 if index else 0])
 frames.append({'frame':frame,'metrics':list(metrics),'descriptor':hex(a+0x8000000),'indices_offset':len(ib),'rgba_offset':len(blob),'width':tx*8,'height':ty*8,'indices_sha256':hashlib.sha256(indices).hexdigest()})
 ib.extend(indices);blob.extend(rgba);write_png(out/f'frame-{frame:02}.png',tx*8,ty*8,rgba)
(out/'sprites.indices').write_bytes(ib);(out/'sprites.rgba').write_bytes(blob);(out/'sprites.crs').write_bytes(b'CRS1'+struct.pack('<I',count)+b''.join(struct.pack('<9H',*f['metrics']) for f in frames))
# Actual1E860 source decoder validates each compressed BG tile (64 bytes).
entry=0x801e860;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x801e95c,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
backgrounds=[];steps=0;blocks=0;descriptor=0x340d8+7*132
for layer in range(2):
 mp,pal,source=[struct.unpack_from('<I',r,descriptor+field+layer*4)[0]-0x8000000 for field in (0,16,32)]
 palette_bytes,size,w,h=[struct.unpack_from('<H',r,descriptor+field+layer*2)[0] for field in (56,64,72,80)]
 assert palette_bytes==512 and size%64==0 and r[descriptor+124+layer]==1
 cursor=source;decoded=bytearray()
 for tile in range(size//64):
  start=cursor;block=bytearray()
  while len(block)<64:
   command=r[cursor];cursor+=1;n=command&127;assert n and len(block)+n<=64
   if command&128:block.extend(r[cursor:cursor+n]);cursor+=n
   else:block.extend([r[cursor]]*n);cursor+=1
  p=InitProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10001000:bytearray([165]*80)};p.scalars={}
  p.reg[0]=0x8000000+start;p.reg[1]=64;p.reg[2]=0x10001000;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
  assert bytes(p.regions[0x10001000][:64])==block and p.reg[0]==0x8000000+cursor
  assert p.regions[0x10001000][64:]==bytes([165]*16)
  decoded.extend(block);steps+=p.steps;blocks+=1
 entries=struct.unpack_from('<'+str(w*h)+'H',r,mp);colors=struct.unpack_from('<256H',r,pal);indices=bytearray();rgba=bytearray()
 for y in range(h*8):
  for x in range(w*8):
   e=entries[(y//8)*w+x//8];tile=e&1023;assert tile<size//64,(layer,tile,size)
   xx=7-x%8 if e&1024 else x%8;yy=7-y%8 if e&2048 else y%8
   index=decoded[tile*64+yy*8+xx];indices.append(index);color=colors[index]
   rgba.extend([(((color>>s)&31)<<3)|(((color>>s)&31)>>2) for s in (0,5,10)]+[255 if layer==0 or index else 0])
 for suffix,data in [('indices',indices),('rgba',rgba),('palette.rgb555',r[pal:pal+512]),('tiles',decoded),('map',r[mp:mp+w*h*2])]:
  (out/f'background-{layer}.{suffix}').write_bytes(data)
 write_png(out/f'background-{layer}.png',w*8,h*8,rgba)
 backgrounds.append({'layer':layer,'width':w*8,'height':h*8,'map':hex(mp+0x8000000),'palette':hex(pal+0x8000000),'compressed_tiles':hex(source+0x8000000),'compressed_end':hex(cursor+0x8000000),'indices_sha256':hashlib.sha256(indices).hexdigest()})
report={'status':'PASS','rom_sha256':hashlib.sha256(r).hexdigest(),'extractor_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),'resource':'08082104','background_descriptor':hex(descriptor+0x8000000),'sprite_group_header':[count,bits,tiles,sprite_start,pals],'frames':frames,'backgrounds':backgrounds,'source_decoder_blocks':blocks,'source_Thumb_instructions':steps,'scope':'Source resource33 raw8bpp sprite frames andtype7 two background tile maps/palettes/flip bits. All seven compressed64byte BG tile blocks checked against actual complete1E860 including source cursor/output guard. Sprite tile layout and BG map reconstruction static source format; no original sprite renderer, blend/scroll/priority/full animated screen pixel equivalence or live viewer claim. PNGs isolated asset previews; BG1 index0 transparent, BG0 preview opaque.','files':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in out.iterdir() if p.is_file() and p.name!='assets.json'}}
(out/'assets.json').write_text(json.dumps(report,indent=2)+'\n');(root/'reports/startup-animation-assets.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(frames),'sprite frames',len(backgrounds),'backgrounds',blocks,'source decoded tile blocks',steps,'Thumb instructions')
