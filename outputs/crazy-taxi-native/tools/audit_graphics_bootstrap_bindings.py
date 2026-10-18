"""Actual original bootstrap relocation stores; diagnostic-only copy contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True
for i in md.disasm(r[0x2ac6c:0x2ae5a],0x0802ac6c):allowed_thumb[i.address]=bytes(i.bytes)
class BindingProbe(ExitProbe):
 def external(self,target):
  assert target==0x0802c8c4,hex(target)
  self.copy=(self.reg[0],self.reg[1]);self.reg[0]=0x02010000
 def write(self,a,v,size=4):
  if hasattr(self,'stores') and (0x03006800<=a<0x03007000 or 0x02010000<=a<0x02014c00):self.stores.append((a,v,size))
  if 0x03006800<=a<0x03007000 or 0x02010000<=a<0x02014c00:return
  super().write(a,v,size)
p=BindingProbe((0,0,0,0,0,1,0,0,0,0));p.stores=[];p.reg[13]=0x10000400
p.run(entry=0x0802ac6c,stop=0x0802ae5a)
base,length=p.copy
bindings=[]
for address,value,size in p.stores:
 assert size==4 and 0x02010000<=value<0x02010000+length,(hex(address),hex(value))
 source=(value-0x02010000+base)&MASK
 bindings.append({'slot':f'{address:08X}','source_identity':f'{source:08X}','kind':'relocated-code-patch' if 0x02010000<=address<0x02010000+length else 'global-binding'})
for b in bindings:
 if b['source_identity'] in ['08001DB0','08000C4C']:b['semantics']='mutable viewport bounds data, fourwords, not a callable function'
 elif b['source_identity']=='08001DC4':b['semantics']='embedded mutable data/pointer block, not ARM entry; consumers pending'
report={'status':'SOURCE_EXECUTED','actual_thumb_instructions':p.steps,'copy_source':f'{base:08X}','copy_length':length,'bindings':bindings,'scope':'Actual original2AC6C..AE58 valid ROM code-size guard, all global bindings and in-copy pointer patch stores executed; some identities are mutable data, not functions. 2C8C4 copy allocator explicit fixture returns02010000; no copied code executed. Source identities require typed native implementations, never native dereference of ROM addresses. Tail/bootstrap/runtime integration excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/graphics-bootstrap-bindings-source-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(bindings),'bindings',p.steps,'Thumb instructions','copy length',length)
