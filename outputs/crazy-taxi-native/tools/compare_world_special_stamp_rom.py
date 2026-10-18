"""Actual38A8..38CC special-reference dispatch and persistent stamp gate."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
code={i.address:i for i in md.disasm(rom[0x38a8:0x38d0],0x080038a8)}
values=[0,1,2,0x7fffffff,0x80000000,0xfffffffe,0xffffffff]
cases=[(seed,generation,reference,prior) for seed in [0,165,0xffffffff] for generation in values for reference in [0x8000,0x8001,0x800f,0xc000,0xc001,0xc00f,0xffff] for prior in values]
rows=subprocess.run([str(root/'bin/world-special-stamp-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 seed,generation,ref,prior=case;cache=[(seed+j*0x1020304)&0xffffffff for j in range(16)];index=ref&0x3fff
 if index<16:cache[index]=prior
 regs={'sb':ref|0xffff0000,'r4':generation};pc=0x080038a8;eq=False
 def value(op):
  v=op.imm if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
  return (v<<op.shift.value)&0xffffffff if op.shift.type else v
 while pc not in [0x080038d0,0x08003aac]:
  i=code[pc];steps+=1;ops=i.operands;mn=i.mnemonic;nxt=pc+4
  if mn in ['tst','cmp']:
   a=value(ops[0]);b=value(ops[1]);eq=(a&b)==0 if mn=='tst' else a==b
  elif mn in ['and','add']:regs[md.reg_name(ops[0].reg)]=(value(ops[1])&value(ops[2]) if mn=='and' else value(ops[1])+value(ops[2]))&0xffffffff
  elif mn=='ldr':
   base=md.reg_name(ops[1].mem.base)
   if base=='pc':at=pc+8+ops[1].mem.disp;v=int.from_bytes(rom[at-0x08000000:at-0x08000000+4],'little')
   else:at=regs[base]+ops[1].mem.disp;assert 0x020276ec<=at<0x0202772c;v=cache[(at-0x020276ec)//4]
   regs[md.reg_name(ops[0].reg)]=v
  elif mn=='str':cache[(regs[md.reg_name(ops[1].mem.base)]-0x020276ec)//4]=value(ops[0])
  elif mn in ['beq','bne']:
   if eq==(mn=='beq'):nxt=ops[0].imm
  else:raise AssertionError((hex(pc),mn,i.op_str))
  pc=nxt
 assert [0 if pc==0x08003aac else 1]+cache==list(map(int,row.split())),case
report=dict(cases=len(cases),actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Complete38A8..38CC raw signedhalfword reference bit14filter/mask/literalpoolowner/generationcompare/store gate vsnative specialstamp, all16cachewords. Bit14skip preservesTSTflags acrossAND andneverloads cache. Validfixtureindices0/1/15; nativecapacityguards/whole31ECgeometry/previewintegration unqualified.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_polygon_stamp.c','src/world_polygon_stamp.h','tests/world-special-stamp-rom-compare.c','tools/compare_world_special_stamp_rom.py']}
(root/'reports/world-special-stamp-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
