"""Exact original bootstrap owners; hardware copy remains a separate boundary."""
from pathlib import Path
import hashlib,json,struct,sys
sys.path.insert(0,'work/python-packages')
from capstone import *
root=Path('outputs/crazy-taxi-native');r=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();digest=hashlib.sha256(r).hexdigest();assert digest=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB)
def literal(address):
 h=struct.unpack_from('<H',r,address)[0];assert h&0xf800==0x4800
 return struct.unpack_from('<I',r,((address+4)&~3)+(h&255)*4)[0]
assert literal(0x264ec)==0x2007561
assert literal(0x26568)==0x20075f7
assert literal(0x26578)==0x2007818
assert literal(0x2657c)==0x2007568
assert literal(0x2776e)==0x200756c
rows=[]
for start,end in [(0x26496,0x264f6),(0x26564,0x2658e),(0x2776c,0x27796)]:
 for i in md.disasm(r[start:end],0x8000000+start):
  item={'address':hex(i.address),'bytes':i.bytes.hex(),'mnemonic':i.mnemonic,'operands':i.op_str}
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:item['literal']=hex(literal(i.address-0x8000000))
  rows.append(item)
report={'rom_sha256':digest,'instructions':rows,'buffer_base_from_bootstrap_r6':hex(literal(0x264ac)),'findings':['02007560 is processing gate: bootstrap sets it0 at2649C and1 at2658A. Separate02007561 copied-code initialization flag is checked at264F0 and set1 at2658C.','02007818 has two pointer/count entries: +0=r6 buffer pointer,+4=02007568 count,+8=r6+count pointer,+12=count. Music clear and per-voice mixing both use selected count at+4; +0 is not a second count.','020075F7 initialized to1 at2656C.','2776C uses separate0200756C handshake request, waits16208 until cleared and updatesprevious frameclock3524; it does not own7560 processing gate.'],'scope':'Exact local bootstrap/service source evidence. Full DMA/copy/configured count/interrupt scheduling not executed.'}
(root/'reports/audio-buffer-owners-source.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report['findings'])
