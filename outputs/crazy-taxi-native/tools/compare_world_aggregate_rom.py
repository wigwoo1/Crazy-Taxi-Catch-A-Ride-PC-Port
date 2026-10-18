"""Original triangle/quad polygon aggregate against productionnative helper."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_vertex_rom.py').read().split('\nrows=subprocess.run')[0])
vertex_cases=cases
rows=subprocess.run([str(root/'bin/world-vertex-rom-compare.exe'),str(root/'assets/city1.ctw'),str(root/'assets/city2.ctw'),str(root/'assets/driving.ctr')],input=''.join(' '.join(map(str,c))+'\n' for c in vertex_cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(vertex_cases)
vertex_data={tuple(c):list(map(int,row.split())) for c,row in zip(vertex_cases,rows)};cases=[];kinds={3:0,4:0}
for city,name in enumerate(['city1','city2']):
 data=(root/f'assets/{name}.ctw').read_bytes();nv,np,nm=struct.unpack_from('<III',data,4);polys=data[20+nv*16:20+nv*16+np*24];cameras=[c[2:] for c in vertex_cases if c[0]==city and c[1]==0]
 for camera in cameras:
  for j in range(np):
   poly=polys[j*24:j*24+24];n=3 if struct.unpack_from('<H',poly,14)[0]&0x8000 else 4;case=[n]
   for i in range(n):
    ref=struct.unpack_from('<H',poly,8+i*2)[0];v=vertex_data[(city,ref,*camera)];case.extend([v[3],v[2]])
   cases.append(case);kinds[n]+=1
for n in [3,4]:
 for z in [0,1,-1,2147483647,-2147483648]:
  for flags in [[0]*n,[1]*n,[16]*n,[1,2,4,8][:n]]:cases.append([n]+[v for i in range(n) for v in [flags[i],z]])
code={i.address:i for lo,hi in [(0x33f0,0x3424),(0x3548,0x3584)] for i in md.disasm(rom[lo:hi],0x08000000+lo)}
max_code={i.address:i for i in md.disasm(rom[0x33c0:0x33d0],0x080033c0)}
rows=subprocess.run([str(root/'bin/world-aggregate-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0;accepted=0
for case,row in zip(cases,rows):
 n=case[0];maxdepth=0
 for z in case[2::2]:
  maxregs={'r3':z&0xffffffff};positive=False
  for at,i in max_code.items():
   steps+=1;ops=i.operands
   if i.mnemonic=='ldr':
    literal=at+8+ops[1].mem.disp;owner=int.from_bytes(rom[literal-0x08000000:literal-0x08000000+4],'little');assert literal==0x08003af0
    maxregs['r4']=maxdepth&0xffffffff
   elif i.mnemonic=='cmp':positive=z>maxdepth
   elif i.mnemonic=='movgt':
    if positive:maxregs['r4']=maxregs['r3']
   elif i.mnemonic=='str':assert at+8+ops[1].mem.disp==0x08003af0;maxdepth=maxregs['r4'] if maxregs['r4']<0x80000000 else maxregs['r4']-0x100000000
   else:raise AssertionError(i.mnemonic)
 mem={0x10000010+i*32:case[1+i*2] for i in range(n)};regs={'r0':0x10000000,'r5':sum(case[2::2])&0xffffffff,'sp':0x10010000};eq=False;pc=0x080033f0 if n==4 else 0x08003548;stop=0x08003424 if n==4 else 0x08003584
 def signed(v):return v if v<0x80000000 else v-0x100000000
 def val(op):return op.imm if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
 while pc not in [stop,0x080038a0]:
  i=code[pc];steps+=1;ops=i.operands;mn=i.mnemonic;nxt=pc+4
  if mn=='ldr':regs[md.reg_name(ops[0].reg)]=mem[regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp]
  elif mn in ['and','ands','orr']:v=val(ops[1])|val(ops[2]) if mn=='orr' else val(ops[1])&val(ops[2]);regs[md.reg_name(ops[0].reg)]=v;eq=(v==0) if mn=='ands' else eq
  elif mn=='bne':
   if not eq:nxt=ops[0].imm
  elif mn=='asr':regs[md.reg_name(ops[0].reg)]=(signed(val(ops[1]))>>ops[1].shift.value)&0xffffffff
  elif mn=='mov':regs[md.reg_name(ops[0].reg)]=val(ops[1])
  elif mn=='bl':
   assert ops[0].imm==0x08002ce8;a=signed(regs['r0']);b=signed(regs['r1']);regs['r0']=(abs(a)//abs(b)*(-1 if (a<0)!=(b<0) else 1))&0xffffffff
  elif mn in ['push','pop','stmdb']:
   names=[md.reg_name(o.reg) for o in (ops[1:] if mn=='stmdb' else ops)];at=regs['sp'] if mn=='pop' else regs['sp']-4*len(names)
   for j,name in enumerate(names):
    if mn=='pop':regs[name]=mem[at+j*4]
    else:mem[at+j*4]=regs[name]
   regs['sp']=at+4*len(names) if mn=='pop' else at
  else:raise AssertionError((hex(pc),mn,i.op_str))
  pc=nxt
 result=pc==stop;accepted+=int(result);want=[1,signed(mem[regs['sp']]),regs['r5'],maxdepth] if result else [0,0,0,0];assert want==list(map(int,row.split())),(case,want,row)
report=dict(cases=len(cases),real_city_polygon_cases=kinds,accepted=accepted,actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actualquad33F0..3420/triangle3548..3580 ANDreject/ORflags/wrappeddepthmean vs sharedproductionaggregate; allcitypolygons atfourcamera fixtures, source-qualifiednativevertex outputs as explicit input boundary plus signeddepthfixtures. Original2CE8division contract. Actual33C0..33CC mutablemaxdepth scratch startszero andsignedCMP/MOVGT/store executedforeach vertex; acceptednative maxdepth exact. Vertexretention nottested; whole31ECpacket/raster/frame unqualified.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_render.c','src/world_render.h','tests/world-aggregate-rom-compare.c','tools/compare_world_aggregate_rom.py','bin/world-vertex-rom-compare.exe']}
(root/'reports/world-aggregate-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
