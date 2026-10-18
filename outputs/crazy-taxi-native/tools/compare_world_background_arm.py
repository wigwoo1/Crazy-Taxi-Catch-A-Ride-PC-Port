"""Complete originalARM2FE0 sky bitmap; data owners explicitly resolved."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
from capstone.arm import ARM_SFT_ASR,ARM_SFT_LSL,ARM_SFT_LSR
code={i.address:i for i in md.disasm(rom[0x2fe0:0x31ec],0x08002fe0)}
def original(case):
 global steps
 h,p,y,l,t,rr,b,seed=case;regs={f'r{i}':0 for i in range(16)};regs.update(sb=0,sl=0,fp=0,ip=0,sp=0x10000400,lr=0xffffffff);regions={0x03006810:bytearray(248),0x10010000:bytearray(64),0x10011000:bytearray(72),0x10012000:bytearray(12),0x10020000:bytearray((seed+j*37+j//240*11)&255 for j in range(h*240)),0x10080000:bytearray([165])*38400,0x10000000:bytearray(2048)};words={0x08001dc4:0x10080000}
 def read(a,n=4):
  if a in words:return words[a]
  for start,data in regions.items():
   if start<=a and a+n<=start+len(data):return int.from_bytes(data[a-start:a-start+n],'little')
  if 0x08000000<=a and a+n<=0x08000000+len(rom):return int.from_bytes(rom[a-0x08000000:a-0x08000000+n],'little')
  raise AssertionError(('unmapped',hex(a),n))
 def write(a,v,n=4):
  for start,data in regions.items():
   if start<=a and a+n<=start+len(data):data[a-start:a-start+n]=(v&((1<<(n*8))-1)).to_bytes(n,'little');return
  raise AssertionError(('write',hex(a)))
 for off,v in [(0,0x10010000),(4,0x10011000),(8,l),(12,t),(16,rr),(20,b),(64,0x04030201)]:write(0x03006810+off,v)
 write(0x10010000+32,0x10012000);write(0x10010000+20,0,2);write(0x10011000+16,y);write(0x10012000,240,1);write(0x10012001,h,1);write(0x10012004,0x10020000);regs['r0']=p&0xffffffff
 N=Z=V=False;C=False;pc=0x08002fe0
 def signed(v):return v if v<0x80000000 else v-0x100000000
 def value(op):
  v=op.imm&0xffffffff if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
  if op.shift.type:
   n=op.shift.value
   if op.shift.type==ARM_SFT_ASR:v=signed(v)>>n
   elif op.shift.type==ARM_SFT_LSL:v=v<<n
   elif op.shift.type==ARM_SFT_LSR:v=v>>n
   else:raise AssertionError(op.shift.type)
  return v&0xffffffff
 while pc!=0xffffffff:
  i=code[pc];steps+=1;assert steps<5000000;
  mn=i.mnemonic;ops=i.operands;nxt=pc+4;regs['pc']=pc+8
  cond=mn[-2:];enabled=True
  if cond in ['eq','ne','lt','gt','ge']:
   enabled={'eq':Z,'ne':not Z,'lt':N!=V,'gt':not Z and N==V,'ge':N==V}[cond];mn=mn[:-2]
  if enabled:
   if mn=='bx':nxt=value(ops[0])
   elif mn=='b':nxt=ops[0].imm
   elif mn in ['push','pop']:
    names=[md.reg_name(o.reg) for o in ops];a=regs['sp']-(4*len(names) if mn=='push' else 0)
    for j,name in enumerate(names):
     if mn=='push':write(a+j*4,regs[name])
     else:regs[name]=read(a+j*4)
    regs['sp']=a if mn=='push' else a+4*len(names)
   elif mn in ['ldm','stm']:
    base=md.reg_name(ops[0].reg);a=regs[base];names=[md.reg_name(o.reg) for o in ops[1:]]
    for j,name in enumerate(names):
     if mn=='ldm':regs[name]=read(a+j*4)
     else:write(a+j*4,regs[name])
    if i.writeback:regs[base]=(a+4*len(names))&0xffffffff
   elif mn in ['ldr','ldrh','ldrb','str']:
    a=(regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp)&0xffffffff;name=md.reg_name(ops[0].reg)
    if mn=='str':write(a,regs[name])
    else:regs[name]=read(a,1 if mn=='ldrb' else 2 if mn=='ldrh' else 4)
   elif mn=='cmp':
    a=value(ops[0]);bb=value(ops[1]);v=(a-bb)&0xffffffff;N=bool(v&0x80000000);Z=v==0;V=bool(((a^bb)&(a^v))&0x80000000);C=a>=bb
   elif mn in ['mov','movs','and','add','sub','subs','rsb','mul','mla','asr','lsl']:
    if mn in ['mov','movs']:v=value(ops[1])
    elif mn in ['asr','lsl']:
     if len(ops)==2 and ops[1].type==ARM_OP_REG:v=value(ops[1])
     else:
      operand=ops[0] if len(ops)==2 else ops[1];shift=ops[1].imm if len(ops)==2 else ops[2].imm;v=signed(value(operand))>>shift if mn=='asr' else value(operand)<<shift
    else:
     a=value(ops[1]);bb=value(ops[2]);v=a&bb if mn=='and' else a+bb if mn=='add' else a-bb if mn in ['sub','subs'] else bb-a if mn=='rsb' else a*bb
     if mn=='mla':v+=value(ops[3])
    v&=0xffffffff;regs[md.reg_name(ops[0].reg)]=v
    if mn in ['movs','subs']:N=bool(v&0x80000000);Z=v==0
   else:raise AssertionError((hex(pc),i.mnemonic,i.op_str))
  pc=nxt
 return regions[0x10080000]
cases=[(160,p,y,0,0,240,160,seed) for p in [0,142,-142,2048,-1] for y in [0,1,1024,-1] for seed in [0,165]]+[(128,0,4095,4,8,236,120,255),(96,142,300,0,0,240,160,1)]
rows=subprocess.run([str(root/'bin/world-background-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 a=list(original(case));b=list(map(int,row.split()));diff=[(j,a[j],b[j]) for j in range(38400) if a[j]!=b[j]]
 assert not diff,dict(case=case,diff_count=len(diff),first=diff[:12])
report=dict(cases=len(cases),actual_arm_instructions=steps,pass_match=True,scope='Complete original2FE0 sky renderer vsnativeworld_background, all38400bytes incluntouchedoutsideclip. Explicitdescriptor/frame/view data owners, noopaquehelper. Supported cases require positive copiedrows; originalnegative/zero rowcount pathological paths and whole31860 excluded. Generated textures, not new real-cityassetproof.',rom_sha256=hashlib.sha256(rom).hexdigest())
report['file_sha256']={name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['src/world_background.c','src/indexed_asset.c','tests/world-background-rom-compare.c','tools/compare_world_background_arm.py','tools/rebuild_world_background_compare.ps1']}
(root/'reports/world-background-arm-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
