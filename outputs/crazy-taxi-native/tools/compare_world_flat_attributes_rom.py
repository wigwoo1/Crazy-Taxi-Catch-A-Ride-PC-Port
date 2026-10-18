"""Complete original3800..3834 flatshade record writes vs native material adapter."""
exec(open('outputs/crazy-taxi-native/tools/compare_world_polygon_stamp_rom.py').read().split('values=')[0])
cases=[];counts={}
for name in ['city1','city2']:
 data=(root/f'assets/{name}.ctw').read_bytes();nv,np,nm=struct.unpack_from('<III',data,4);polys=data[20+nv*16:20+nv*16+np*24];mats=data[20+nv*16+np*24:20+nv*16+np*24+nm*28];counts[name]=0
 for j in range(np):
  poly=polys[j*24:j*24+24]
  if mats[poly[0]*28+16]!=255:continue
  n=3 if int.from_bytes(poly[14:16],'little')&0x8000 else 4;cases.append([n]+list(struct.unpack_from('<4H',poly,16)));counts[name]+=1
for n in [3,4]:
 for values in [(0,0,0,0),(1,2,3,4),(255,256,4095,4096),(65535,32768,32767,65534)]:cases.append([n,*values])
code={i.address:i for i in md.disasm(rom[0x3800:0x3838],0x08003800)}
rows=subprocess.run([str(root/'bin/world-flat-attributes-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 count,*attributes=case;poly=bytearray(24);struct.pack_into('<4H',poly,16,*attributes);packet=bytearray([165])*144;regs={'sb':0x10010000,'r4':0x10020000};pc=0x08003800
 def val(op):
  v=op.imm if op.type==ARM_OP_IMM else regs[md.reg_name(op.reg)]
  return (v<<op.shift.value)&0xffffffff if op.shift.type else v
 while pc!=0x08003838:
  i=code[pc];steps+=1;mn=i.mnemonic;ops=i.operands
  if mn=='ldrh':at=regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp-0x10010000;regs[md.reg_name(ops[0].reg)]=int.from_bytes(poly[at:at+2],'little')
  elif mn=='lsl':regs[md.reg_name(ops[0].reg)]=val(ops[1])
  elif mn=='eor':regs[md.reg_name(ops[0].reg)]=val(ops[1])^val(ops[2])
  elif mn=='str':at=regs[md.reg_name(ops[1].mem.base)]+ops[1].mem.disp-0x10020000;packet[at:at+4]=val(ops[0]).to_bytes(4,'little')
  else:raise AssertionError((hex(pc),mn,i.op_str))
  pc+=4
 assert int.from_bytes(packet[12:16],'little')==0
 expected=[255]+[int.from_bytes(packet[at:at+4],'little') for at in [44,76,108,140]]
 assert expected==list(map(int,row.split())),(case,expected,row)
 written={j for at in [12,44,76,108,140] for j in range(at,at+4)};assert all(b==165 for j,b in enumerate(packet) if j not in written)
report=dict(cases=len(cases),actual_city_flat_polygons=counts,actual_arm_instructions=steps,pass_match=True,rom_sha256=hashlib.sha256(rom).hexdigest(),scope='Complete3800..3834 fourflatshade attributes/NULLdescriptor writes, everyactualcityflatpolygon plus signedhalfword-boundaryfixtures, trianglesretainfourthattribute. Native255texture identity maps NULLboundary, geometrycount/activeXYZ retainednativechecked; sourceother144bytepacket fields unchangedguard. Fullpacketproducer/raster/pixel/frame unqualified.')
report['sha256']={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/world_material.c','tests/world-flat-attributes-rom-compare.c','tools/compare_world_flat_attributes_rom.py']}
(root/'reports/world-flat-attributes-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
