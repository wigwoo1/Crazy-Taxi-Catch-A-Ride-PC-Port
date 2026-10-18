"""Independent tile/nibble and transparency checks for native taxi extraction."""
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from extract_taxis import decode_frame

left=bytearray(2048);right=bytearray(2048)
left[0]=0x21;left[32]=0x43;left[256]=0x65;right[2047]=0x87
palette=b''.join(i.to_bytes(2,'little') for i in range(16))
image=decode_frame(left,right,palette)
def pixel(x,y):return tuple(image[(y*128+x)*4:(y*128+x+1)*4])
for x,y,index in [(0,0,1),(1,0,2),(8,0,3),(9,0,4),(0,8,5),(1,8,6),(126,63,7),(127,63,8)]:
    assert pixel(x,y)==(index*255//31,0,0,255)
assert pixel(64,0)==(0,0,0,0)
try:decode_frame(left[:-1],right,palette)
except ValueError:pass
else:raise AssertionError('Truncated half accepted')
print('PASS: taxi tile boundaries, nibble order, half placement, palette and transparency.')
