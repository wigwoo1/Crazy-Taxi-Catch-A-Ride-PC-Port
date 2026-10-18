"""Original3264..32B0 facing gate vs shared native production helper."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
worlds=[];cases=[];counts={}
for city,name in enumerate(['city1','city2']):
 data=(root/f'assets/{name}.ctw').read_bytes();nv,np,nm=struct.unpack_from('<III',data,4);vertices=data[20:20+nv*16];polys=data[20+nv*16:20+nv*16+np*24];worlds.append((vertices,polys));counts[name]=np
 passengers=(root/f'assets/{name}.ctp').read_bytes();pc,rc,dc=struct.unpack_from('<III',passengers,4);offset=16+pc*32;cameras=[(0,0,0),struct.unpack_from('<3i',passengers,offset),struct.unpack_from('<3i',passengers,offset+(dc-1)*48),(-2147483648,2147483647,-2147483648)]
 for camera in cameras:cases.extend((city,j,*camera) for j in range(np))
code={i.address:i for i in md.disasm(rom[0x3264:0x32b4],0x08003264)}
rows=subprocess.run([str(root/'bin/world-facing-rom-compare.exe'),str(root/'assets/city1.ctw'),str(root/'assets/city2.ctw')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0;accepted=rejected=0
for (city,ref,x,y,z),row in zip(cases,rows):
 vertices,polys=worlds[city];regions={0x10010000:polys,0x12000000:vertices};regs={'sb':0x10010000+ref*24,'r5':0x10000000,'r1':x&0xffffffff,'r2':y&0xffffffff,'r3':z&0xffffffff};pc=0x08003264;eq=False;negative=False
 def val(op):
  v=op.imm if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
  return (v<<op.shift.value)&0xffffffff if op.shift.type else v
 while pc not in [0x08003aac,0x080032b4]:
  i=code[pc];steps+=1;mn=i.mnemonic;ops=i.operands;nxt=pc+4
  if mn in ['ldr','ldrb','ldrh','ldrsb']:
   at=regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp;n=4 if mn=='ldr' else 2 if mn=='ldrh' else 1
   if at==0x10000024:v=0x12000000
   else:
    v=None
    for start,data in regions.items():
     if start<=at and at+n<=start+len(data):v=int.from_bytes(data[at-start:at-start+n],'little');break
    assert v is not None,hex(at)
   if mn=='ldrsb' and v&128:v-=256
   regs[md.reg_name(ops[0].reg)]=v&0xffffffff
  elif mn in ['add','sub','mul','mla']:
   a=val(ops[1]);b=val(ops[2]);v=a+b if mn=='add' else a-b if mn=='sub' else a*b
   if mn=='mla':v+=val(ops[3])
   regs[md.reg_name(ops[0].reg)]=v&0xffffffff
  elif mn=='tst':eq=(val(ops[0])&val(ops[1]))==0
  elif mn=='cmp':assert val(ops[1])==0;negative=bool(val(ops[0])&0x80000000)
  elif mn in ['bne','blt']:
   if (mn=='bne' and not eq) or (mn=='blt' and negative):nxt=ops[0].imm
  else:raise AssertionError((hex(pc),mn,i.op_str))
  pc=nxt
 result=int(pc==0x080032b4);accepted+=result;rejected+=1-result;assert result==int(row),(city,ref,x,y,z,row)
report=dict(cases=len(cases),city_polygons=counts,accepted=accepted,rejected=rejected,actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Complete original3264..32B0 facing gate vs extracted shared productionnative helper. Everyactualcity polygon at origin/first+last destinationcamera/INTMIN/MAXfixture; bit0bypass/signedbyte normals/firstvertex/wrappedsub/mul/mla/sign exact. Material/cache order alreadyseparatelyqualified; remainingvertex transforms/clipping/packet production andfullframe excluded.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_render.c','src/world_render.h','tests/world-facing-rom-compare.c','tools/compare_world_facing_rom.py']}
(root/'reports/world-facing-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
