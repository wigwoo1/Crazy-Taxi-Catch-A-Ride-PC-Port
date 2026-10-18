from pathlib import Path
import json,types
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/verify_bound_background_bootstrap_input_rom.py').read_text().split('\nrows=[]')[0],ns)
m=ns['ns']['md'];rom=ns['ns']['r'];todo=[0x80227ac];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x80227ac<=a<0x8022cc8,hex(a);seen.add(a);i=next(m.disasm(rom[a-0x8000000:a-0x8000000+4],a,count=1));ns['ns']['allowed_thumb'][a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ['bl','bic','bics']:todo.append(i.operands[0].imm)
for i in m.disasm(rom[0xd764:0xd8a8],0x800d764):ns['ns']['allowed_thumb'][i.address]=bytes(i.bytes)
ground={};exec((root/'tools/compare_bound_ground_contact_rom.py').read_text().split('\nschema=')[0],ground)
cfg=json.loads((root/'reports/effect-height-cfg.json').read_text());ground['allowed_thumb'].update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']})
ground_calls=[]
p=ns['Probe']((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768),0x4000000:bytearray(1024)};p.scalars={};p.stores=[];p.published_copy=False;p.calls=[];p.contracts=[]
p.regions[0x3000000][0x69c0:0x6a28]=rom[0x7ebd98:0x7ebe00];p.write(0x3006810,0x30069c0);p.write(0x2001eb0,128,1);p.write(0x2001537,5,1)
for off in range(66):p.write(0x20034dc+off,(165+off*13)&255,1)
before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.observing=True;requests=[]
class Dependency(Exception):pass
def external(self,t):
 requests.append({'target':hex(t),'args':self.reg[:4].copy()})
 if t==0x8032520:
  a,b=ns['ns']['signed'](self.reg[0]),ns['ns']['signed'](self.reg[1]);assert b;self.reg[0]=(a%b)&0xffffffff
 elif t==0x8032488:
  a,b=ns['ns']['signed'](self.reg[0]),ns['ns']['signed'](self.reg[1]);assert b;self.reg[0]=ns['ns']['trunc'](a,b)&0xffffffff
 elif t==0x8029b44:
  point=[ns['ns']['signed'](self.read(self.reg[0]+off)) for off in (0,4,8)]
  gp=ground['GroundProbe']((0,0,0,0,0,1),0)
  for off,value in zip((0,4,8),point):gp.write(gp.base+off,value)
  gp.arm.mem[0x3006810]=ground['address'];gp.arm.mem[0x3006940]=0x80029fc;gp.reg[0]=gp.base;gp.reg[14]=0x10020001;gp.run(entry=t,stop=0x10020000)
  self.reg[0]=gp.reg[0];ground_calls.append({'point':point,'height':ground['signed'](gp.reg[0]),'instructions':gp.steps})
 elif t==0x800d764:self.run(entry=t,stop=self.reg[14]&~1)
 elif t==0x802c9f0:return
 else:raise Dependency(hex(t))
p.external=types.MethodType(external,p);p.reg[13]=0x10000400;p.reg[14]=0x10020001;dependency=None
try:p.run(entry=0x80227ac,stop=0x10020000)
except Dependency as e:dependency=str(e)
assert before==bytes(p.regions[0x2000000][0x34dc:0x351e])
report={'status':'ORIGINAL_BOUND_ACTOR_INIT_DEPENDENCY_CAPTURE','instructions':p.steps,'next_dependency':dependency,'requests':requests,'ground_calls':ground_calls,'scope':'Actual227AC Boundsettings128/realheader/sourceactor untilfirstunqualifiedcallee; inputshadow retained onexecutedprefix only. Modulo registercontract andD764initialclear no-state explicit. Ground height executed in separate original diagnostic with bounded containment/division contracts; caller receives height only. Notwholeinitializer/actorstateproof.'}
(root/'reports/bound-actor-init-dependency.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
