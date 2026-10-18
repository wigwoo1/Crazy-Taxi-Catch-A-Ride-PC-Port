"""Actual36EC..373C animated texture selection against native material helper."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
phases=[0,1,-1,4095,4096,-4096,2147483647,-2147483648,123456789];cases=[];counts={}
for name in ['city1','city2']:
 data=(root/f'assets/{name}.ctw').read_bytes();nv,np,nm=struct.unpack_from('<III',data,4);start=20+nv*16+np*24;counts[name]=nm
 for j in range(nm):
  m=data[start+j*28:start+(j+1)*28]
  for phase in phases:cases.append((m[16],m[17],int.from_bytes(m[18:20],'little'),phase))
for base in [0,3,240,255]:
 for count in [0,1,2,7,255]:
  for rate in [0,1,4096,65535]:
   cases.extend((base,count,rate,phase) for phase in phases)
code={i.address:i for i in md.disasm(rom[0x36ec:0x3740],0x080036ec)};steps=0;expected=[];valid=[];guard_cases=0;actual_excluded=0
for case_index,(base,count,rate,phase) in enumerate(cases):
 memory={0x10000010:base,0x10000011:count,0x10000012:rate,0x03006904:phase&0xffffffff,0x10010020:0x12000000};regs={'r7':0x10000000,'fp':0x03006810,'r8':0x10010000,'r4':0x10020000,'sp':0x10030000};eq=le=False;pc=0x080036ec
 def signed(v):return v if v<0x80000000 else v-0x100000000
 def val(op):
  v=op.imm if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
  return (v<<op.shift.value)&0xffffffff if op.shift.type else v
 while pc not in [0x08003740,0x08003800]:
  i=code[pc];steps+=1;ops=i.operands;mn=i.mnemonic;nxt=pc+4
  if mn in ['ldrb','ldrh','ldr']:regs[md.reg_name(ops[0].reg)]=memory[regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp]
  elif mn=='str':memory[regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp]=val(ops[0])
  elif mn=='cmp':a=signed(val(ops[0]));b=signed(val(ops[1]));eq=a==b;le=a<=b
  elif mn in ['beq','ble']:
   if eq if mn=='beq' else le:nxt=ops[0].imm
  elif mn in ['mul','sub','add']:a=val(ops[1]);b=val(ops[2]);regs[md.reg_name(ops[0].reg)]=(a*b if mn=='mul' else a-b if mn=='sub' else a+b)&0xffffffff
  elif mn=='asr':regs[md.reg_name(ops[0].reg)]=(signed(regs[md.reg_name(ops[1].reg)])>>ops[1].shift.value)&0xffffffff
  elif mn in ['push','pop']:
   names=[md.reg_name(o.reg) for o in ops];at=regs['sp'] if mn=='pop' else regs['sp']-len(names)*4
   for j,name in enumerate(names):
    if mn=='pop':regs[name]=memory[at+j*4]
    else:memory[at+j*4]=regs[name]
   regs['sp']=at+len(names)*4 if mn=='pop' else at
  elif mn=='bl':
   assert ops[0].imm==0x08002ce8;a=signed(regs['r0']);b=signed(regs['r1']);regs['r0']=(abs(a)//abs(b)*(-1 if (a<0)!=(b<0) else 1))&0xffffffff
  else:raise AssertionError((hex(pc),mn,i.op_str))
  pc=nxt
 selected=255 if pc==0x08003800 else signed(regs['r0'])
 if selected!=255 and not 0<=selected<256:guard_cases+=1;actual_excluded+=int(case_index<sum(counts.values())*len(phases));continue
 if pc!=0x08003800:assert memory[0x1002000c]==0x12000000+selected*12
 valid.append((base,count,rate,phase));expected.append([0,selected])
rows=subprocess.run([str(root/'bin/world-texture-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in valid),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(expected)
for case,want,row in zip(valid,expected,rows):assert want==list(map(int,row.split())),(case,want,row)
report=dict(cases=len(valid),actual_city_materials=counts,excluded_source_outside_fixture=guard_cases,actual_city_cases_outside_fixture=actual_excluded,actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actual36EC..373C texture selection with flat255 dispatch anddescriptoroffset vsnative helper. Allactual480city material selectionfields exercised at9signed phases plus boundaryfixtures; selectedindices outsideexplicitowner excluded fromnative match. Explicit256descriptor owner; outside-owner sourceindices excluded, notsourceinvalid-path/nativeguard qualification. Signed2CE8divide contract; materialbytes retainednativecheck. FullUV/flatbranch/packet/frame unqualified.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_material.c','tests/world-texture-rom-compare.c','tools/compare_world_texture_rom.py']}
(root/'reports/world-texture-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
