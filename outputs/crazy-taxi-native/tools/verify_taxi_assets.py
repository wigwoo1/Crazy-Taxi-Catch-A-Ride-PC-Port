"""Compare every current taxi pixel to direct bounded ROM tile decoding."""
import argparse,hashlib,json,struct
from pathlib import Path
from extract_taxis import EXPECTED,region,decode_frame
parser=argparse.ArgumentParser();parser.add_argument('rom',type=Path);parser.add_argument('assets',type=Path);parser.add_argument('report',type=Path)
args=parser.parse_args();rom=args.rom.read_bytes()
if hashlib.sha256(rom).hexdigest()!=EXPECTED:raise ValueError('Unexpected ROM revision')
report={'rom_sha256':EXPECTED,'characters':[],'scope':'Source tile/palette and frame ordering; not full OBJ affine/priority emulation'}
for driver in range(4):
 left,right,palette=struct.unpack('<III',region(rom,0x084a5514+driver*12,12))
 data=(args.assets/f'taxi-{driver}.rgba').read_bytes()
 if len(data)!=64*128*64*4:raise ValueError('Taxi RGBA asset size mismatch')
 for frame in range(64):
  expected=decode_frame(region(rom,left+frame*2048,2048),region(rom,right+frame*2048,2048),region(rom,palette,32))
  if data[frame*32768:(frame+1)*32768]!=expected:raise ValueError(f'ROM pixel mismatch driver{driver} frame{frame}')
 report['characters'].append({'driver':driver,'frames':64,'pixels':64*128*64,'rgba_sha256':hashlib.sha256(data).hexdigest()})
args.report.write_text(json.dumps(report,indent=2)+'\n');print('PASS original ROM taxi pixels: all4drivers/256views/2097152pixels, frame and half order preserved.')
