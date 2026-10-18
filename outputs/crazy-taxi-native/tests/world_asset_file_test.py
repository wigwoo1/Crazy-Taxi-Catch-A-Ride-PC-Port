"""Reject malformed runtime world assets without crashing or accepting them."""
import struct
import subprocess
import sys
from pathlib import Path
root=Path(__file__).resolve().parents[1]
scratch=Path(sys.argv[1]);scratch.mkdir(parents=True,exist_ok=True)
data=(root/'assets/city1.ctw').read_bytes()
cases={'short':data[:12],'trailing':data+b'x','truncated':data[:-1]}
def changed(offset,value):
    b=bytearray(data);struct.pack_into('<I',b,offset,value);return b
cases['huge_vertices']=changed(4,0xffffffff)
grid=20+struct.unpack_from('<I',data,4)[0]*16+struct.unpack_from('<I',data,8)[0]*24+struct.unpack_from('<I',data,12)[0]*28
cases['zero_cell_size']=changed(grid+12,0)
cases['huge_grid']=changed(grid+4,0xffffffff)
for name,content in cases.items():
    path=scratch/(name+'.ctw');path.write_bytes(content)
    result=subprocess.run([str(root/'bin/world-asset-check.exe'),str(path)])
    assert result.returncode==1,(name,result.returncode)
assert subprocess.run([str(root/'bin/world-asset-check.exe'),str(root/'assets/city1.ctw')]).returncode==0
print('PASS: runtime world loader accepts valid data and rejects6 malformed variants.')
