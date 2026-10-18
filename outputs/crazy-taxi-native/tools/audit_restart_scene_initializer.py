"""Reachable source2AC6C initializer dependencies, hash-bound ROM only."""
import sys,json,hashlib,struct
from pathlib import Path
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
from capstone.arm import ARM_OP_IMM
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(rom).hexdigest()=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
todo=[0x0802ac6c];seen=set();rows=[];calls=[];dynamic=[]
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x0802ac6c<=a<0x0802b44c,hex(a)
 seen.add(a);i=next(md.disasm(rom[a-0x08000000:a-0x08000000+4],a,count=1));row=dict(address=hex(a),mnemonic=i.mnemonic,operands=i.op_str,bytes=bytes(i.bytes).hex());h=struct.unpack_from('<H',rom,a-0x08000000)[0]
 if h&0xf800==0x4800:
  slot=((a+4)&~3)+(h&255)*4;row['literal']=hex(struct.unpack_from('<I',rom,slot-0x08000000)[0])
 rows.append(row)
 if i.mnemonic=='bl':calls.append(dict(address=hex(a),target=hex(i.operands[0].imm)));todo.append(a+i.size);continue
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='mov' and i.op_str.startswith('pc,'):dynamic.append(row);continue
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bics',):
  assert i.operands[0].type==ARM_OP_IMM;todo.append(i.operands[0].imm)
  if i.mnemonic!='b':todo.append(a+i.size)
 else:todo.append(a+i.size)
rows.sort(key=lambda x:int(x['address'],16));calls.sort(key=lambda x:int(x['address'],16))
report=dict(rom_sha256=hashlib.sha256(rom).hexdigest(),entry='0x0802ac6c',reachable_instructions=len(rows),calls=calls,dynamic_exits=dynamic,scope='Reachable static Thumb dependency inventory; helper bodies/engine allocation/relocation and runtime execution not proven.',instructions=rows)
(root/'reports/restart-scene-initializer-audit.json').write_text(json.dumps(report,indent=2)+'\n')
(root/'reports/restart-scene-initializer-source.txt').write_text('\n'.join(f"{x['address']}: {x['mnemonic']} {x['operands']}"+(f" literal={x['literal']}" if 'literal' in x else '') for x in rows)+'\n')
print(json.dumps({k:v for k,v in report.items() if k!='instructions'},indent=2))
