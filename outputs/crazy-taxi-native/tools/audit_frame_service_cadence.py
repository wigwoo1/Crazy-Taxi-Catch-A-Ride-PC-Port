from pathlib import Path
import sys,struct,json,hashlib
sys.path.insert(0,'work/python-packages')
from capstone import *
root=Path('outputs/crazy-taxi-native')
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
m=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
def instruction(a):
 i=next(m.disasm(r[a:a+4],0x8000000+a,count=1))
 row=dict(address=hex(i.address),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str)
 h=int.from_bytes(i.bytes[:2],'little')
 if h&0xf800==0x4800:row['literal']=hex(struct.unpack_from('<I',r,((a+4)&~3)+(h&255)*4)[0])
 return row
rows=[instruction(a) for a in list(range(0x16208,0x16244,2))+[0x26318,0x2631a,0x2631c,0x26348,0x263c6,0x263ca,0x263ce]+list(range(0x263d2,0x2641e,2))]
by={int(x['address'],16)-0x8000000:x for x in rows}
assert by[0x2631a]['operands']=='r0, #1' and by[0x2631c]['operands']=='#0x8016208'
assert by[0x263d4]['literal']=='0x2003520' and by[0x263d6]['literal']=='0x2003524'
assert by[0x263dc]['mnemonic']=='subs' and by[0x263e2]['operands']=='r0, #0x3c'
assert by[0x16218]['operands']=='r2, r4' and by[0x1623c]['operands']=='#0x8016222'
report=dict(status='PASS',rom_sha256=hashlib.sha256(r).hexdigest(),instructions=rows,findings=['Frame service26318 explicitly requests ONE fresh VBlank from16208, not five.','16208 truncates requested count to8bits, waits only when hardware enable equals1 and loops according to the supplied count.','Frame service runs calls15C3C,EE10,27CD0,28EE0 before reading actual counter3520 minusprevious3524; it clamps signed difference to1..60.','Observed4..7 tick driving deltas cannot be justified as an explicit five-event wait in this service. The elapsed work before counter sampling must be considered.'],scope='Static local ROM instruction/literal checks only. Does not measure instruction cycles or recover a general variable driving scheduler. Full caller timing and rendering/audio costs remain unresolved.')
(root/'reports/frame-service-cadence-source-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS local frame-service audit; one-event wait and elapsed counter sampled after service calls')
