import pathlib,sys,json,hashlib
sys.path.insert(0,'work/python-packages')
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
from capstone.arm import ARM_OP_MEM,ARM_REG_PC
root=pathlib.Path('outputs/crazy-taxi-native');r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
entry=0x8016208;todo=[entry];seen=set();instructions=[];literals={}
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x8016242;seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));instructions.append(dict(address=hex(a),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str))
 for op in i.operands:
  if op.type==ARM_OP_MEM and op.mem.base==ARM_REG_PC:
   p=((a+4)&~3)+op.mem.disp;literals[hex(p)]=hex(int.from_bytes(r[p-0x8000000:p-0x8000000+4],'little'))
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
report={'status':'RECOVERED_STATIC_CONTROL_FLOW','rom_sha256':hashlib.sha256(r).hexdigest(),'instructions':sorted(instructions,key=lambda v:int(v['address'],16)),'literals':literals,'finding':'16208 truncates requested wait count to8bits. When interrupt-master register equals1, each requested wait clears logical interrupt flag bit0 and polls until set, then proceeds. Presentation26318 requests one wait before input/palette/object/copy work; elapsed counter sampled after that work. This does not prescribe fixed4 gameplay ticks.','scope':'Original instruction and literal recovery only. Busy wait must be replaced by native event/time owner, never a hardware runtime loop. Actual helper runtime cost/IRQ dispatch and full scheduler remain unverified.'}
(root/'reports/original-frame-wait-control.json').write_text(json.dumps(report,indent=2));print('Recovered',len(seen),'instructions',literals)
