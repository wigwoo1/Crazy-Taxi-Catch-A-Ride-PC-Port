"""Whole cold graphics bootstrap source path, native-frame comparison; diagnostic only."""
exec(open('outputs/crazy-taxi-native/tools/audit_graphics_bootstrap_real_allocation.py').read().split('\np=RealBindingsProbe')[0])
for entry,end in [(0x802ac6c,0x802af36),(0x802cae4,0x802caee),(0x802c834,0x802c844),(0x802c790,0x802c826),(0x802c784,0x802c790),(0x802b44c,0x802b524),(0x801e7f0,0x801e862),(0x802b330,0x802b442),(0x802c558,0x802c686),(0x802b0b0,0x802b32e)]:
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
exec(source.replace('self.steps<50000','self.steps<200000').replace("'bne':not z","'bhs':c,'blo':not c,'bne':not z"));allowed_thumb.update(saved_allowed)
arm={};exec(open('outputs/crazy-taxi-native/tools/compare_projection_divide_rom.py').read().split('\nvalues=')[0],arm)
class BootstrapProbe(RealBindingsProbe):
 run=MotionProbe.run
 def external(self,target):
  if target==0x802c8c4:return super().external(target)
  self.calls.append((target,self.reg[:4].copy()))
  if target in [0x802cae4,0x802db54,0x802c834,0x802c790,0x802c784,0x802b44c,0x801e7f0,0x802b330,0x802c558,0x802b0b0]:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032488:
   assert self.reg[1]==192
   n=self.reg[0] if self.reg[0]<0x80000000 else self.reg[0]-0x100000000
   self.reg[0]=(abs(n)//192*(-1 if n<0 else 1))&0xffffffff
  elif target==0x8032454:
   assert self.reg[2]==0x3000008+0x29fc-0x700
   self.reg[0]=arm['original'](self.reg[0],self.reg[1]);self.reg[1:4]=[0xbad00001,0xbad00002,0xbad00003]
  else:raise AssertionError(('unexpectedbootstraphelper',hex(target)))
p=BootstrapProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768),0x4000000:bytearray(1024)};p.scalars={};p.stores=[];p.published_copy=False;p.calls=[];p.contracts=[]
descriptor=int.from_bytes(r[0x4a4f8c:0x4a4f90],'little');far=(int.from_bytes(r[0x4a4f90:0x4a4f94],'little')<<8)&0xffffffff
p.write(0x2003899,165,1);p.reg[:4]=[descriptor,0,0,240];p.reg[13]=0x10000400;p.reg[14]=0x10020001
for j,v in enumerate([160,256,far]):p.write(0x10000400+j*4,v)
saved=p.reg[4:12].copy();p.run(entry=0x802ac6c,stop=0x10020000)
assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
assert p.read(0x2003899,1)==165 and p.read(0x202c4ec)==0
assert not p.contracts
assert p.read(0x3006810)==0x30069c0
assert p.regions[0x3000000][0x69c0:0x6a28]==r[descriptor-0x8000000:descriptor-0x8000000+104]
for base,length in [(0x201b6ec,49152),(0x20276ec,16384),(0x202b6ec,512)]:assert bytes(p.regions[0x2000000][base-0x2000000:base-0x2000000+length])==bytes(length)
for base in [p.read(0x300692c),p.read(0x3006938)]:assert [p.read(base+j*4) for j in range(4)]==[0,0,240,160]
assert p.read(0x300683c)==256 and p.read(0x3006840)==far
case=[p.read(0x3006810+o) for o in [8,12,16,20,24,28,40,44,48]]+[p.read(0x30069c0+0x40),p.read(0x30069c0+0x44),0]
binary=root/'bin/render-frame-init-rom-compare.exe'
row=subprocess.run([str(binary),"--constant-padding"],input=' '.join(f'{v:08x}' for v in case)+'\n',text=True,capture_output=True,check=True).stdout.split()
expected=[int(v,16) for v in row[:40]]
actual=[p.read(0x3006810+o) for o in range(0x50,0xf0,4)]
assert actual==expected,(actual,expected)
report={'status':'PASS_COLD_SOURCE_PATH','source_Thumb_instructions':p.steps,'boot_descriptor':f'{descriptor:08X}','bounds':[0,0,240,160],'near_plane':256,'far_plane':far,'allocator_cursor_after_bootstrap':0,'retained_parent_flag':165,'service_calls':[{'target':f'{t:08X}','arguments':args} for t,args in p.calls],'contracts':[{'target':f'{t:08X}','arguments':args} for t,args in p.contracts],'scope':'Actualwhole2AC6C coldcursor/zeroRAM bootdescriptor path includingactual2B44C registration/1E7F0 descriptorcopy,actual2C558 mode0 bypass twice,actual2B0B0 zero shade bypass,actual2B330 frame initialization and originalARM029FC quotient result via32454 dispatch (r1-r3 poisoned, notexact scratch outputs). Signeddivide192 explicitcontract. Sourcecaller returns with3899 retained underthese cold path owners. CopiedARM code neverexecuted; boundedoriginalROM quotient diagnostic only. Warmboot/nonzero shade/bitmap4/fullboot/nativeintegration excluded.','native_frame_words_compared':40,'native_frame_comparator_sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'rom_sha256':hashlib.sha256(r).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/graphics-bootstrap-whole-control-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS coldbootstrap actualregistration/frame/clear path',p.steps,'Thumb',arm['steps'],'ARM quotient instructions')
