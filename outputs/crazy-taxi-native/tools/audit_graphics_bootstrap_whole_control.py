"""Whole graphics bootstrap, actual allocator/storage/trig/viewport; explicit remaining service contracts."""
exec(open('outputs/crazy-taxi-native/tools/audit_graphics_bootstrap_real_allocation.py').read().split('\np=RealBindingsProbe')[0])
for entry,end in [(0x802ac6c,0x802af36),(0x802cae4,0x802caee),(0x802c834,0x802c844),(0x802c790,0x802c826),(0x802c784,0x802c790)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<end,(hex(a),hex(entry))
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic in branches:todo.append(i.operands[0].imm)
for line in (root/'reports/boot-0802db54-source.txt').read_text().splitlines()[1:]:
 a=int(line[:8],16);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
saved_allowed=allowed_thumb.copy()
exec(source.replace('self.steps<50000','self.steps<200000'));allowed_thumb.update(saved_allowed)
class BootstrapProbe(RealBindingsProbe):
 run=MotionProbe.run
 def external(self,target):
  if target==0x802c8c4:return super().external(target)
  self.calls.append((target,self.reg[:4].copy()))
  if target in [0x802cae4,0x802db54,0x802c834,0x802c790,0x802c784]:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032488:
   assert self.reg[1]==192
   n=self.reg[0] if self.reg[0]<0x80000000 else self.reg[0]-0x100000000
   self.reg[0]=(abs(n)//192*(-1 if n<0 else 1))&0xffffffff
  elif target in [0x802b44c,0x802b330,0x802c558]:self.contracts.append((target,self.reg[:4].copy()))
  else:raise AssertionError(('unexpectedbootstraphelper',hex(target)))
p=BootstrapProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768),0x4000000:bytearray(1024)};p.scalars={};p.stores=[];p.published_copy=False;p.calls=[];p.contracts=[]
descriptor=int.from_bytes(r[0x4a4f8c:0x4a4f90],'little');far=(int.from_bytes(r[0x4a4f90:0x4a4f94],'little')<<8)&0xffffffff
p.write(0x2003899,165,1);p.reg[:4]=[descriptor,0,0,240];p.reg[13]=0x10000400;p.reg[14]=0x10020001
for j,v in enumerate([160,256,far]):p.write(0x10000400+j*4,v)
saved=p.reg[4:12].copy();p.run(entry=0x802ac6c,stop=0x10020000)
assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
assert p.read(0x2003899,1)==165 and p.read(0x202c4ec)==0
assert [t for t,args in p.calls]==[0x802cae4,0x802db54,0x802b44c,0x802c834,0x802c790,0x8032488,0x8032488,0x802c784,0x802b330,0x802c558]
assert p.contracts[0][1][0]==descriptor and p.contracts[-1][1][:3]==[0,0,0]
for base,length in [(0x201b6ec,49152),(0x20276ec,16384),(0x202b6ec,512)]:assert bytes(p.regions[0x2000000][base-0x2000000:base-0x2000000+length])==bytes(length)
for base in [p.read(0x300692c),p.read(0x3006938)]:assert [p.read(base+j*4) for j in range(4)]==[0,0,240,160]
assert p.read(0x300683c)==256 and p.read(0x3006840)==far
report={'status':'PASS_WITH_EXPLICIT_SERVICE_CONTRACTS','source_Thumb_instructions':p.steps,'boot_descriptor':f'{descriptor:08X}','bounds':[0,0,240,160],'near_plane':256,'far_plane':far,'allocator_cursor_after_bootstrap':0,'retained_parent_flag':165,'service_calls':[{'target':f'{t:08X}','arguments':args} for t,args in p.calls],'contracts':[{'target':f'{t:08X}','arguments':args} for t,args in p.contracts],'scope':'Actualwhole2AC6C caller return with actual2C8C4 allocator/logicalDMAcopy, actualstorageclear loops, actual2CAE4 allocatorreset, actual2DB54 tables, actual2C834 transformselectionreset, actual2C790 viewport andactual2C784 depth. Signeddivide192 contract. 2B44C scene-registration,2B330 frame-initialization,2C558 clear-frame explicit no-mutation service contracts; therefore no wholeboot retainedparent claim through those services and no fullrenderer initialization claim. CopiedARM code neverexecuted.','rom_sha256':hashlib.sha256(r).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/graphics-bootstrap-whole-control-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS wholebootstrap control with 3 explicitservice contracts',p.steps,'Thumb')
