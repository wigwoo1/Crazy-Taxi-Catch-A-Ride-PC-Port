"""Actual ARM3228/322C/3230 first-visit gate; no game/runtime emulation."""
from pathlib import Path
import sys,struct,subprocess,json,hashlib
sys.path.insert(0,'work/python-packages')
from capstone import *
from capstone.arm import ARM_OP_IMM,ARM_OP_REG,ARM_OP_MEM
root=Path('outputs/crazy-taxi-native');rom=Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();md=Cs(CS_ARCH_ARM,CS_MODE_ARM);md.detail=True
code={i.address:i for i in md.disasm(rom[0x3228:0x3234],0x08003228)}
assert [(i.mnemonic,i.op_str) for i in code.values()]==[('cmp','r6, r4'),('beq','#0x8003aac'),('str','r4, [r5]')]
values=[0,1,2,0x7fffffff,0x80000000,0xfffffffe,0xffffffff]
cases=[(seed,generation,reference,prior) for seed in [0,165,0xffffffff] for generation in values for reference in [0,1,15] for prior in values];rows=subprocess.run([str(root/'bin/world-polygon-stamp-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 seed,generation,ref,prior=case;cache=[(seed+j*0x1020304)&0xffffffff for j in range(16)];cache[ref]=prior;regs={'r6':prior,'r4':generation,'r5':ref*4};pc=0x08003228;eq=False
 while pc!=0x08003234 and pc!=0x08003aac:
  i=code[pc];steps+=1
  if i.mnemonic=='cmp':eq=regs[i.reg_name(i.operands[0].reg)]==regs[i.reg_name(i.operands[1].reg)]
  elif i.mnemonic=='beq':
   if eq:pc=i.operands[0].imm;continue
  elif i.mnemonic=='str':cache[regs[i.reg_name(i.operands[1].mem.base)]//4]=regs[i.reg_name(i.operands[0].reg)]
  else:raise AssertionError(i.mnemonic)
  pc+=4
 assert [0 if pc==0x08003aac else 1]+cache==list(map(int,row.split())),case
report=dict(cases=len(cases),actual_arm_instructions=steps,pass_match=True,scope='Actual normal-reference stamp gate3228..3230; all16cachewords and firstvisit result including generationzero/highbit/equality. Cache indexing fixtures explicit, not complete31EC/2D18/31860 or live integration. Source stamp write precedes material rejection; native guards invalid/special references unclaimed sourcepaths.',rom_sha256=hashlib.sha256(rom).hexdigest())
(root/'reports/world-polygon-stamp-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
