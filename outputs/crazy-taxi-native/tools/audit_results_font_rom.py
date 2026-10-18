"""Verify exported results glyph pixels/metrics directly against immutable ROM."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json
screen=sys.argv[1] if len(sys.argv)>1 else "results"
root,slots={"results":(0x64f38,3),"attract":(0x5a338,2)}[screen]
rows=[];base_bank=0;total=0
for slot in range(slots):
 g=root+struct.unpack_from('<I',r,root+4+4*slot)[0]
 count,colors,tiles,start,pals=struct.unpack_from('<5I',r,g)
 assert colors==16
 palette=g+20+count*4;pixels=palette+32*pals
 exported=pathlib.Path(f'outputs/crazy-taxi-native/assets/{screen}/font{slot}.rgba').read_bytes()
 metrics=pathlib.Path(f'outputs/crazy-taxi-native/assets/{screen}/font{slot}-metrics.ctf').read_bytes()
 assert len(exported)==count*pals*4096 and metrics[:4]==b'CTF1'
 assert struct.unpack_from('<H',metrics,4)[0]==count
 for glyph in range(count):
  descriptor=root+struct.unpack_from('<I',r,g+20+glyph*4)[0]
  m=struct.unpack_from('<9H',r,descriptor)
  assert metrics[8+glyph*18:8+(glyph+1)*18]==r[descriptor:descriptor+18]
  w,h=m[0]*8,m[1]*8;tile=m[8]
  assert w<=32 and h<=32 and tile+m[0]*m[1]<=tiles
  for bank in range(pals):
   values=struct.unpack_from('<16H',r,palette+bank*32)
   for y in range(32):
    for x in range(32):
     expected=bytes(4)
     if x<w and y<h:
      address=pixels+32*(tile+(y//8)*m[0]+x//8)+(y&7)*4+(x&7)//2
      idx=(r[address]>>(4*(x&1)))&15;v=values[idx]
      expected=bytes([((v>>shift)&31)*255//31 for shift in (0,5,10)]+[255 if idx else 0])
     off=((bank*count+glyph)*1024+y*32+x)*4
     assert exported[off:off+4]==expected,(slot,bank,glyph,x,y)
     total+=1
 rows.append(dict(slot=slot,group=hex(g+0x8000000),glyphs=count,palettes=pals,registered_palette_base=base_bank,tiles=tiles,tile_start=start,source_palette=hex(palette+0x8000000),rgba_sha256=hashlib.sha256(exported).hexdigest()))
 base_bank+=pals
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),groups=rows,pixels_verified=total,qualification='Exact raw container RGB555 palettes, tile data and metrics. Registered bases are cumulative27994 model; later palette mutations and OAM geometry not proven by this audit.')
p=pathlib.Path(f'outputs/crazy-taxi-native/reports/{screen}-font-rom-audit.json');p.write_text(json.dumps(report,indent=2)+'\n');assert json.loads(p.read_text())==report
print(f'PASS {screen}: all{slots} groups, {total} original-ROM pixels and metrics; no extraction mismatch.')
