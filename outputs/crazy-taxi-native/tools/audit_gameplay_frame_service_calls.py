from pathlib import Path
import sys,struct,json,hashlib
sys.path.insert(0,'work/python-packages')
from capstone import *
root=Path('outputs/crazy-taxi-native');r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();digest=hashlib.sha256(r).hexdigest();assert digest=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
m=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
def ins(a):
 i=next(m.disasm(r[a:a+4],0x8000000+a,count=1));d=dict(address=hex(i.address),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)
 h=int.from_bytes(i.bytes[:2],'little')
 if h&0xf800==0x4800:d['literal']=hex(struct.unpack_from('<I',r,((a+4)&~3)+(h&255)*4)[0])
 return d
calls=[]
for a in range(0xaf80,0xb580,2):
 h,l=struct.unpack_from('<HH',r,a)
 if h&0xf800==0xf000 and l&0xf800==0xf800:calls.append(ins(a))
by={int(x['address'],16)-0x8000000:x for x in calls}
for a,target in [(0xb022,0x8011a3c),(0xb2ae,0x8011a3c),(0xb2e8,0x8011a3c),(0xb0e0,0x800cb84),(0xb128,0x802360c),(0xb57c,0x8026318)]:assert by[a]['operands']==hex(target).join(['#',''])
# Decode direct tail instructions individually; no accidental decoding across literal pools.
tail=[ins(a) for a in range(0xb570,0xb590,2) if a!=0xb57a and a!=0xb57e]
assert next(x for x in tail if x['address']=='0x800b58e')['operands']=='#0x800af80'
report=dict(status='PASS',rom_sha256=digest,main_loop_range='0800AF80..0800B58E',direct_calls=calls,tail=tail,confirmed_calls={'vehicle_update_branches':['0800B022','0800B2AE','0800B2E8'],'trip_timer':'0800B0E0 -> 0800CB84','pickup_orientation':'0800B128 -> 0802360C','frame_service':'0800B57C -> 08026318','loop_return':'0800B58E -> 0800AF80'},findings=['Main gameplay loop contains three conditional vehicle-update call sites to11A3C before its final frame-service26318 call. Static order does not imply all three execute on one iteration.','Trip timer and pickup orientation calls are also before the final frame service.','Frame service reads elapsed VBlank difference after its own wait and auxiliary calls, so the delta measures more than a fixed driving step.'],scope='Verified local call encodings and loop-back branch. Other enumerated BL-looking words are candidates, not a proven control-flow graph. No original execution cycle counts, runtime branch path, or universal driving cadence established.')
(root/'reports/gameplay-frame-service-call-order.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS vehicle-update branches, timer, pickup, final frame-service and loop-back encodings')
