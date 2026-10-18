"""Actual renderer relocation prefix with actualIWRAMcopy allocator; diagnostic only."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
branches={'beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl'}
for entry,stop in [(0x802ac6c,0x802ae5a),(0x802c8c4,0x802c906)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a==stop or a in seen:continue
  assert entry<=a<stop,hex(a)
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic in branches:todo.append(i.operands[0].imm)
class RealBindingsProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):
  assert target==0x802c8c4,hex(target)
  self.copy=(self.reg[0],self.reg[1]);self.run(entry=target,stop=self.reg[14]&~1)
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if hasattr(self,'stores') and (0x3006800<=a<0x3007000 or getattr(self,'published_copy',False) and 0x3000008<=a<0x3004964):self.stores.append((a,v,size))
  if a==0x40000dc:
   assert v==0x84001257,hex(v)
   src,dst=self.read(0x40000d4),self.read(0x40000d8);count=(v&65535)*4
   assert (src,dst,count)==(0x8000700,0x3000008,18780)
   self.regions[0x3000000][8:8+count]=r[src-0x8000000:src-0x8000000+count];self.published_copy=True
p=RealBindingsProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768),0x4000000:bytearray(1024)};p.scalars={};p.stores=[];p.published_copy=False
p.write(0x2003899,165,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001
p.run(entry=0x802ac6c,stop=0x802ae5a)
assert p.copy==(0x8000700,18780) and p.read(0x202c4ec)==18780 and p.read(0x2003899,1)==165
bindings=[]
for address,value,size in p.stores:
 assert size==4 and 0x3000008<=value<0x3000008+18780,(hex(address),hex(value))
 bindings.append({'slot':f'{address:08X}','source_identity':f'{value-0x3000008+0x8000700:08X}','kind':'relocated-code-patch' if 0x3000008<=address<0x3000008+18780 else 'global-binding'})
old=json.loads((root/'reports/graphics-bootstrap-bindings-source-audit.json').read_text())
assert len(bindings)==len(old['bindings'])
for a,b in zip(bindings,old['bindings']):
 expectedslot=int(b['slot'],16)
 if b['kind']=='relocated-code-patch':expectedslot=expectedslot-0x2010000+0x3000008
 assert (a['slot'],a['source_identity'],a['kind'])==(f'{expectedslot:08X}',b['source_identity'],b['kind'])
report={'status':'PASS','original_Thumb_instructions':p.steps,'copy_source':'08000700','copy_destination':'03000008','copy_length':18780,'allocator_cursor_address':'0202C4EC','allocator_cursor_after':18780,'retained_parent_flag':165,'bindings':bindings,'scope':'Actual2AC6C..AE58 prefix includingactual2C8C4 allocator withcoldcursor0 andlogicalDMArawROMbytescopy toIWRAM03000008. Allbindingandcopiedcodepatch stores executed; normalizedidentities matchearlier02010000allocatorfixture, butrealplacement supersedesfixture. Parent3899 retained. CopiedARMcode neverexecuted; productionmustuse native typedservices. Bootstrap tail/actualwholeboot/runtime excluded.','verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/graphics-bootstrap-real-allocation-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS actualrenderer allocation/bindings',len(bindings),'bindings',p.steps,'Thumb')
class AllocatorLimitProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):
  assert target==0x802c880,hex(target)
  raise OverflowError('original allocator fatal path')
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   src,dst=self.read(0x40000d4),self.read(0x40000d8);count=(v&65535)*4
   assert v==0x84001257 and src==0x8000700
   offset=dst-0x3000000
   self.regions[0x3000000][offset:offset+count]=r[0x700:0x700+count]
allocator_cases=[]
for cursor in [0,4,672,676,680]:
 q=AllocatorLimitProbe((0,0,0,0,0,1,0,0,0,0));q.regions={0x2000000:bytearray(262144),0x3000000:bytearray([165])*32768,0x4000000:bytearray(1024)};q.scalars={}
 q.write(0x202c4ec,cursor);q.reg[0]=0x8000700;q.reg[1]=18780;q.reg[13]=0x10000400;q.reg[14]=0x10020001
 saved=q.reg[4:12].copy();rejected=False
 try:q.run(entry=0x802c8c4,stop=0x10020000)
 except OverflowError:rejected=True
 assert rejected==(cursor+18780>19456)
 assert q.read(0x202c4ec)==cursor+18780
 if not rejected:
  assert q.reg[0]==0x3000008+cursor and q.reg[13]==0x10000400 and q.reg[4:12]==saved
  assert q.regions[0x3000000][8+cursor:8+cursor+18780]==r[0x700:0x700+18780]
  assert q.regions[0x3000000][:8+cursor]==bytes([165])*(8+cursor)
  assert q.regions[0x3000000][8+cursor+18780:]==bytes([165])*(32768-8-cursor-18780)
 else:assert q.regions[0x3000000]==bytes([165])*32768
 allocator_cases.append({'cursor_before':cursor,'cursor_after':cursor+18780,'fatal_path':rejected,'Thumb_instructions':q.steps})
report['allocator_limit_cases']=allocator_cases
report['rom_sha256']=hashlib.sha256(r).hexdigest()
report['verifier_sha256']=hashlib.sha256(Path(__file__).read_bytes()).hexdigest()
(root/'reports/graphics-bootstrap-real-allocation-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS original allocator limit',len(allocator_cases),'cases; exact limit accepted, overflow fatal before DMA')
