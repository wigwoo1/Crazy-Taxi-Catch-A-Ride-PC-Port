"""Complete originalARM2FE0 sky bitmap; data owners explicitly resolved."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
from capstone.arm import ARM_SFT_ASR,ARM_SFT_LSL,ARM_SFT_LSR
code={i.address:i for i in md.disasm(rom[0x3588:0x36c8],0x08003588)}
def original(case):
 global steps
 n,clip,width,height,nx,ny,nz,cache,maxdepth,far,shade=case;regs={f'r{i}':0 for i in range(16)};regs.update(sb=0x10010000,sl=16 if n==3 else 32,fp=0x03006810,ip=0,sp=0x10000400,lr=0,r0=0x10020000,r5=clip);regions={0x03006810:bytearray(248),0x10010000:bytearray([0,0,nx&255,ny&255,nz&255])+bytearray(19),0x0202b8ec:bytearray([cache]),0x10020000:bytearray(144),0x10000000:bytearray(2048)};words={0x08003af0:maxdepth&0xffffffff}
 def read(a,n=4):
  if a in words:return words[a]
  for start,data in regions.items():
   if start<=(a&~3 if n==4 else a) and (a&~3 if n==4 else a)+n<=start+len(data):
    aligned=a&~3 if n==4 else a;v=int.from_bytes(data[aligned-start:aligned-start+n],'little')
    shift=(a&3)*8 if n==4 else 0
    return ((v>>shift)|(v<<(32-shift)))&0xffffffff if shift else v
  if 0x08000000<=a and a+n<=0x08000000+len(rom):return int.from_bytes(rom[a-0x08000000:a-0x08000000+n],'little')
  raise AssertionError(('unmapped',hex(a),n))
 def write(a,v,n=4):
  if a in words:words[a]=v&0xffffffff;return
  for start,data in regions.items():
   if start<=a and a+n<=start+len(data):data[a-start:a-start+n]=(v&((1<<(n*8))-1)).to_bytes(n,'little');return
  raise AssertionError(('write',hex(a)))
 write(0x03006840,far);write(0x03006854,shade)
 N=Z=V=False;C=False;pc=0x08003588;case_steps=0
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
 while pc!=0x080036c8:
  if pc==0x080035b0:
   regs['r1']=width&0xffffffff;regs['r2']=height&0xffffffff;pc=0x0800364c # Explicit projection/minmax contract, not executed helper body.
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
   elif mn in ['ldr','ldrh','ldrb','ldrsb','str']:
    a=(regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp)&0xffffffff;name=md.reg_name(ops[0].reg)
    if mn=='str':write(a,regs[name])
    else:
     v=read(a,1 if mn in ['ldrb','ldrsb'] else 2 if mn=='ldrh' else 4)
     regs[name]=(v-256 if mn=='ldrsb' and v&128 else v)&0xffffffff
   elif mn=='cmp':
    a=value(ops[0]);bb=value(ops[1]);v=(a-bb)&0xffffffff;N=bool(v&0x80000000);Z=v==0;V=bool(((a^bb)&(a^v))&0x80000000);C=a>=bb
   elif mn in ['mov','movs','and','ands','eor','orr','orrs','add','sub','subs','rsb','mul','mla','asr','lsl']:
    if mn in ['mov','movs']:v=value(ops[1])
    elif mn in ['asr','lsl']:
     if len(ops)==2 and ops[1].type==ARM_OP_REG:v=value(ops[1])
     else:
      operand=ops[0] if len(ops)==2 else ops[1];shift=(value(ops[1]) if len(ops)==2 else value(ops[2]))&255;v=signed(value(operand))>>min(shift,31) if mn=='asr' else (value(operand)<<shift if shift<32 else 0)
    else:
     a=value(ops[1]);bb=value(ops[2]);v=a^bb if mn=='eor' else a|bb if mn in ['orr','orrs'] else a&bb if mn in ['and','ands'] else a+bb if mn=='add' else a-bb if mn in ['sub','subs'] else bb-a if mn=='rsb' else a*bb
     if mn=='mla':v+=value(ops[3])
    v&=0xffffffff;regs[md.reg_name(ops[0].reg)]=v
    if mn in ['movs','subs','orrs','ands']:N=bool(v&0x80000000);Z=v==0
   else:raise AssertionError((hex(pc),i.mnemonic,i.op_str))
  pc=nxt
 return regs['sl']
cases=[(n,clip,w,h,nx,ny,nz,cache,maxdepth,far,shade) for n in [3,4] for clip in range(64) for w,h in [(119,79),(120,79),(119,80),(-2147483648,2147483647)] for nx,ny,nz in [(0,0,0),(1,2,1),(2,2,1),(1,2,2),(-128,127,0),(0,-128,127),(127,0,-128),(-128,-128,-128)] for cache in [0,255] for maxdepth,far,shade in [(0,38400,0),(38399,38400,0),(38400,38400,0),(37887,38400,512),(37888,38400,512),(-2147483648,2147483647,-1),(2147483647,-2147483648,1)]]
rows=subprocess.run([str(root/'bin/world-packet-flags-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 got=original(case);assert got==int(row),(case,got,row)
report=dict(cases=len(cases),actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actual3588..36C4 flags decisions vs productionnative world_packet_flags, triangle/quad/all64clipunions/119-120and79-80size thresholds/signednormal extremaandties/cache256/wrappedshade512threshold. Projection35B0..3648 explicit suppliedwidth-height boundary, notwholeprojection/packet/31EC/frame equivalence.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/raster_dispatch.c','tests/world-packet-flags-rom-compare.c','tools/compare_world_packet_flags_rom.py']}
(root/'reports/world-packet-flags-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
