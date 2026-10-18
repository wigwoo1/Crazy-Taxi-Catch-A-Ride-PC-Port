"""Native collision data bundle: raw geometry plus the verified CTC1 grid.
CTW1 header: magic, u32 vertex/polygon/material counts, u32 grid byte size.
Then vertex16, polygon24, material28 arrays, followed by exact CTC1 bytes.
No executable ROM code or ROM pointers are retained.
"""
import argparse
import struct
from pathlib import Path
from extract_collision import extract
from extract_scene import BASE,SCENES

def world(data,scene):
    grid,_=extract(data,scene) # Includes revision/hash verification.
    h=data[SCENES[scene]-BASE:SCENES[scene]-BASE+104]
    counts=[struct.unpack_from('<H',h,o)[0] for o in (8,10,4)]
    arrays=[]
    for count,stride,offset in zip(counts,(16,24,28),(0x24,0x28,0x1c)):
        start=struct.unpack_from('<I',h,offset)[0]-BASE;end=start+count*stride
        if start<0 or end>len(data):raise ValueError('World reference outside ROM')
        arrays.append(data[start:end])
    return b'CTW1'+struct.pack('<4I',*counts,len(grid))+b''.join(arrays)+grid

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('rom',type=Path);p.add_argument('--output',type=Path,required=True)
    a=p.parse_args();data=a.rom.read_bytes();a.output.mkdir(parents=True,exist_ok=True)
    for scene in SCENES:
        out=world(data,scene);(a.output/(scene+'.ctw')).write_bytes(out)
        print(scene,len(out),'native collision bytes')
