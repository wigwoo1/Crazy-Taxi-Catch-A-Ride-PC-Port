"""Production native approach versus bounded original ARM code on identical inputs."""
import sys,json,subprocess,hashlib
from pathlib import Path
sys.path.insert(0,'outputs/crazy-taxi-native/tools')
from original_angle_approach_owner import execute_angle_owner
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
cases=[(a&0xffffffff,b&0xffffffff,s) for a in (-4096,-2048,-1,0,1,2048,4095,8192) for b in (-4096,-2048,-1,0,128,2048,4095,8192) for s in (0,1,32,2048,4096)]
exe=root/'bin/retained-angle-approach-compare.exe'
p=subprocess.run([str(exe)],input=''.join(f'{a} {b} {s}\n' for a,b,s in cases),capture_output=True,text=True,check=True)
rows=list(map(int,p.stdout.split()));assert len(rows)==len(cases)
for case,native in zip(cases,rows):
 regs=[0]*16;regs[:3]=case;regs[13]=0x10010000;regs[14]=0x10020000
 original=execute_angle_owner(rom,regs)['registers'][0];assert native==original,(case,native,original)
report=dict(match=True,cases=len(cases),rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Production traffic_heading scalar versus bounded original ARM531C+52B4 on wrap and step fixtures. Not captured shared gameplay or full routine ABI verification.',sha256={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [exe,root/'src/traffic_heading.c',root/'tools/original_angle_approach_owner.py']})
(root/'reports/retained-angle-approach-native-fixture-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS native/original approach',len(cases))
