"""Static sound flag references and qualified direct playback callsites.
Literal scans are inventory, not proof against indirect/ARM writes.
"""
from pathlib import Path
import hashlib,json,struct,sys
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
root=Path('outputs/crazy-taxi-native')
r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
cs=Cs(CS_ARCH_ARM,CS_MODE_THUMB);refs=[]
for o in range(0,0x40000,2):
 h=struct.unpack_from('<H',r,o)[0]
 if h&0xf800!=0x4800:continue
 lit=((o+4)&~3)+(h&255)*4
 value=struct.unpack_from('<I',r,lit)[0]
 if value not in (0x20025d0,0x20025d1):continue
 refs.append(dict(address=hex(0x8000000+o),literal=hex(0x8000000+lit),value=hex(value),instructions=[dict(address=hex(i.address),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str) for i in cs.disasm(r[o:o+16],0x8000000+o)]))
assert {x['address'] for x in refs}=={'0x8010d5c','0x8010d7c','0x80114da','0x80114de'}
direct=[]
for addr,index in [(0x80130fe,25),(0x8013124,43),(0x80132ac,25),(0x80132d2,43),(0x80142a2,24)]:
 o=addr-0x8000000;ins=list(cs.disasm(r[o:o+4],addr));assert len(ins)==1 and ins[0].mnemonic=='bl' and ins[0].op_str=='#0x80267c8'
 flag_args=list(cs.disasm(r[o-4:o],addr-4))
 assert [(i.mnemonic,i.op_str) for i in flag_args]==[('movs','r1, #0'),('movs','r2, #0')],hex(addr)
 # Recover table pointer from preceding literal load; index43 adds0xAC.
 load=next(i for i in cs.disasm(r[o-10:o],addr-10) if i.mnemonic=='ldr' and '[pc,' in i.op_str)
 h=struct.unpack_from('<H',r,load.address-0x8000000)[0];lit=((load.address+4)&~3)+(h&255)*4
 assert struct.unpack_from('<I',r,lit-0x8000000)[0]==0x87ec658
 direct.append(dict(call=hex(addr),source_index=index,instructions=[dict(address=hex(i.address),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str) for i in cs.disasm(r[o-10:o+4],addr-10)]))
report=dict(rom_sha256=hashlib.sha256(r).hexdigest(),thumb_literal_scan_range='08000000..0803FFFF (inventory, not CFG)',deferred_flag_references=refs,direct_driving_calls=direct,conclusion='Keep existing direct gear25/43 and brake24 requests at original driver positions. Do not convert them into deferred25D0/1 flags or suppress them solely on matching sound indices. Flags retain qualified reset values and controller clear semantics; no nonzero producer established by this scan.',limitations='Does not exclude indirect/ARM or base-relative flag writers, and does not prove all direct calls in ROM.',source_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest())
(root/'reports/game-audio-event-ownership-audit-y-20261014.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS four flag literal references and five direct gear/brake playback sites; no full-game write proof')
