from pathlib import Path
import json,math,types,hashlib
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/verify_bound_background_bootstrap_input_rom.py').read_text().split('\nrows=[]')[0],ns)
for i in ns['ns']['md'].disasm(ns['ns']['r'][0x203c4:0x204bc],0x80203c4):ns['ns']['allowed_thumb'][i.address]=bytes(i.bytes)
rows=[]
for seed in [0,165,255]:
 p=ns['Probe']((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768),0x4000000:bytearray(1024)};p.scalars={};p.stores=[];p.published_copy=False;p.calls=[];p.contracts=[]
 p.regions[0x3000000][0x69c0:0x6a28]=ns['ns']['r'][0x7ebd98:0x7ebe00];p.write(0x3006810,0x30069c0)
 for off in range(66):p.write(0x20034dc+off,(seed+off*13)&255,1)
 before=bytes(p.regions[0x2000000][0x34dc:0x351e]);p.observing=True;external_calls=[]
 def external(self,t):
  external_calls.append(hex(t))
  if t==0x8033f80:
   def string(a):
    v=bytearray()
    for i in range(256):
     c=self.read(a+i,1)
     if not c:return bytes(v)
     v.append(c)
    raise AssertionError('unbounded string')
   a,b=string(self.reg[0]),string(self.reg[1]);self.reg[0]=((a>b)-(a<b))&0xffffffff
  elif t==0x8032038:self.reg[0]=math.isqrt(self.reg[0])
  elif t==0x8032488:
   a,b=ns['ns']['signed'](self.reg[0]),ns['ns']['signed'](self.reg[1]);assert b;self.reg[0]=ns['ns']['trunc'](a,b)&0xffffffff
  else:raise AssertionError(('unqualified terrain child',hex(t)))
 p.external=types.MethodType(external,p);p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x80203c4,stop=0x10020000)
 assert before==bytes(p.regions[0x2000000][0x34dc:0x351e]);rows.append({'seed':seed,'instructions':p.steps,'boundary_count':p.read(0x2005394),'calls':external_calls})
report={'status':'PASS_ORIGINAL_BOUND_TERRAIN_RESOURCE_OWNED_INPUT_RETAINED','cases':rows,'scope':'Whole203C4 onactualBoundsceneheader/source arrays,66inputretained/nooverlappingcaller stores. String/sqrt/divisionregister-onlycontracts; theydonotprovewholecallee memoryeffects orfullbootstrap. Originalerror2C880child notaccepted.'}
(root/'reports/bound-terrain-resource-input-rom.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS Boundterrainresource',rows)
