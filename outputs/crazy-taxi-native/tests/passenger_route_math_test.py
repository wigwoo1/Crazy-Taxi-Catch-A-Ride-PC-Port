"""Independent integer oracle for native route arithmetic, not GBA execution."""
import json
import math
import random
import subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
def signed(n):
    return (n+2**31)%2**32-2**31
def divide(a,b):
    return signed((abs(a)//abs(b))*(-1 if (a<0)!=(b<0) else 1)) if b else 0
def length(points):
    total=0
    for a,b in zip(points,points[1:]):
        dx=signed(b[0]-a[0])>>8; dz=signed(b[1]-a[1])>>8
        total+=math.isqrt((dx*dx+dz*dz)%2**32)&65535
    return signed(total*256)
cases=[]
for scene in ['city1','city2']:
    source=json.loads((root/'reports/passengers'/f'{scene}-passengers.json').read_text())
    for route in source['routes']:
        points=[p[1:3] for p in route['points_raw']]
        cases.append((points,0,1000000,points[-1]))
rom_cases=len(cases)
rng=random.Random(0x2289e)
for i in range(2000):
    points=[(rng.randrange(-2**31,2**31),rng.randrange(-2**31,2**31)) for _ in range(rng.randrange(1,20))]
    minimum=rng.randrange(-2**31,2**31)
    maximum=minimum if i%4==0 else rng.randrange(-2**31,2**31)
    dest=(rng.randrange(-2**31,2**31),rng.randrange(-2**31,2**31))
    cases.append((points,minimum,maximum,dest))
lines=[];expected=[]
for points,minimum,maximum,dest in cases:
    route_length=length(points)
    bucket=divide(signed(maximum-minimum),5)
    category=min(4,divide(signed(route_length-minimum),bucket))&255
    dx=signed(dest[0]-points[0][0]);dz=signed(dest[1]-points[0][1])
    direct=math.isqrt((dx*dx+dz*dz)%2**32)&65535
    ratio=divide(signed(direct*100),route_length)
    detour=next((4-i for i,limit in enumerate([20,40,60,80]) if ratio<limit),0)
    expected.append((route_length,category,detour))
    lines.append(' '.join(map(str,[len(points),minimum,maximum,*dest,*[v for p in points for v in p]])))
process=subprocess.run([str(root/'bin/passenger-route-probe.exe')],input='\n'.join(lines)+'\n',
                       text=True,capture_output=True,check=True)
actual=[tuple(map(int,line.split())) for line in process.stdout.splitlines()]
assert len(actual)==len(expected)
for i,(a,b) in enumerate(zip(actual,expected)):
    assert a==b,(i,a,b)
report={'status':'Native arithmetic versus independent Python integer oracle; not original execution equivalence.',
        'rom_routes_checked':rom_cases,'randomized_cases':2000,'seed':hex(0x2289e),'passed':True,
        'limits':'Probe ranges and targets are diagnostic. Runtime eligibility and category meanings remain unresolved.'}
(root/'reports/passengers/route-math-check.json').write_text(json.dumps(report,indent=2)+'\n')
print(f'PASS: {rom_cases} extracted routes and 2000 deterministic arithmetic cases.')
