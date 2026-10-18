"""CTR1: original driving data only, fixed-size native runtime bundle.
Magic; sine8192; cosine8192; normal angles16386; traction angles514;
two vehicle templates584 each; three GRID_01 points16 each (city1/city2/startup).
"""
import argparse
import hashlib
import struct
from pathlib import Path
from extract_scene import SCENES,EXPECTED,BASE

def extract(data):
    if hashlib.sha256(data).hexdigest()!=EXPECTED:raise ValueError('Unsupported ROM revision')
    def read(address,size):
        a=address-BASE
        if a<0 or a+size>len(data):raise ValueError('Runtime data outside ROM')
        return data[a:a+size]
    result=bytearray(b'CTR1')
    for address,size in [(0x84a0d08,8192),(0x84a2d08,8192),(0x87ec9ec,16386),
                         (0x87f09ee,514),(0x84a554c,584),(0x84a5794,584)]:result+=read(address,size)
    for scene in ('city1','city2','startup'):
        h=read(SCENES[scene],104);count=struct.unpack_from('<H',h,16)[0]
        paths=read(struct.unpack_from('<I',h,52)[0],count*20)
        for i in range(count):
            entry=paths[i*20:i*20+20]
            if entry[4:16].split(b'\0')[0]==b'GRID_01':
                result+=read(struct.unpack_from('<I',entry,16)[0],16);break
        else:raise ValueError('Missing original start marker')
    assert len(result)==34504
    return result

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('rom',type=Path);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();a.output.parent.mkdir(parents=True,exist_ok=True);a.output.write_bytes(extract(a.rom.read_bytes()))
    print('Wrote34504 bytes of native driving data; no executable code.')
