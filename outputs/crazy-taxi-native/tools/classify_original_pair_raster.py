from pathlib import Path
import json,hashlib,collections,sys
sys.path.insert(0,'work/python-packages')
from capstone import *
r=Path('outputs/crazy-taxi-native');d=r/'reports/original-core-reference';cpu=json.loads((d/'cpuWindowsLeft.cpu-timing-trace.json').read_text())['records'];mapping=json.loads((r/'reports/offline-iwram-window-mapping.json').read_text());ticks={x['tick'] for x in mapping['mapped'] if 0x3004440<=int(x['saved_pc'],16)<0x30044b0};rows=[x for x in cpu if x['tick'] in ticks];assert len(rows)==101
rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();md=Cs(CS_ARCH_ARM,CS_MODE_ARM);ins=list(md.disasm(rom[0x4b5c:0x4ba0],0x8004b5c));by={x.address:x for x in ins};assert by[0x8004b74].mnemonic=='ldrb' and by[0x8004b74].op_str=='r0, [ip, r0]';assert by[0x8004b7c].mnemonic=='strhne' and by[0x8004b7c].op_str=='r0, [lr]';assert by[0x8004b78].op_str=='r0, r0, r0, lsl #8'
pages=collections.Counter();records=[]
for x in rows:
 regs=x['gprs'];assert not x['cpsr']&32;assert 0x8000000<=regs[12]<0x8800000;dest=regs[14];assert dest%2==0
 if 0x6000000<=dest<0x6009600:page=0
 elif 0x600a000<=dest<0x6013600:page=1
 else:raise AssertionError(hex(dest))
 pages[page]+=1;records.append(dict(tick=x['tick'],saved_pc=hex(regs[15]),texture_row=hex(regs[12]),destination=hex(dest),page=page,row_count_remaining=regs[9],pixel_count_remaining=regs[8],x_mask=regs[10]))
report=dict(status='PASS_ORIGINAL_BYTE_LOOKUP_PAIR_STORE_CLASSIFICATION',samples=len(rows),destination_page_samples=dict(pages),instructions=[dict(address=hex(x.address),mnemonic=x.mnemonic,operands=x.op_str) for x in ins],records=records,scope='101 exact ROM-mapped CPU samples in this loop are ARM mode; r12 points into original ROM and halfword-aligned r14 lies in one of the two240x160 indexed-color framebuffer spans. Exact instructions duplicate one byte into a halfword and conditionally write it, with2pixel/row loops. Classification supports rendering data movement, not fullsprite function identity, pixel correctness, instruction-entry/loop counts or cycle-cost model. SavedPC pipeline semantics remain qualified.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),d/'cpuWindowsLeft.cpu-timing-trace.json',r/'reports/offline-iwram-window-mapping.json']})
(r/'reports/original-pair-raster-classification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS101 mapped ARM samples: ROM bytes -> two framebuffer spans',dict(pages))
