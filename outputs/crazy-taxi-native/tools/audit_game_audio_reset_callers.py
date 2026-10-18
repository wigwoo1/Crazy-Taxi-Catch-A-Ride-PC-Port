"""Static exact original setup sound-reset call ordering."""
from pathlib import Path
import hashlib,json,sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
root=Path('outputs/crazy-taxi-native');r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
cs=Cs(CS_ARCH_ARM,CS_MODE_THUMB);rows=[]
for start,stop,targets in [(0xabcc,0xabe8,[0x800b6ec,0x800df8c,0x80227ac,0x8011490,0x801ee28,0x802277c,0x8021c10]),(0xae5a,0xae72,[0x800b6ec,0x800df8c,0x80227ac,0x8011490,0x802954c,0x8021c10])]:
 ins=list(cs.disasm(r[start:stop],0x8000000+start));assert len(ins)==len(targets)
 assert [(i.mnemonic,i.op_str) for i in ins]==[('bl','#'+hex(t)) for t in targets]
 rows.append(dict(start=hex(0x8000000+start),stop=hex(0x8000000+stop),instructions=[dict(address=hex(i.address),bytes=i.bytes.hex(),target=hex(t)) for i,t in zip(ins,targets)]))
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),setup_call_chains=rows,conclusion='Both surveyed setup paths run B6EC session scalars then DF8C destination clear then227AC passenger initialization then11490 sound reset. OrdinaryAE66 follows with2954C then21C10; alternateABD8 follows with1EE28,2277C,21C10. Apply sound reset after actual passenger init, not before, and commit25D6/25D7/amplitude/pending flags back to shared owners. Reset is not channel cleanup.',limitations='Exact selected sequential call instructions only. Does not execute whole setup/callees, exclude indirect reset calls, prove all reentry or qualify10AAC exit order.',source_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest())
(root/'reports/game-audio-reset-caller-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS both original reset call chains: passenger initialization precedes sound reset')
