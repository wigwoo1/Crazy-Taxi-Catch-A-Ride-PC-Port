"""Preserve original Flag8150 HUD positions; no inferred sprite geometry."""
import hashlib,json,struct
from pathlib import Path
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
assert hashlib.sha256(rom).hexdigest()==expected
out=Path('outputs/crazy-taxi-native/assets/challenge-flag');out.mkdir(exist_ok=True)
blob=rom[0x7ec12c:0x7ec168];assert len(blob)==60
rows=[dict(identity=hex(0x087ec12c+i*12),position=list(struct.unpack_from('<3i',blob,i*12))) for i in range(5)]
(out/'flag-hud-positions.bin').write_bytes(blob)
assert (out/'flag-hud-positions.bin').read_bytes()==rom[0x7ec12c:0x7ec168]
(out/'flag-hud-positions.json').write_text(json.dumps(dict(rom_sha256=expected,source='0x087EC12C..0x087EC167',sha256=hashlib.sha256(blob).hexdigest(),records=rows,glyph_order='records4,3,2,1,0; source8150 reverses five character slots',font='existing assets/hud/clock.rgba original spritegroup3; bank0 from8150 argument3=0',scope='Exact source position triples;28288/28658 anchoring and affine/OAM fields still require qualification'),indent=2)+'\n')
print('PASS original Flag HUD positions60bytes, ROM SHA/readback exact',rows)

base=0x70e50;group=base+struct.unpack_from('<I',rom,base+16)[0]
assert struct.unpack_from('<5I',rom,group)==(26,16,48,269,2)
metrics=bytearray(b'CTF1'+struct.pack('<HH',26,0))
for i in range(26):
 address=base+struct.unpack_from('<I',rom,group+20+i*4)[0]
 metrics.extend(rom[address:address+18])
metrics.extend(bytes(26*26));(out/'flag-clock-metrics.ctf').write_bytes(metrics)
print('PASS originalgroup3 descriptor metrics26records/468sourcebytes with absent pair table')

w,h,pixels=struct.unpack_from('<HHI',rom,0x7ec364);assert (w,h,pixels)==(20,24,0x080eca8c)
frames=rom[pixels-0x08000000:pixels-0x08000000+w*h*3];assert len(frames)==1440
(out/'flag-indicator.indices').write_bytes(frames)
assert (out/'flag-indicator.indices').read_bytes()==rom[0xeca8c:0xeca8c+1440]
(out/'flag-indicator.json').write_text(json.dumps(dict(rom_sha256=expected,descriptor='0x087EC364',width=w,height=h,pixels=hex(pixels),variants=[1,2],variant_rule='vehicle+0x4AC bit5 clear:1; set:2',anchor='x4, viewportHeight-23',format='8-bit indexed, source04F6C copies opaque including index0; registered BG palette owner',sha256=hashlib.sha256(frames).hexdigest(),scope='Original frame0 retained, Flag uses1/2; viewport clip/registeredpalette application needs native integration'),indent=2)+'\n')
print('PASS original Flag indicator20x24,1440indexed bytes/source readback exact; opaque bitmap semantics')

# Source8032..8046 copies16 RGB555 colours from08290740 into timer OBJ bank6.
# Decode original tile indices again, preserving index identity (no RGB guessing).
palette=struct.unpack_from('<16H',rom,0x290740)
palbase=group+20+26*4;tiles=palbase+64
warning=bytearray()
for bank in range(2):
 colors=palette if bank==0 else struct.unpack_from('<16H',rom,palbase+32)
 for glyph in range(26):
  a=base+struct.unpack_from('<I',rom,group+20+glyph*4)[0]
  tx,ty,ox,oy,cx,cy,w,h,tile=struct.unpack_from('<9H',rom,a)
  frame=bytearray(1024)
  for y in range(ty*8):
   for x in range(tx*8):
    pos=tiles+(tile+(y//8)*tx+x//8)*32+(y%8)*4+(x%8)//2
    index=(rom[pos]>>(4*(x&1)))&15;color=colors[index]
    frame[(y*16+x)*4:(y*16+x+1)*4]=bytes([((color>>shift)&31)*255//31 for shift in (0,5,10)]+[255 if index else 0])
  warning.extend(frame)
assert len(warning)==53248
(out/'flag-clock-warning.rgba').write_bytes(warning)
(out/'flag-clock-warning.json').write_text(json.dumps(dict(rom_sha256=expected,palette='0x08290740',source='08008032..08008046: DMA16halfwords to050002C0 and retain pointer02001818',format='Originalgroup3 tile indices; bank0 warning colours, bank1 preserved',sha256=hashlib.sha256(warning).hexdigest()),indent=2)+'\n')
print('PASS original Flag warning glyphs53248bytes, direct4bpp index decode/RGB555 sourcepalette')
