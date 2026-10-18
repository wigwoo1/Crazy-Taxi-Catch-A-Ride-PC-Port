"""Verify original challenge music pointers, selection call and exported module."""
import pathlib,hashlib,struct,json
rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
module=pathlib.Path('outputs/crazy-taxi-native/assets/music/original-module.bin').read_bytes()
pointer=struct.unpack_from('<I',rom,0x7ec048)[0]
assert pointer==0x8421fc0
assert module==rom[pointer-0x8000000:pointer-0x8000000+len(module)]
orders=list(rom[0x7ec060:0x7ec065]);assert orders==[63,88,115,34,5]
for offset,target in [(0x74fe,0x8026ef8),(0x7508,0x801e72c),(0x751e,0x8027110),(0x7a02,0x8027730),(0x7adc,0x8027730),(0x7ae4,0x8026ef8),(0x7aec,0x8027110),(0x24728,0x8027730),(0x24730,0x8026ef8),(0x24738,0x8027110)]:
 a,b=struct.unpack_from('<HH',rom,offset);assert a&0xf800==0xf000 and b&0xf800==0xf800
 delta=((a&2047)<<12)|((b&2047)<<1)
 if delta&(1<<22):delta-=1<<23
 assert 0x8000000+offset+4+delta==target
resolved=[module[6+order] for order in orders];assert all(x<module[0] for x in resolved)
report=dict(module_pointer=hex(pointer),module_sha256=hashlib.sha256(module).hexdigest(),orders=orders,patterns=resolved,scope='ROM byte/source-call verification; full live shared RNG ordering and audible playback not proven')
pathlib.Path('outputs/crazy-taxi-native/reports/challenge-music-order-verification.json').write_text(json.dumps(report,indent=2))
print(report)
