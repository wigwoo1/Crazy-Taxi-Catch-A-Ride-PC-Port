"""Original Retry interior transfer and real saved-frame return audit.
Does not execute full gameplay; proves entry/exit stack shape and epilogue
return destination with explicit caller-frame fixture.
"""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
instructions={};todo=[0x800ac70]
while todo:
 a=todo.pop()
 if a in instructions:continue
 assert 0x800ac70<=a<=0x800b6de,hex(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));instructions[a]=i
 if i.mnemonic=='bx' or ('pc' in i.op_str and i.mnemonic=='pop'):continue
 if i.mnemonic=='bl':
  target=int(i.op_str.lstrip('#'),16)
  if a==0x800b5bc:assert target==0x800ac70;todo.append(target);continue
  todo.append(a+i.size);continue # external helper calls assumed balanced
 if i.mnemonic.startswith('b'):
  target=int(i.op_str.lstrip('#'),16);todo.append(target)
  if i.mnemonic!='b':todo.append(a+i.size)
 else:todo.append(a+i.size)
frame_ops=[{'address':hex(a),'mnemonic':i.mnemonic,'operands':i.op_str} for a,i in sorted(instructions.items()) if i.mnemonic in ['push','pop'] or (i.mnemonic in ['add','sub'] and i.op_str.startswith('sp,'))]
assert not any(i.mnemonic=='push' or (i.mnemonic=='sub' and i.op_str.startswith('sp,')) for i in instructions.values()),frame_ops
prologue=list(md.disasm(r[0xabec:0xabf8],0x800abec));assert prologue[0].mnemonic=='push' and 'lr' in prologue[0].op_str
transfer=next(md.disasm(r[0xb5bc:0xb5c0],0x800b5bc));assert transfer.mnemonic=='bl' and transfer.op_str=='#0x800ac70'
for a in range(0x800b6d0,0x800b6e0,2):allowed_thumb[a]=r[a-0x8000000:a-0x8000000+2]
p=ExitProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];stack=0x10000400;p.reg[13]=stack;p.reg[14]=0x800b5c1
for j in range(3):p.write(stack+0x28+j*4,0x80+j)
for j in range(4):p.write(stack+0x34+j*4,0x40+j)
p.write(stack+0x44,0x10020001)
p.run(entry=0x800b6d0,stop=0x10020000)
assert p.reg[13]==stack+72
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'entry_prologue':'0x0800abec','retry_transfer':'0x0800b5bc BL0x0800ac70','target_is_inside_existing_frame':True,'long_branch_displacement':0x800ac70-(0x800b5bc+4),'reachable_local_instructions':len(instructions),'local_frame_operations':frame_ops,'epilogue_actual_thumb_instructions':p.steps,'fixture_incoming_lr':'0x0800b5c1','fixture_saved_caller_lr':'0x10020001','actual_return':'0x10020000','actual_sp_restored_bytes':72,'scope':'Reachable local AC70..B6DE CFG has no frame allocation; called helpers opaque balanced-call contracts. Actual B6D0 epilogue pops original ABEC saved caller return, ignoring LR from Retry BL. Therefore treating AC70 as a newly framed returning child is unsupported; full initialization/gameplay not executed.','source_sha256':hashlib.sha256((root/'tools/audit_retry_transfer_rom.py').read_bytes()).hexdigest()}
(root/'reports/retry-transfer-rom-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
