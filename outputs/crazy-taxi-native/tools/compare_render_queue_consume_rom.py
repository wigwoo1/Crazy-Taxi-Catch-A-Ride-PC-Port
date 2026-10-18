"""Actual source2B6A0 bucket/link loop with packet processing contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
code={i.address:i for lo,hi in [(0x2b6f8,0x2b716),(0x2b964,0x2b97e)] for i in md.disasm(rom[lo:hi],0x08000000+lo)}
head_address=int.from_bytes(rom[0x2b774:0x2b778],'little');assert head_address==0x02009cec,hex(head_address)
cases=[(seed,n) for seed in [0,1,127,165,0xffffffff] for n in [0,1,2,16,129,256]]
rows=subprocess.run([str(root/'bin/render-queue-consume-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for (seed,n),row in zip(cases,rows):
 mem={head_address+j*4:0 for j in range(128)};mem[0x03006910]=(seed+n*144)&0xffffffff;ids={}
 for j in range(n):
  bucket=((seed+j*17)&0xffffffff)%128;node=0x10010000+j*32;mem[node]=mem[head_address+bucket*4];mem[head_address+bucket*4]=node;ids[node]=j+1
 regs={'sp':0x10000400};pc=0x0802b6f8;eq=False;le=False;calls=[]
 def reg(op):return regs[md.reg_name(op.reg)]
 while pc!=0x0802b97e:
  if pc==0x0802b716:pc=0x0802b720
  if pc==0x0802b720:calls.append(ids[regs['r5']]);pc=0x0802b964 # Explicit opaque packet processing boundary.
  i=code[pc];steps+=1;ops=i.operands;mn=i.mnemonic;nxt=pc+i.size
  if mn in ['mov','movs']:regs[md.reg_name(ops[0].reg)]=ops[1].imm&0xffffffff if ops[1].type==ARM_OP_IMM else reg(ops[1])
  elif mn in ['lsls','adds','subs']:
   left=ops[0] if len(ops)==2 else ops[1];right=ops[1] if len(ops)==2 else ops[2];a=reg(left);b=right.imm if right.type==ARM_OP_IMM else reg(right);v=(a<<b) if mn=='lsls' else a+b if mn=='adds' else a-b;regs[md.reg_name(ops[0].reg)]=v&0xffffffff
  elif mn=='ldr':
   op=ops[1].mem
   if md.reg_name(op.base)=='pc':addr=((pc+4)&~3)+op.disp;v=int.from_bytes(rom[addr-0x08000000:addr-0x08000000+4],'little')
   else:v=mem[regs[md.reg_name(op.base)]+op.disp]
   regs[md.reg_name(ops[0].reg)]=v
  elif mn=='str':op=ops[1].mem;mem[regs[md.reg_name(op.base)]+op.disp]=reg(ops[0])
  elif mn=='cmp':
   a=reg(ops[0]);b=ops[1].imm if ops[1].type==ARM_OP_IMM else reg(ops[1]);eq=a==b;sa=a if a<0x80000000 else a-0x100000000;sb=b if b<0x80000000 else b-0x100000000;le=sa<=sb
  elif mn in ['b','beq','bne','ble']:
   if mn=='b' or (mn=='beq' and eq) or (mn=='bne' and not eq) or (mn=='ble' and le):nxt=ops[0].imm
  else:raise AssertionError((hex(pc),mn,i.op_str))
  pc=nxt
 assert mem[0x03006910]==0, 'Original packet byte cursor must reset after consumption'
 want=[len(calls)]+calls+[mem[head_address+j*4] for j in range(128)]+[0]
 assert want==list(map(int,row.split())),dict(case=(seed,n),source=want,native=row)
report=dict(cases=len(cases),actual_thumb_instructions=steps,pass_match=True,scope='Original2B6A0 final-render02009CEC bucket loop127down0/headclear/link-follow and COMPLETE2B978..2B97C original03006910 packet-byte-cursor reset. Native queueconsume order/all128heads/countzero compared; native count is not source bytecursor layout. Packet-processing2B720..2B962 explicit contract; node links fixture. No intermediate prepared queue claim. Full producer/packet/raster callbacks and preview lifecycle remain unqualified.',rom_sha256=hashlib.sha256(rom).hexdigest())
(root/'reports/render-queue-consume-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
