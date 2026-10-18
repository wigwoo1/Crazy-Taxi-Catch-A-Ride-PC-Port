"""Complete originalARM2FE0 sky bitmap; data owners explicitly resolved."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
from capstone.arm import ARM_SFT_ASR,ARM_SFT_LSL,ARM_SFT_LSR
code={i.address:i for i in md.disasm(rom[0x2d18:0x2e70],0x08002d18)}
def original(case):
 global steps
 x,z,y,p=case[:4];nx,nz,sx,sz,ox,oz=case[4:10];corners=case[10:];regs={f'r{i}':0 for i in range(16)};regs.update(sb=0,sl=0,fp=0,ip=0,sp=0x10000400,lr=0xffffffff);regions={0x03006810:bytearray(248),0x10010000:bytearray(100),0x10011000:bytearray(72),0x10000000:bytearray(2048)};words={}
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
 for off,v in [(0,0x10010000),(4,0x10011000)]:write(0x03006810+off,v)
 for j,v in enumerate(corners):write(0x03006860+(j//3)*16+(j%3)*4,v)
 for off,v in [(0,x),(8,z),(24,sine[p&4095]),(36,cosine[p&4095]),(28,sine[y&4095]),(40,cosine[y&4095])]:write(0x10011000+off,v)
 for off,v in [(60,nx),(62,nz)]:write(0x10010000+off,v,2)
 for off,v in [(64,sx),(68,sz),(72,ox),(80,oz)]:write(0x10010000+off,v)
 N=Z=V=False;C=False;pc=0x08002d18
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
 while pc!=0x08002e70:
  i=code[pc];steps+=1;assert steps<5000000;
  mn=i.mnemonic;ops=i.operands;nxt=pc+4;regs['pc']=pc+8
  cond=mn[-2:];enabled=True
  if cond in ['eq','ne','lt','gt','ge']:
   enabled={'eq':Z,'ne':not Z,'lt':N!=V,'gt':not Z and N==V,'ge':N==V}[cond];mn=mn[:-2]
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
   elif mn in ['mov','movs','and','orrs','add','sub','subs','rsb','mul','mla','asr','lsl']:
    if mn in ['mov','movs']:v=value(ops[1])
    elif mn in ['asr','lsl']:
     if len(ops)==2 and ops[1].type==ARM_OP_REG:v=value(ops[1])
     else:
      operand=ops[0] if len(ops)==2 else ops[1];shift=ops[1].imm if len(ops)==2 else ops[2].imm;v=signed(value(operand))>>shift if mn=='asr' else value(operand)<<shift
    else:
     a=value(ops[1]);bb=value(ops[2]);v=a|bb if mn=='orrs' else a&bb if mn=='and' else a+bb if mn=='add' else a-bb if mn in ['sub','subs'] else bb-a if mn=='rsb' else a*bb
     if mn=='mla':v+=value(ops[3])
    v&=0xffffffff;regs[md.reg_name(ops[0].reg)]=v
    if mn in ['movs','subs','orrs']:N=bool(v&0x80000000);Z=v==0
   else:raise AssertionError((hex(pc),i.mnemonic,i.op_str))
  pc=nxt
 return [signed(regs[n]) for n in ['r4','r6','r5','r7']]
import struct
trig=(root/'assets/driving.ctr').read_bytes();sine=[v>>4 for v in struct.unpack('<4096h',trig[4:8196])];cosine=[v>>4 for v in struct.unpack('<4096h',trig[8196:16388])]
cases=[]
for x,z in [(0,0),(131072,-131072),(-2147483648,2147483647)]:
 for yaw,pitch in [(0,0),(1024,0),(-1,142),(300,700)]:
  for focal in [192,128]:
   corners=[]
   for i in range(8):
    depth=256 if i<4 else 32768;xx=120 if i%4 in [1,2] else -120;yy=80 if i%4>=2 else -80
    corners.extend([int(xx*depth/focal),int(yy*depth/focal),depth])
   cases.append([x,z,yaw,pitch,64,64,4096,4096,-131072,-131072]+corners)
city_counts={}
for city in ['city1','city2']:
 world=(root/f'assets/{city}.ctw').read_bytes();nv,np,nm=struct.unpack_from('<III',world,4);off=20+nv*16+np*24+nm*28;assert world[off:off+4]==b'CTC1';nx,nz,sx,sz,ox,oy,oz=struct.unpack_from('<7i',world,off+4)
 data=(root/f'assets/{city}.ctp').read_bytes();count,routes,dc=struct.unpack_from('<III',data,4);city_counts[city]=dc
 for j in range(dc):
  x,yy,z,pitch,yaw,roll=struct.unpack_from('<6i',data,16+count*32+j*48)
  for focal in [192,128]:
   corners=[]
   for i in range(8):
    depth=256 if i<4 else 38400;xx=120 if i%4 in [1,2] else -120;yy=80 if i%4>=2 else -80;corners.extend([int(xx*depth/focal),int(yy*depth/focal),depth])
   cases.append([x,z,yaw,pitch,nx,nz,sx,sz,ox,oz]+corners)
rows=subprocess.run([str(root/'bin/world-bounds-corners-rom-compare.exe'),str(root/'assets/driving.ctr')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();steps=0
assert len(rows)==len(cases)
for case,row in zip(cases,rows):assert original(case)==list(map(int,row.split())),(case,original(case),row)
report=dict(cases=len(cases),city_destinations=city_counts,actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actual2D18 through2E6C eight inheritedcorners transform/minmax/grid/clamp vs productionnativebounds_corners, signed2CE8divide contract, 24boundaryfixtures plus all60actualcitydestinations at inheritedfocal192/128 with realcityCTW dimensions/spacings/origins andrealCTRtrig; syntheticcornerfixtures, no fullcelltraversal/previewlifecycle/wholeasset extraction proof.')
(root/'reports/world-bounds-corners-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
