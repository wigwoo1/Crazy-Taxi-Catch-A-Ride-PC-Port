"""Original world depth bucket and linked insertion; explicit packet owners."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
from capstone.arm import ARM_SFT_ASR,ARM_SFT_LSL,ARM_SFT_LSR
code={i.address:i for i in md.disasm(rom[0x3838:0x38a0],0x08003838)}
def original(case):
 global steps
 depth,far,prior=case;regs={f'r{i}':0 for i in range(16)};regs.update(sb=0,sl=0,fp=0x03006810,ip=0,sp=0x10000400,lr=0,r4=0x02009eec+prior*144,r5=depth&0xffffffff);regions={0x03006810:bytearray(248),0x02009cec:bytearray(512),0x02009eec:bytearray([0xa5])*288,0x03006910:bytearray(4)};words={}
 trig=(root/'assets/driving.ctr').read_bytes();regions[0x0202c4f0]=bytearray(struct.pack('<4096i',*[v>>4 for v in struct.unpack('<4096h',trig[4:8196])]))
 def read(a,n=4):
  if a in words:return words[a]
  for start,data in regions.items():
   if start<=a and a+n<=start+len(data):return int.from_bytes(data[a-start:a-start+n],'little')
  if 0x08000000<=a and a+n<=0x08000000+len(rom):return int.from_bytes(rom[a-0x08000000:a-0x08000000+n],'little')
  raise AssertionError(('unmapped',hex(a),n))
 def write(a,v,n=4):
  if a in words:words[a]=v&0xffffffff;return
  for start,data in regions.items():
   if start<=a and a+n<=start+len(data):data[a-start:a-start+n]=(v&((1<<(n*8))-1)).to_bytes(n,'little');return
  raise AssertionError(('write',hex(a)))
 write(0x03006840,far);write(0x03006910,prior*144)
 # Prior heads cover every bucket; only the selected bucket may change.
 if prior:
  for j in range(128):write(0x02009cec+j*4,0x02009eec)
 before={k:bytes(v) for k,v in regions.items()}
 N=Z=V=False;C=False;pc=0x08003838;case_steps=0
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
 while pc!=0x080038a0:
  i=code[pc];steps+=1;case_steps+=1;assert case_steps<=256;
  mn=i.mnemonic;ops=i.operands;nxt=pc+4;regs['pc']=pc+8
  cond=mn[-2:];enabled=True
  if cond in ['eq','ne','lt','gt','ge','le']:
   enabled={'eq':Z,'ne':not Z,'lt':N!=V,'gt':not Z and N==V,'ge':N==V,'le':Z or N!=V}[cond];mn=mn[:-2]
  if enabled:
   if mn=='bl':
    assert ops[0].imm==0x08002ce8;aa=signed(regs['r0']);bb=signed(regs['r1']);regs['r0']=(abs(aa)//abs(bb)*(-1 if (aa<0)!=(bb<0) else 1))&0xffffffff
   elif mn=='bx':nxt=value(ops[0])
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
   elif mn in ['ldr','ldrh','ldrb','str','strh']:
    a=(regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp)&0xffffffff;name=md.reg_name(ops[0].reg)
    if mn in ['str','strh']:write(a,regs[name],2 if mn=='strh' else 4)
    else:regs[name]=read(a,1 if mn=='ldrb' else 2 if mn=='ldrh' else 4)
   elif mn=='cmp':
    a=value(ops[0]);bb=value(ops[1]);v=(a-bb)&0xffffffff;N=bool(v&0x80000000);Z=v==0;V=bool(((a^bb)&(a^v))&0x80000000);C=a>=bb
   elif mn in ['mov','movs','and','ands','eor','orr','orrs','add','sub','subs','rsb','mul','mla','asr','lsl','lsr']:
    if mn in ['mov','movs']:v=value(ops[1])
    elif mn in ['asr','lsl','lsr']:
     if len(ops)==2 and ops[1].type==ARM_OP_REG:v=value(ops[1])
     else:
      operand=ops[0] if len(ops)==2 else ops[1];shift=ops[1].imm if len(ops)==2 else ops[2].imm;v=signed(value(operand))>>shift if mn=='asr' else value(operand)>>shift if mn=='lsr' else value(operand)<<shift
    else:
     a=value(ops[1]);bb=value(ops[2]);v=a^bb if mn=='eor' else a|bb if mn in ['orr','orrs'] else a&bb if mn in ['and','ands'] else a+bb if mn=='add' else a-bb if mn in ['sub','subs'] else bb-a if mn=='rsb' else a*bb
     if mn=='mla':v+=value(ops[3])
    v&=0xffffffff;regs[md.reg_name(ops[0].reg)]=v
    if mn in ['movs','subs','orrs','ands']:N=bool(v&0x80000000);Z=v==0
   else:raise AssertionError((hex(pc),i.mnemonic,i.op_str))
  pc=nxt
 bucket=regs['r5'];packet=regs['r4'];assert 0<=bucket<128
 expected=bytearray(before[0x02009cec]);expected[bucket*4:bucket*4+4]=packet.to_bytes(4,'little');assert regions[0x02009cec]==expected
 expected=bytearray(before[0x02009eec]);off=prior*144;expected[off:off+4]=(0x02009eec if prior else 0).to_bytes(4,'little');expected[off+6:off+8]=(bucket>>6).to_bytes(2,'little');assert regions[0x02009eec]==expected
 assert read(0x03006910)==(prior+1)*144
 return [bucket,prior,prior+1,prior+1,23]
cases=[]
for far in [256,38400,65536,2147483647]:
 for depth in list(range(-2,65537,127))+[-2147483648,-1,0,1,far-1,far,far+1 if far<2147483647 else far,2097151,2097152,2147483647]:
  delta=max(depth,0);wrapped=(delta<<10)&0xffffffff;signed=wrapped if wrapped<0x80000000 else wrapped-0x100000000
  if signed<0:continue # Source table owner would precede sine base; native invalid guard is separate.
  for prior in [0,1]:cases.append([depth,far,prior])
rows=subprocess.run([str(root/'bin/world-link-rom-compare.exe'),str(root/'assets/driving.ctr')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for c,row in zip(cases,rows):assert original(c)==list(map(int,row.split())),(c,row)
report=dict(cases=len(cases),actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actual3838..389C depth bucket, source shade halfword, prepend links, ALL128head words, packet byte guards and144byte cursor advance. Production native depth bucket and queue mapped logical indices, source NULL to SIZE_MAX. Native payload/kind representation separate. Shade source checked, not native packet shade integration proof. Explicit realCTR-decoded sine RAM owner and signed2CE8division contract; excludes negative wrapped phase outside sine owner. Not whole31EC/fullframe/cache lifecycle proof.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/traffic_view.c','src/render_queue.c','tests/world-link-rom-compare.c','tools/compare_world_link_rom.py']}
(root/'reports/world-link-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
