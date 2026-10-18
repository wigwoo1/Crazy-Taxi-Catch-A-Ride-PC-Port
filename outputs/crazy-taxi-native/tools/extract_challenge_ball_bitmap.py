"""Original Crazy Ball indexed bitmap; no redraw or runtime palette claim."""
import hashlib,json,pathlib,struct,sys
sys.path.insert(0,'outputs/crazy-taxi-native/tools')
from png_writer import write_png
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
expected='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5';assert hashlib.sha256(rom).hexdigest()==expected
address=0x0849b0c9;offset=address-0x08000000
assert struct.unpack_from('<4H',rom,offset-520)==(32,256,256,0)
# Literal selected by reachable source234D0 for submode2.
op=struct.unpack_from('<H',rom,0x234d0)[0];assert op&0xf800==0x4800
literal=((0x234d0+4)&~3)+(op&255)*4;assert struct.unpack_from('<I',rom,literal)[0]==address
palette=rom[offset-512:offset];indices=rom[offset:offset+8192]
colors=[]
for i,(color,) in enumerate(struct.iter_unpack('<H',palette)):
 channels=[(color>>shift)&31 for shift in (0,5,10)]
 colors.append(bytes([(c<<3)|(c>>2) for c in channels]+[255 if i else 0]))
rgba=b''.join(colors[i] for i in indices)
out=pathlib.Path('outputs/crazy-taxi-native/assets/challenge-ball');out.mkdir(exist_ok=True)
for name,data in [('balloon.indices',indices),('balloon.palette',palette),('balloon.rgba',rgba)]:
 (out/name).write_bytes(data);assert (out/name).read_bytes()==data
write_png(out/'balloon.png',32,256,rgba)
report=dict(rom_sha256=expected,bitmap_pointer=hex(address),width=32,height=256,frame_width=32,frame_height=32,frames=8,palette_sha256=hashlib.sha256(palette).hexdigest(),indices_sha256=hashlib.sha256(indices).hexdigest(),rgba_sha256=hashlib.sha256(rgba).hexdigest(),scope='Original234D0-selected balloon atlas; embedded palette preview, not verified live palette/lighting or sprite placement.')
(out/'balloon.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS Crazy Ball original bitmap32x256: pointer/literal/header/palette/8192indices/readback verified.')
