"""Exercise native scene parser rejection paths without opening a rendering window."""
import shutil
import struct
import subprocess
import sys
from pathlib import Path

project=Path(__file__).resolve().parents[1]
scratch=Path(sys.argv[1]).resolve()
(scratch/'bin').mkdir(parents=True,exist_ok=True)
(scratch/'assets').mkdir(exist_ok=True)
executable=scratch/'bin'/'scene-viewer.exe'
shutil.copyfile(project/'bin'/'scene-viewer.exe',executable)
original=(project/'assets'/'city1.ctm').read_bytes()
nv,nf,nt,nm=struct.unpack_from('<4I',original,4)
face_offset=20+nv*12
material_offset=face_offset+nf*44
texture_offset=material_offset+nm*8
cases=[]
cases.append(('truncated',original[:30]))
corrupt=bytearray(original);struct.pack_into('<I',corrupt,4,0xffffffff);cases.append(('oversized count',corrupt))
corrupt=bytearray(original);struct.pack_into('<H',corrupt,face_offset+4,65535);cases.append(('invalid vertex index',corrupt))
corrupt=bytearray(original);struct.pack_into('<I',corrupt,material_offset,nt+1);cases.append(('invalid texture reference',corrupt))
corrupt=bytearray(original);struct.pack_into('<I',corrupt,texture_offset,0xffffffff);cases.append(('oversized texture',corrupt))
cases.append(('trailing bytes',original+b'extra'))
for name,payload in cases:
    (scratch/'assets'/'city1.ctm').write_bytes(payload)
    result=subprocess.run([str(executable)],capture_output=True,text=True,timeout=10)
    assert result.returncode==1,(name,result.returncode,result.stdout,result.stderr)
    assert 'Invalid' in result.stderr or 'Trailing' in result.stderr,(name,result.stderr)
    print('PASS:',name,'rejected by native loader')
