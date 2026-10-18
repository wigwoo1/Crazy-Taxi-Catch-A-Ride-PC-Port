"""Source generation+stamp prefixes sequenced with restored counter boundary."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
codes={i.address:i for lo,hi in [(0x2d20,0x2d3c),(0x3228,0x3234)] for i in md.disasm(rom[lo:hi],0x08000000+lo)}
def execute(start,stop,regs,mem):
 global steps
 pc=start;eq=False
 while pc!=stop:
  i=codes[pc];steps+=1;parts=i.op_str.split(', ')
  if i.mnemonic=='ldr':regs[parts[0]]=mem[regs[i.reg_name(i.operands[1].mem.base)]+i.operands[1].mem.disp]
  elif i.mnemonic=='orrs':regs[parts[0]]=regs[parts[1]]|regs[parts[2]];eq=regs[parts[0]]==0
  elif i.mnemonic=='cmp':eq=regs[parts[0]]==regs[parts[1]]
  elif i.mnemonic=='beq':
   if eq:return i.operands[0].imm
  elif i.mnemonic=='add':regs[parts[0]]=(regs[parts[1]]+i.operands[2].imm)&0xffffffff
  elif i.mnemonic=='str':mem[regs[i.reg_name(i.operands[1].mem.base)]+i.operands[1].mem.disp]=regs[parts[0]]
  else:raise AssertionError((i.mnemonic,i.op_str))
  pc+=4
 return pc
cases=[(base,prior) for base in [0,1,0x7fffffff,0x80000000,0xfffffffe,0xffffffff] for prior in [0,1,2,0x7fffffff,0x80000000,0xffffffff]]
rows=subprocess.run([str(root/'bin/graphics-preview-stamp-sequence.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for (base,prior),row in zip(cases,rows):
 graph=0x03006810;cache=0x0201b6ec;mem={graph:1,graph+4:1,graph+0x34:base,cache:prior,cache+4:0xa5a5a5a5,0x0202b6ec:0x12345678}
 def frame():
  assert execute(0x08002d20,0x08002d3c,{'fp':graph},mem)==0x08002d3c
  result=execute(0x08003228,0x08003234,{'r6':mem[cache],'r4':mem[graph+0x34],'r5':cache},mem)
  return 0 if result==0x08003aac else 1
 preview=frame();pg=mem[graph+0x34];mem[graph+0x34]=base # Explicit248snapshot counter-copy boundary.
 next_result=frame();following=frame();want=[pg,preview,next_result,mem[graph+0x34],following,mem[cache],mem[cache+4],mem[0x0202b6ec]]
 assert want==list(map(int,row.split())),dict(case=(base,prior),source=want,native=row)
report=dict(cases=len(cases),actual_arm_instructions=steps,pass_match=True,scope='Native actualGraphicsStorage owners generation+normalstamp vs original2D20..2D38/3228..3230 across preview/counterrestore/next/following sequence inclwrap. Counterrestore explicit sourcecopy boundary, not executedwhole248copy. Retained externalcache/head sentinel checked. Full31ECpacket production/renderhead consumption, whole31860 and liveintegration unqualified.',rom_sha256=hashlib.sha256(rom).hexdigest())
(root/'reports/graphics-preview-stamp-sequence-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
