"""Extract source scene+2c 32-byte billboard records; no executable code."""
import argparse
import hashlib
import json
import struct
from pathlib import Path
from extract_scene import BASE, SCENES, EXPECTED

def extract(data, scene):
    if hashlib.sha256(data).hexdigest() != EXPECTED:
        raise ValueError('Unsupported ROM revision')
    header=SCENES[scene]-BASE
    count=struct.unpack_from('<H',data,header+12)[0]
    address=struct.unpack_from('<I',data,header+44)[0]
    start=address-BASE
    if count>16384 or (count and (start<0 or start+count*32>len(data))):
        raise ValueError('Special world records outside ROM')
    records=data[start:start+count*32] if count else b''
    return b'CTS1'+struct.pack('<I',count)+records, {'scene':scene,
        'rom_sha256':EXPECTED,'address':hex(address),'count':count,'stride':32,
        'records_sha256':hashlib.sha256(records).hexdigest(),
        'source':'ARM080038d0..08003aa8; material byte0, XYZ+4, rectangle offsets+16/+20/+24/+28'}

if __name__=='__main__':
    parser=argparse.ArgumentParser()
    parser.add_argument('rom',type=Path)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();data=args.rom.read_bytes()
    args.output.mkdir(parents=True,exist_ok=True)
    metadata=[]
    for scene in SCENES:
        bundle,info=extract(data,scene)
        (args.output/(scene+'-special.cts')).write_bytes(bundle)
        metadata.append(info);print(scene,info['count'],'original special records')
    (args.output/'world-special.json').write_text(json.dumps(metadata,indent=2))
