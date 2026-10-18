import json
import struct
import subprocess

from pathlib import Path
root=Path(__file__).resolve().parents[1]
exe=root/'bin/passenger-asset-check.exe'
for scene,expected in [('city1','128 128 33'),('city2','128 128 27'),('startup','0 0 0')]:
    result=subprocess.run([str(exe),str(root/'assets'/f'{scene}.ctp')],text=True,capture_output=True,check=True)
    assert result.stdout.strip()==expected
data=(root/'assets/city1.ctp').read_bytes()
count,routes,destinations=struct.unpack_from('<3I',data,4)
route_offset=16+count*32+destinations*48
def changed(offset,value):
    b=bytearray(data);struct.pack_into('<I',b,offset,value);return b
bad_destination=bytearray(data)
source=json.loads((root/'reports/passengers/city1-passengers.json').read_text())
customer=next(p['index'] for p in source['pedestrians'] if p['customer_candidate'])
bad_destination[16+customer*32+1]=255
second_route=route_offset+8+struct.unpack_from('<I',data,route_offset+4)[0]*16
invalid=[b'NOPE'+data[4:],data[:15],data[:-1],data+b'\0',changed(4,129),changed(8,1025),
         changed(12,257),bad_destination,changed(route_offset+4,0),changed(route_offset+4,4097),
         changed(second_route,struct.unpack_from('<I',data,route_offset)[0])]
directory=root/'reports/passenger-loader-fixtures'
directory.mkdir(exist_ok=True)
for i,raw in enumerate(invalid):
    path=directory/f'bad-{i}.ctp';path.write_bytes(raw)
    subprocess.run([str(exe),str(path),'expect-failure'],check=True)
print(f'PASS: three native city customer assets and {len(invalid)} malformed assets.')

