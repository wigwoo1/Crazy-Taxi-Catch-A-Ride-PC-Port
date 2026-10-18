"""Complete originalARM2FE0 sky bitmap; data owners explicitly resolved."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
from capstone.arm import ARM_SFT_ASR,ARM_SFT_LSL,ARM_SFT_LSR
code={i.address:i for i in md.disasm(rom[0x32fc:0x33e4],0x080032fc)}
def original(case):
 global steps
 city,ref,x,y,z,yaw,pitch=case;vertex=worlds[city][ref*16:ref*16+16];regs={f'r{i}':0 for i in range(16)};regs.update(sb=0,sl=0,fp=0x03006810,ip=0,sp=0x10000400,lr=0,r7=0x12000000,r6=0x10011000,r0=0x10020000,r5=0);regions={0x03006810:bytearray(248),0x10011000:bytearray(72),0x12000000:bytearray(vertex),0x10020000:bytearray(144),0x10000000:bytearray(2048)};words={0x08003af0:0}
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
 for off,v in [(0,x),(4,y),(8,z),(48,sine[(-pitch)&4095]),(60,cosine[(-pitch)&4095]),(52,sine[(-yaw)&4095]),(64,cosine[(-yaw)&4095])]:write(0x10011000+off,v)
 for off,v in [(44,256),(48,38400),(56,160),(60,106)]:write(0x03006810+off,v)
 N=Z=V=False;C=False;pc=0x080032fc;case_steps=0
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
 while pc!=0x080033e4:
  i=code[pc];steps+=1;case_steps+=1;assert case_steps<=128;
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
   elif mn in ['ldr','ldrh','ldrb','str']:
    a=(regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp)&0xffffffff;name=md.reg_name(ops[0].reg)
    if mn=='str':write(a,regs[name])
    else:regs[name]=read(a,1 if mn=='ldrb' else 2 if mn=='ldrh' else 4)
   elif mn=='cmp':
    a=value(ops[0]);bb=value(ops[1]);v=(a-bb)&0xffffffff;N=bool(v&0x80000000);Z=v==0;V=bool(((a^bb)&(a^v))&0x80000000);C=a>=bb
   elif mn in ['mov','movs','and','eor','orr','orrs','add','sub','subs','rsb','mul','mla','asr','lsl']:
    if mn in ['mov','movs']:v=value(ops[1])
    elif mn in ['asr','lsl']:
     if len(ops)==2 and ops[1].type==ARM_OP_REG:v=value(ops[1])
     else:
      operand=ops[0] if len(ops)==2 else ops[1];shift=ops[1].imm if len(ops)==2 else ops[2].imm;v=signed(value(operand))>>shift if mn=='asr' else value(operand)<<shift
    else:
     a=value(ops[1]);bb=value(ops[2]);v=a^bb if mn=='eor' else a|bb if mn in ['orr','orrs'] else a&bb if mn=='and' else a+bb if mn=='add' else a-bb if mn in ['sub','subs'] else bb-a if mn=='rsb' else a*bb
     if mn=='mla':v+=value(ops[3])
    v&=0xffffffff;regs[md.reg_name(ops[0].reg)]=v
    if mn in ['movs','subs','orrs']:N=bool(v&0x80000000);Z=v==0
   else:raise AssertionError((hex(pc),i.mnemonic,i.op_str))
  pc=nxt
 return [signed(read(0x10020000+j)) for j in [32,36,40]]+[read(0x10020010)]
import struct
trig=(root/'assets/driving.ctr').read_bytes();sine=[v>>4 for v in struct.unpack('<4096h',trig[4:8196])];cosine=[v>>4 for v in struct.unpack('<4096h',trig[8196:16388])]
worlds=[];cases=[];counts={}
for city,name in enumerate(['city1','city2']):
 data=(root/f'assets/{name}.ctw').read_bytes();nv=struct.unpack_from('<I',data,4)[0];worlds.append(data[20:20+nv*16]);counts[name]=nv
 data=(root/f'assets/{name}.ctp').read_bytes();pc,rc,dc=struct.unpack_from('<III',data,4);off=16+pc*32
 cameras=[(0,0,0,0,0)]
 for j in [0,dc-1]:
  x,y,z,pitch,yaw,roll=struct.unpack_from('<6i',data,off+j*48);cameras.append((x,y,z,yaw,pitch))
 cameras.append((-2147483648,2147483647,-2147483648,-1,142))
 for camera in cameras:cases.extend((city,j,*camera) for j in range(nv))
rows=subprocess.run([str(root/'bin/world-vertex-rom-compare.exe'),str(root/'assets/city1.ctw'),str(root/'assets/city2.ctw'),str(root/'assets/driving.ctr')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 got=original(case);assert got==list(map(int,row.split())),(case,got,row)
report=dict(cases=len(cases),city_vertices=counts,actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actual32FC..33E0 originalquad vertex body vs shared productionnativevertex helper, everyrealcityvertex atorigin/first+lastdestinationcamera/wrappedlimitfixture. Rawcachedinverse trig supplied from realCTR; XYZ andsixinclusiveclipflags exact. Packetallocation/culling/depthaggregate/whole31EC andfullframe notqualified.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_render.c','src/world_render.h','tests/world-vertex-rom-compare.c','tools/compare_world_vertex_rom.py']}
(root/'reports/world-vertex-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
