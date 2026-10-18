from pathlib import Path
import sys,struct,json,hashlib
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_ARM
root=Path('outputs/crazy-taxi-native'); rp=Path('Crazy Taxi - Catch a Ride (USA).gba');rom=rp.read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
ramfile=root/'reports/original-core-reference/cpuWindowsLeft.03000000.bin';ram=ramfile.read_bytes();decoder=Cs(CS_ARCH_ARM,CS_MODE_ARM)
rows=[]
for ins in decoder.disasm(rom[0x1304:0x1434],0x8001304):
 row=dict(address=hex(ins.address),bytes=ins.bytes.hex(),mnemonic=ins.mnemonic,operands=ins.op_str)
 w=int.from_bytes(ins.bytes,'little')
 if (w>>16)&15==15 and ins.mnemonic=='ldr':
  a=ins.address-0x8000000;ptr=a+8+((w&4095) if w&(1<<23) else -(w&4095));off=ptr-0x6f8
  assert 0<=off<=len(ram)-4
  row.update(literal_rom_address=hex(0x8000000+ptr),literal_rom_value=hex(struct.unpack_from('<I',rom,ptr)[0]),literal_live_address=hex(0x3000000+off),literal_final_capture_value=hex(struct.unpack_from('<I',ram,off)[0]))
 rows.append(row)
by={int(x['address'],16):x for x in rows}
assert by[0x8001318]['literal_final_capture_value']=='0xa0'
assert by[0x8001324]['literal_final_capture_value']=='0x0'
assert by[0x80013a0]['literal_final_capture_value']=='0x0'
assert by[0x80013a4]['literal_final_capture_value']=='0xa0'
assert by[0x8001314]['operands']=='#0x8001414'
assert by[0x80013b0]['mnemonic']=='mla' and by[0x80013b4]['mnemonic']=='mla'
assert by[0x8001430]['operands']=='#0x8001304'
page=struct.unpack_from('<I',ram,0x1dc4-0x6f8)[0];assert page==0x6000000
report=dict(status='PASS_ORIGINAL_COLUMN_CALLER_STATIC_AND_FINAL_MEMORY_AUDIT',instructions=rows,final_capture_framebuffer=hex(page),findings=['Column endpoints are descriptor[0] and descriptor[0x10]+2, with positive untrimmed difference required.','Upper/lower rejection compares endpoints against live patched row limits, not ROM placeholder zero literals.','Perspective reciprocal and endpoint interpolation happen before clipping. Top clipping advances both wrapped texture coordinates using their per-row increments; bottom clipping limits end row.','The inner-loop row count is clipped end minus clipped start. Destination is page plus twice column index plus 240 times clipped start.','Outer loop advances destination column by two bytes and descriptor by 32 bytes.'],scope='Original ARM instruction encodings and final offline captured IWRAM literal values only. Final memory snapshot does not establish values for every earlier frame. No execution-path, full caller entry replay, cycle counts, source GPU fidelity or native integration claim.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [rp,ramfile,Path(__file__)]})
(root/'reports/original-column-caller-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS column caller clipping/stride encodings and final captured patched row/page literals')
