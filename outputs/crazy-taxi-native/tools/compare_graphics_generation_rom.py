"""Actual originalARM2D20..2D38 generation prefix."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
code={i.address:i for i in md.disasm(rom[0x2d20:0x2d3c],0x08002d20)}
values=[0,1,2,0x7fffffff,0x80000000,0xfffffffe,0xffffffff];cases=[(n,s,t) for n in values for s in [0,1,0x80000000,0xffffffff] for t in [0,1,0x80000000,0xffffffff]]
rows=subprocess.run([str(root/'bin/graphics-generation-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();steps=0
for case,row in zip(cases,rows):
 n,s,t=case;regs={'fp':0x10010000};mem={0:s,4:t,0x34:n};pc=0x08002d20;eq=False
 while pc!=0x08002d3c and pc!=0x08002fd8:
  i=code[pc];steps+=1;parts=i.op_str.split(', ')
  if i.mnemonic=='ldr':regs[parts[0]]=mem[i.operands[1].mem.disp]
  elif i.mnemonic=='orrs':regs[parts[0]]=regs[parts[1]]|regs[parts[2]];eq=regs[parts[0]]==0
  elif i.mnemonic=='beq':
   if eq:pc=i.operands[0].imm;continue
  elif i.mnemonic=='add':regs[parts[0]]=(regs[parts[1]]+i.operands[2].imm)&0xffffffff
  elif i.mnemonic=='str':mem[i.operands[1].mem.disp]=regs[parts[0]]
  else:raise AssertionError((i.mnemonic,i.op_str))
  pc+=4
 assert [0 if pc==0x08002fd8 else 1,mem[0x34]]==list(map(int,row.split())) and mem[0]==s and mem[4]==t,case
report=dict(cases=len(cases),actual_arm_instructions=steps,pass_match=True,scope='Actual2D20..2D38 prefix original ORscene/transform guard and wrap32bit generation increment; untouched identities retained. No full2D18/31EC or scene/transform null fatal-path claim. Native owner pointer missing guarded separately.',rom_sha256=hashlib.sha256(rom).hexdigest())
(root/'reports/graphics-generation-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
