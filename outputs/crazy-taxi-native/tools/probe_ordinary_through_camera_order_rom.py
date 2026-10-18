"""Execute original ordinary caller through its first movement/camera call; offline only."""
import pathlib,json
ns={};root=pathlib.Path('outputs/crazy-taxi-native')
exec((root/'tools/compare_session_init_rom.py').read_text().split('\ncases=')[0],ns)
todo=[0x800afd4];seen=set()
while todo:
 a=todo.pop()
 if a in seen or a==0x800b320:continue
 assert 0x800afd4<=a<0x800b320,hex(a)
 seen.add(a);i=next(ns['md'].disasm(ns['r'][a-0x8000000:a-0x8000000+4],a,count=1));ns['allowed_thumb'][a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class FirstCall(Exception):pass
class CallerProbe(ns['InitProbe']):
 def external(self,target):
  self.calls.append(hex(target))
  assert target in [0x8029264,0x80130ac,0x8011a3c,0x8015ac8,0x80145e4,0x8020d2c,0x8021078,0x8021220,0x80214f8,0x80208d4,0x8014dd8,0x8034074,0x802360c],('unqualified caller dependency',hex(target))
  # Callee bodies are explicit ordered-call boundaries, not physics/effect proofs.
  self.reg[0]=0xffffffff if target==0x8034074 else 0
rows=[]
for active in [0,1]:
 p=CallerProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
 p.scalars.update({0x2001700:0,0x2001704:0,0x2003b00:1,0x30053c0:1,0x3004ca0:0x2002634,0x2001768:0xffffffff,0x200170c:0,0x2001714:0,0x2002634:0,0x200263c:0})
 p.write(0x2005f20,0,1);p.write(0x2001696,active,1);p.write(0x2001697,0,1);p.write(0x2001708,0,1)
 p.reg[9]=0x2001708;p.reg[7]=0x2002634;p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x800afd4,stop=0x800b320)
 rows.append(dict(active=active,calls=p.calls,instructions=p.steps))
report=dict(status='PASS',cases=rows,scope='Original AFD4 through B31C camera call ordering, explicit no-state callee contracts, no-customer1768=-1 fixture and ordinary zero gates; numerical callee behavior qualified separately, not full original caller state or framebuffer proof.')
(root/'reports/ordinary-through-camera-order-rom.json').write_text(json.dumps(report,indent=2));print(report)
