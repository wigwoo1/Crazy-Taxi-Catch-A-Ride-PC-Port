"""Diagnostic original full normal producer; no runtime emulator in native game."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
from capstone.arm import ARM_SFT_ASR,ARM_SFT_LSL,ARM_SFT_LSR
cfg=json.loads((root/'reports/world-producer-cfg.json').read_text());code={int(row[:8],16):next(md.disasm(rom[int(row[:8],16)-0x08000000:int(row[:8],16)-0x08000000+4],int(row[:8],16))) for row in cfg['instructions']}
def original(case,generation=1,normal_cache=None):
 global steps
 city,refs,x,y,z,yaw,pitch=case;ref=refs[0];scene=[0x087556cc,0x08755734][city];regs={f'r{i}':0 for i in range(16)};regs.update(sb=0,sl=0,fp=0,ip=0,sp=0x10001000,lr=0x15000000,r0=0x11000000);regions={0x03006810:bytearray(248),0x12000000:bytearray(72),0x11000000:bytearray(28+len(refs)*2),0x10000000:bytearray(8192),0x0201b6ec:bytearray(49152),0x020276ec:bytearray(16384),0x0202b8ec:bytearray(512),0x02009cec:bytearray(512),0x02009eec:bytearray([0xa5])*(144*len(refs)),0x03006910:bytearray(4)};words={0x08003af0:0}
 if normal_cache is not None:regions[0x0201b6ec][:]=normal_cache
 trig=(root/'assets/driving.ctr').read_bytes();sine=[v>>4 for v in struct.unpack('<4096h',trig[4:8196])];cosine=[v>>4 for v in struct.unpack('<4096h',trig[8196:16388])];regions[0x0202c4f0]=bytearray(struct.pack('<4096i',*sine))
 def read(a,n=4):
  if a in words:return words[a]
  for start,data in regions.items():
   aligned=a&~3 if n==4 else a
   if start<=aligned and aligned+n<=start+len(data):
    v=int.from_bytes(data[aligned-start:aligned-start+n],'little');shift=(a&3)*8 if n==4 else 0
    return ((v>>shift)|(v<<(32-shift)))&0xffffffff if shift else v
  if 0x08000000<=a and a+n<=0x08000000+len(rom):return int.from_bytes(rom[a-0x08000000:a-0x08000000+n],'little')
  raise AssertionError(('unmapped',hex(a),n))
 def write(a,v,n=4):
  if a in words:words[a]=v&0xffffffff;return
  for start,data in regions.items():
   if start<=a and a+n<=start+len(data):data[a-start:a-start+n]=(v&((1<<(n*8))-1)).to_bytes(n,'little');return
  raise AssertionError(('write',hex(a)))
 for off,v in [(0,scene),(4,0x12000000),(24,120),(28,80),(40,192),(44,256),(48,38400),(52,generation),(56,160),(60,106)]:write(0x03006810+off,v)
 for off,v in [(0,x),(4,y),(8,z),(48,sine[(-pitch)&4095]),(60,cosine[(-pitch)&4095]),(52,sine[(-yaw)&4095]),(64,cosine[(-yaw)&4095])]:write(0x12000000+off,v)
 write(0x11000018,len(refs))
 for i,reference in enumerate(refs):write(0x1100001c+i*2,reference,2)
 N=Z=V=False;C=False;pc=0x080031ec;case_steps=0
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
 while pc!=0x15000000:
  i=code[pc];steps+=1;case_steps+=1;assert case_steps<=2048*len(refs);
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
   elif mn in ['ldm','stm','stmdb']:
    base=md.reg_name(ops[0].reg);a=regs[base];names=[md.reg_name(o.reg) for o in ops[1:]]
    if mn=='stmdb':a-=4*len(names)
    for j,name in enumerate(names):
     if mn=='ldm':regs[name]=read(a+j*4)
     else:write(a+j*4,regs[name])
    if i.writeback:regs[base]=(a if mn=='stmdb' else a+4*len(names))&0xffffffff
   elif mn in ['ldr','ldrh','ldrb','ldrsh','ldrsb','str','strh']:
    m=ops[1].mem;index=regs[md.reg_name(m.index)] if m.index else 0;index=index<<ops[1].shift.value if ops[1].shift.type else index;a=(regs[md.reg_name(m.base)]+m.disp+index)&0xffffffff;name=md.reg_name(ops[0].reg)
    if mn in ['str','strh']:write(a,regs[name],2 if mn=='strh' else 4)
    else:
     size=1 if mn in ['ldrb','ldrsb'] else 2 if mn in ['ldrh','ldrsh'] else 4;v=read(a,size);regs[name]=(v-(1<<(size*8)) if mn in ['ldrsb','ldrsh'] and v&(1<<(size*8-1)) else v)&0xffffffff
   elif mn=='tst':
    v=value(ops[0])&value(ops[1]);N=bool(v&0x80000000);Z=v==0
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
 assert regs['sp']==0x10001000
 return {'cursor':read(0x03006910),'stamp':read(0x0201b6ec+ref*4),'packet':bytes(regions[0x02009eec]).hex(),'heads':bytes(regions[0x02009cec]).hex(),'cache':bytes(regions[0x0201b6ec])}
steps=0;cases=[];results=[]
data=json.loads((root/'reports/world-producer-multi-reference-observation.json').read_text())
for fixture in data['cases']:
 for base in [0,1,0xfffffffe,0xffffffff]:
  cache=bytearray(49152);counts=[];checksums=[]
  for gen in [(base+1)&0xffffffff,(base+1)&0xffffffff,(base+2)&0xffffffff]:
   result=original(fixture,gen,cache);cache=result['cache'];counts.append(result['cursor']//144);checksums.append(sum((i+1)*int.from_bytes(cache[i*4:i*4+4],'little') for i in range(12288)))
  cases.append([fixture,base]);results.append([counts,checksums])
report={'cases':cases,'results':results,'actual_arm_instructions':steps,'scope':'Complete31EC three sequenced calls with persistent externalnormalcache; generations preview(base+1), restored-counter next(base+1), following(base+2), wrap included. Generation advance/counterrestore and consumedheads/cursorreset explicit previously-qualified boundaries, notwholepreviewcaller/consumer execution.'}
(root/'reports/world-producer-cache-sequence-source.json').write_text(json.dumps(report,indent=2)+'\n');print('Source cache sequence cases',len(cases),'instructions',steps)
