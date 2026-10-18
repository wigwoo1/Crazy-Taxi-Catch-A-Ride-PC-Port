"""Original screen projection and bounds; explicit geometry inputs."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
from capstone.arm import ARM_SFT_ASR,ARM_SFT_LSL,ARM_SFT_LSR
code={i.address:i for i in md.disasm(rom[0x35b0:0x3648],0x080035b0)}
def original(case):
 global steps
 n,cx,cy,focal=case[:4];vertices=case[4:];regs={f'r{i}':0 for i in range(16)};regs.update(sb=0,sl=16 if n==3 else 32,fp=0x03006810,ip=0,sp=0x10000400,lr=0,r0=0x10020000);regions={0x03006810:bytearray(248),0x10020000:bytearray(144),0x10000000:bytearray(2048)};words={}
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
 for off,v in [(24,cx),(28,cy),(40,focal)]:write(0x03006810+off,v)
 for j,v in enumerate(vertices):write(0x10020000+32+(j//3)*32+(j%3)*4,v)
 N=Z=V=False;C=False;pc=0x080035b0;case_steps=0
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
 while pc!=0x08003648:
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
   elif mn in ['ldr','ldrh','ldrb','str']:
    a=(regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp)&0xffffffff;name=md.reg_name(ops[0].reg)
    if mn=='str':write(a,regs[name])
    else:regs[name]=read(a,1 if mn=='ldrb' else 2 if mn=='ldrh' else 4)
   elif mn=='cmp':
    a=value(ops[0]);bb=value(ops[1]);v=(a-bb)&0xffffffff;N=bool(v&0x80000000);Z=v==0;V=bool(((a^bb)&(a^v))&0x80000000);C=a>=bb
   elif mn in ['mov','movs','and','ands','eor','orr','orrs','add','sub','subs','rsb','mul','mla','asr','lsl']:
    if mn in ['mov','movs']:v=value(ops[1])
    elif mn in ['asr','lsl']:
     if len(ops)==2 and ops[1].type==ARM_OP_REG:v=value(ops[1])
     else:
      operand=ops[0] if len(ops)==2 else ops[1];shift=ops[1].imm if len(ops)==2 else ops[2].imm;v=signed(value(operand))>>shift if mn=='asr' else value(operand)<<shift
    else:
     a=value(ops[1]);bb=value(ops[2]);v=a^bb if mn=='eor' else a|bb if mn in ['orr','orrs'] else a&bb if mn in ['and','ands'] else a+bb if mn=='add' else a-bb if mn in ['sub','subs'] else bb-a if mn=='rsb' else a*bb
     if mn=='mla':v+=value(ops[3])
    v&=0xffffffff;regs[md.reg_name(ops[0].reg)]=v
    if mn in ['movs','subs','orrs','ands']:N=bool(v&0x80000000);Z=v==0
   else:raise AssertionError((hex(pc),i.mnemonic,i.op_str))
  pc=nxt
 return [signed(regs[name]) for name in ['sb','sl','fp','ip']]+[signed(read(0x10020000+16+i*32+j)) for i in range(n) for j in [0,12]]
# Source-qualified native vertex calculations are explicit geometry inputs.
exec(open('outputs/crazy-taxi-native/tools/compare_world_aggregate_rom.py').read().split('code=')[0],fixture_namespace:={})
vertex_data=fixture_namespace['vertex_data'];vertex_cases=fixture_namespace['vertex_cases'];cases=[];city_cases={}
for city,name in enumerate(['city1','city2']):
 data=(root/f'assets/{name}.ctw').read_bytes();nv,np,nm=struct.unpack_from('<III',data,4);polys=data[20+nv*16:20+nv*16+np*24];cameras=[c[2:] for c in vertex_cases if c[0]==city and c[1]==0];city_cases[name]=0
 for camera in cameras:
  for j in range(np):
   poly=polys[j*24:j*24+24];n=3 if int.from_bytes(poly[14:16],'little')&0x8000 else 4;points=[vertex_data[(city,int.from_bytes(poly[8+i*2:10+i*2],'little'),*camera)] for i in range(n)]
   if any(v[3]&48 for v in points):continue
   common=63
   for v in points:common&=v[3]
   if common:continue
   cases.append([n,120,80,192]+[a for v in points for a in v[:3]]);city_cases[name]+=1
fixture_count=0
for n in [3,4]:
 for cx,cy in [(120,80),(0,0),(-2147483648,2147483647)]:
  for focal in [1,192,2147483647]:
   for z in [1,256,38400,-1,-2147483648]:
    vertices=[a for i in range(n) for a in [(-2147483648 if i%2 else 2147483647),(-17 if i%2 else 31),z]]
    if z==-1 and any((a*focal)&0xffffffff==0x80000000 for i,a in enumerate(vertices) if i%3!=2):continue
    cases.append([n,cx,cy,focal]+vertices);fixture_count+=1
rows=subprocess.run([str(root/'bin/world-project-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 got=original(case);assert got==list(map(int,row.split())),(case,got,row)
report=dict(cases=len(cases),actual_city_geometry_inputs=city_cases,arithmetic_fixtures=fixture_count,actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Complete35B0..3644 triangle/quad screenprojection/minmax vs productionnativeproject_vertices. Source-qualifiednativevertex geometry input boundary, eligiblecity no-depthclip/common-plane cases plus wrappedcenter/product/negative-divisor fixtures. Signed2CE8division contract. NativeXYZ/UV/attribute retainedchecked. No sourcefullupstream/material/facing/clip-rejection/packet/frame proof.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_clip.c','src/world_clip.h','tests/world-project-rom-compare.c','tools/compare_world_project_rom.py']}
(root/'reports/world-project-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
