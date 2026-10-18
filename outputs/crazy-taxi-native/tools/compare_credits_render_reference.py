"""Native compositor vs original instruction-produced tile map and raw ROM assets."""
exec(open('outputs/crazy-taxi-native/tools/compare_credits_flow_rom.py').read().split('\nrng=random.Random')[0])
# Decode ROM tile payloads independently of native-loaded asset buffers.
font=r[0xbd7ec:0xbe7ec];background=bytearray();cursor=0xb7118
for tile in range(250):
 block=bytearray()
 while len(block)<32:
  cmd=r[cursor];cursor+=1;size=cmd&127;assert 0<size<=32-len(block)
  if cmd&128:block.extend(r[cursor:cursor+size]);cursor+=size
  else:block.extend([r[cursor]]*size);cursor+=1
 background.extend(block)
assert cursor==0xb8a7a
maps=[list(struct.unpack_from('<1024H',r,off)) for off in (0xb92a0,0xb8a7c)]
pal_sources=(0xbe7ec,0xb927c,*[0xbe7ec+32*k for k in range(5)])
palette=[struct.unpack_from('<16H',r,off) for off in pal_sources]
trig=(root/'assets/driving.ctr').read_bytes();assert trig[:4]==b'CTR1'
state=(45056,0,0,0,704,0,0,0);steps=0
for tick in range(320):
 sine=struct.unpack_from('<h',trig,4+state[3]*2)[0];oldrow=state[4]
 actual,count=original_frame((*state,1,sine,1,0,0));steps+=count
 if actual[11]:maps[0][oldrow:oldrow+30]=actual[15:]
 state=tuple(actual[:8]);scroll=actual[12:15]
# BG0 priority0, BG1 priority1 from original5778; no mosaic/alpha/fade
# context for this explicit diagnostic. Index0 transparent, backdrop BG color0.
expected=[]
for y in range(160):
 for x in range(240):
  color=palette[0][0]
  for layer in (1,0):
   if layer:sx=(x+scroll[1])%256;sy=(y+scroll[2])%256;tiles=background
   else:sx=x;sy=(y+scroll[0])%256;tiles=font
   e=maps[layer][(sy//8)*32+sx//8];tile=e&1023
   tx=sx%8;ty=sy%8
   if e&1024:tx=7-tx
   if e&2048:ty=7-ty
   idx=(tiles[tile*32+ty*4+tx//2]>>(4*(tx%2)))&15
   bank=1 if layer else e>>12
   if idx:color=palette[bank][idx]
  expected.append(color)
cp=subprocess.run([str(root/'bin/credits-render-check.exe'),'320',str(root/'reports/credits-native-frame320.rgb555')],capture_output=True,text=True,check=True)
actual=list(struct.unpack('<38400H',(root/'reports/credits-native-frame320.rgb555').read_bytes()));assert actual==expected
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'running_frames':320,'thumb_instructions':steps,'equal_rgb555_pixels':38400,'scroll':scroll,'scope':'Actual original running-frame instructions build font tile map/scroll at320 ticks using actual extracted runtime sine values. Independent raw-ROM tile/maps/RLE/palette reference compared native C asset loader/compositor38400pixels. BGpriority0/1 and simple transparent-index0/backdrop sampling explicit no-mosaic/no-alpha/no-fade diagnostic. This is not captured original GBA whole-frame pixel oracle or fullcredits entry/fade/IRQ/return verification.','source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/credits_asset.c','src/credits_asset.h','src/credits_flow.c','tests/credits_render_check.c','tools/compare_credits_render_reference.py']}}
(root/'reports/credits-render-reference-comparison.json').write_text(json.dumps(report,indent=2));print(cp.stdout.strip());print('PASS',report)
