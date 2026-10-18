"""Actual321C..3260 stamp-before-material-rejection with realcity records."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
worlds=[];cases=[];counts={};baseline=[]
for city,name in enumerate(['city1','city2']):
 data=(root/f'assets/{name}.ctw').read_bytes();nv,np,nm=struct.unpack_from('<III',data,4);polys=data[20+nv*16:20+nv*16+np*24];mats=data[20+nv*16+np*24:20+nv*16+np*24+nm*28];blocked=[j for j in range(np) if mats[28*polys[24*j]+20]&9];worlds.append((np,polys,mats));counts[name]=len(blocked);baseline.append(sum((j+1)*(0xa5a50000+j) for j in range(np)))
 cases.extend((city,j,1,0) for j in blocked)
 for j in [blocked[0],blocked[len(blocked)//2],blocked[-1]]:
  for gen in [0,1,2,0x7fffffff,0x80000000,0xfffffffe,0xffffffff]:
   for prior in [0,1,2,0x7fffffff,0x80000000,0xfffffffe,0xffffffff]:cases.append((city,j,gen,prior))
code={i.address:i for i in md.disasm(rom[0x321c:0x3264],0x0800321c)}
rows=subprocess.run([str(root/'bin/world-material-stamp-rom-compare.exe'),str(root/'assets/city1.ctw'),str(root/'assets/city2.ctw')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for (city,ref,gen,prior),row in zip(cases,rows):
 np,polys,mats=worlds[city];regions={0x10010000:polys,0x11000000:mats};words={0x03006810:0x10000000,0x10000028:0x10010000,0x1000001c:0x11000000};regs={'fp':0x03006810,'sb':ref,'r4':gen};stamp=prior;eq=False;pc=0x0800321c;material_read=False
 def value(op):
  v=op.imm if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
  return (v<<op.shift.value)&0xffffffff if op.shift.type else v
 def read(at,n):
  if at==0x0201b6ec+ref*4:return stamp
  if at in words:return words[at]
  if 0x08000000<=at<0x08800000:return int.from_bytes(rom[at-0x08000000:at-0x08000000+n],'little')
  for start,data in regions.items():
   if start<=at and at+n<=start+len(data):return int.from_bytes(data[at-start:at-start+n],'little')
  raise AssertionError(hex(at))
 while pc not in [0x08003aac,0x08003264]:
  i=code[pc];steps+=1;mn=i.mnemonic;ops=i.operands;nxt=pc+4
  if mn=='ldr' or mn=='ldrb':
   base=md.reg_name(ops[1].mem.base);at=(pc+8 if base=='pc' else regs[base])+ops[1].mem.disp;regs[md.reg_name(ops[0].reg)]=read(at,1 if mn=='ldrb' else 4)
   if pc==0x08003258:material_read=True
  elif mn=='add':regs[md.reg_name(ops[0].reg)]=(value(ops[1])+value(ops[2]))&0xffffffff
  elif mn in ['cmp','tst']:a=value(ops[0]);b=value(ops[1]);eq=a==b if mn=='cmp' else (a&b)==0
  elif mn=='str':assert regs[md.reg_name(ops[1].mem.base)]==0x0201b6ec+ref*4;stamp=value(ops[0])
  elif mn in ['beq','bne']:
   if eq==(mn=='beq'):nxt=ops[0].imm
  else:raise AssertionError((hex(pc),mn,i.op_str))
  pc=nxt
 assert pc==0x08003aac
 checksum=baseline[city]+(ref+1)*(stamp-(0xa5a50000+ref))
 assert [0,int(material_read),stamp,checksum]==list(map(int,row.split())),(city,ref,gen,prior,row)
report=dict(cases=len(cases),blocked_polygons=counts,actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Actual321C..3260 cache/index/materialflags9 prefix vs productionnative visit_stamped then world_render_polygon on every blocked actualcity polygon, plus generation boundaries. Targetstamp/callbackcount/weightedwholecachechecksum exact. Nativechecksum not bytewise allcache proof. Completegeometry/acceptedmaterials/whole31EC/previewframe unqualified.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_render_visit.c','src/world_render.c','src/world_polygon_stamp.c','tests/world-material-stamp-rom-compare.c','tools/compare_world_material_stamp_rom.py','assets/city1.ctw','assets/city2.ctw']}
(root/'reports/world-material-stamp-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
