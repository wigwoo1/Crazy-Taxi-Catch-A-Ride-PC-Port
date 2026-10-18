from pathlib import Path
import json,hashlib,itertools
root=Path('outputs/crazy-taxi-native')
ns={};exec((root/'tools/verify_bound_restart_session_input_rom.py').read_text().split('\nrows=[]')[0],ns)
original=ns['ns']
for lo,hi in [(0xaad0,0xaaf0),(0xabc4,0xabec)]:
 for i in original['md'].disasm(original['r'][lo:hi],0x08000000+lo): original['allowed_thumb'][i.address]=bytes(i.bytes)
qualified={0x0800b6ec,0x08011490,0x0802277c,0x08021c10}
boundaries={0x08014a58,0x0800df8c,0x080227ac,0x0801ee28}
class Probe(ns['Probe']):
 def __init__(self,*args):self.calls=[];super().__init__(*args)
 def external(self,t):
  if t==0x0800abc4 or t in qualified:
   self.calls.append(t);self.run(entry=t,stop=self.reg[14]&~1)
  elif t in boundaries:self.calls.append(t)
  else:super().external(t)
rows=[]
expected=[0x0800abc4,0x08014a58,0x0800b6ec,0x0800df8c,0x080227ac,0x08011490,0x0801ee28,0x0802277c,0x08021c10]
for seed,settings in itertools.product([0,165,255],[128,129,159,255]):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x02000000:bytearray(262144),0x03000000:bytearray(65536)};p.scalars={}
 for off in range(66):p.write(0x020034dc+off,(seed+off*13)&255,1)
 p.write(0x02001eb0,settings,1);p.write(0x02001537,5,1);p.write(0x02007542,240,2);p.write(0x02001ea8,seed,1)
 before=bytes(p.regions[0x02000000][0x34dc:0x351e]);p.writes=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 saved=p.reg[4:12].copy();p.run(entry=0x0800aad0,stop=0x10020000)
 assert p.calls==expected,[hex(t) for t in p.calls]
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 assert p.read(0x02001660,2)==2400 and p.read(0x0200165e,1)==0 and p.read(0x02005f2f,1)==4
 # B6EC owns outer state; only 2277C alone retains it.
 assert before==bytes(p.regions[0x02000000][0x34dc:0x351e])
 assert not any(a<0x0200351e and a+n>0x020034dc for a,n in p.writes)
 rows.append({'seed':seed,'settings':settings,'instructions':p.steps,'ordered_calls':[hex(t) for t in p.calls]})
report={'status':'PASS_PARTIAL_CALLBACK_JOIN','cases':len(rows),'thumb_instructions':sum(x['instructions'] for x in rows),'observations':rows,'rom_sha256':hashlib.sha256(original['r']).hexdigest(),'executed_helpers':[hex(t) for t in sorted(qualified)],'unexecuted_no_state_boundaries':[hex(t) for t in sorted(boundaries)],'scope':'Actual AAD0 prologue/ABC4 dispatcher/return/postcall stores joined on retained RAM with complete B6EC,11490,2277C,21C10 and their original settings-copy/byte-fill helpers. Four listed bootstrap routines remain explicit no-state boundaries. Input preservation proves only this partial join; not complete callback, warm restart or authorization to promote input retention.'}
(root/'reports/bound-restart-callback-partial-join-rom.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],report['cases'],report['thumb_instructions'])
