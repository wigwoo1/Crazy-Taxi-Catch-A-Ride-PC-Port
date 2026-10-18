"""Actual original1556C retained shadow point and1534 submission gate."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');base_source=(root/'tools/verify_taxi_shadow_joined_renderer.py').read_text();results=[]
for gate in [0,1,255]:
 ns={};s=base_source.replace(';results=[]','')
 s=s.replace('p.arm.mem[0x02001708]=0;p.arm.mem[0x030053e0]=0','p.arm.mem[0x02001708]=0;p.arm.mem[0x030053e0]=0;p.arm.mem[0x02001534]=GATE')
 s=s.replace('p.reg[:3]=[base,0,0];','p.write(base+0xe8c,19008);p.reg[:3]=[base,0,0];')
 s=s.replace('stop=0x10020000)\n call=p.shadow_call;','stop=0x08015640)\n call=p.shadow_call;')
 a=s.index(' call=p.shadow_call;');s=s[:a]+"""
 assert p.read(base+0xe8c)==19008 and p.read(base+0xe88)==x&MASK and p.read(base+0xe90)==z&MASK
 called=hasattr(p,'shadow_call');assert called==(GATE==0),(GATE,ticks)
 results.append(dict(gate=GATE,ticks=ticks,shadow_submitted=called,retained_point=[signed(p.read(base+o)) for o in [0xe88,0xe8c,0xe90]],depth_offsets=p.depth_offsets))
"""
 ns.update(GATE=gate,results=results);exec(compile(s,'shadow-gate-original','exec'),ns)
assert len(results)==15,len(results)
report=dict(status='PASS',cases=results,scope='Actual original1556C through15640, same retainedpointread/write and31A50call on1534zero; gates1/255 skipshadow but stillupdatepoint. Groundheightreturn explicit19200contract, division/depthsetter/allocatorcontracts inherited; no entireframe/gameclaim.')
(root/'reports/shadow-branch-gate-verification.json').write_text(json.dumps(report,indent=2));print('PASS',len(results),'branch cases')
