"""Actual credits common setup E1BC with nested original F5E8 reset."""
exec(open('outputs/crazy-taxi-native/tools/audit_credits_layer_palette_calls.py').read().split('\np=InitProbe')[0])
for lo,hi in [(0x1e1bc,0x1e1ee),(0xf5e8,0xf62e)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[lo:hi],0x8000000+lo):allowed_thumb[i.address]=bytes(i.bytes)
class SetupProbe(InitProbe):
 def external(self,target):
  self.calls.append(hex(target))
  if target==0x800f5e8:self.run(entry=target,stop=self.reg[14]&~1)
  else:assert target in (0x8025c18,0x800ff20,0x8007378,0x80073c0),hex(target)
results=[]
for count in (0,1,7,16):
 p=SetupProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
 p.write(0x2001780,count,2);p.write(0x2001782,224,2);p.write(0x2001784,256,2)
 for j in range(count*20):p.write(0x2001790+j,165,1)
 p.reg[14]=0x10020001;p.run(entry=0x801e1bc,stop=0x10020000)
 assert p.calls==['0x800f5e8','0x8025c18',*['0x800ff20']*4,'0x8007378','0x80073c0']
 assert all(p.read(a,2)==0 for a in (0x2001780,0x2001782,0x2001784))
 for j in range(count):
  base=0x2001790+j*20
  for off,n in [(0,2),(2,2),(4,2),(6,1),(8,1),(9,1),(10,1),(11,1),(12,1),(13,1),(16,4)]:assert p.read(base+off,n)==0
  for off in (7,14,15):assert p.read(base+off,1)==165
 results.append({'prior_records':count,'bg_allocation_after':0,'obj_allocation_after':0,'records_after':0,'unused_record_padding_retained':True})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':results,'scope':'ActualE1BC andnestedF5E8 opcode execution proves commoncredits setup clears BG/OBJ palette byte offsets and recordcount, representedrecordfields, retains padding. Remaining setup callees captured opaque; pixel/DMA/layer init/fullframe excluded.','source_sha256':hashlib.sha256((root/'tools/audit_credits_registry_reset_rom.py').read_bytes()).hexdigest()}
(root/'reports/credits-registry-reset-rom-audit.json').write_text(json.dumps(report,indent=2));print('PASS',report)
