import sys
import struct
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from extract_passenger_sprites import extract
data=Path(sys.argv[1]).read_bytes();banks=extract(data)
assert len(banks)==32 and sum(b[3] for b in banks)==144
assert all(len(b[4])==512 for b in banks)
for kind,stage,address,frames,palette,pixels in banks:
    raw=data[address-0x08000000:address-0x08000000+frames*2048]
    assert len(pixels)==frames*32*64*4
    assert sum(p==0 for p in raw)==sum(pixels[i]==0 for i in range(3,len(pixels),4))
    for i in [0,31,32,1024,len(raw)-1]:
        color=struct.unpack_from('<H',palette,2*raw[i])[0]
        assert tuple(pixels[i*4:i*4+3])==tuple(((color>>shift)&31)*255//31 for shift in (0,5,10)) or tuple(pixels[i*4:i*4+3])==tuple((c<<3)|(c>>2) for c in [(color>>shift)&31 for shift in (0,5,10)])
try:extract(data[:-1])
except ValueError:pass
else:raise AssertionError('Wrong ROM accepted')
print('PASS: 32 original banks, 144 linear frames, embedded palettes, sampled colors and transparency.')

