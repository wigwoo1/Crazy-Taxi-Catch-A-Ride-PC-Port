from pathlib import Path
import json,hashlib,itertools
root=Path('outputs/crazy-taxi-native')
ns={};exec((root/'tools/verify_bound_restart_session_input_rom.py').read_text().split('\nrows=[]')[0],ns)
original=ns['ns']
block_ns={};exec((root/'tools/compare_restart_block_reset_rom.py').read_text().split('\nseeds=')[0],block_ns)
original['allowed_thumb'].update(block_ns['allowed_thumb'])
saved_instructions=original['allowed_thumb'].copy()
runner=original['source'].replace("   elif m=='pop':", "   elif m=='stm':\n    base=rn(ops[0]);address=self.reg[base];registers=sorted(rn(o) for o in ops[1:])\n    for j,k in enumerate(registers):self.write(address+4*j,self.reg[k])\n    self.reg[base]=(address+4*len(registers))&MASK\n   elif m=='pop':")
runner=runner.replace("'bhi':c and not z","'bhs':c,'blo':not c,'bhi':c and not z")
runner=runner.replace('    self.reg[13]+=4*len(regs)','    self.reg[13]+=4*len(regs)\n    if 15 in regs:nextpc=self.reg[15]&~1')
exec(runner,original);original['allowed_thumb'].update(saved_instructions)
ns['Probe'].run=original['MotionProbe'].run

for lo,hi in [(0xaad0,0xaaf0),(0xabc4,0xabec)]:
 for i in original['md'].disasm(original['r'][lo:hi],0x08000000+lo): original['allowed_thumb'][i.address]=bytes(i.bytes)

scene_ns={};exec((root/'tools/compare_vehicle_start_scene_original_markers_rom.py').read_text().split('steps=0;total=0;points=[]')[0],scene_ns)
capture=(root/'reports/bound-whole-startup-preview-owner.txt').read_text().splitlines()[0].split()
x,z,heading=map(int,capture[2:5]);captured_after=bytes.fromhex(capture[10])
point=[x,z,int.from_bytes(captured_after[4:8],'little',signed=True)-64,heading,captured_after[0x374],captured_after[0x375],captured_after[0x376],int(capture[8])]
class SharedSceneProbe(scene_ns['SceneProbe']):
 def external(self,t):
  if t==0x0801e72c:
   assert self.reg[:2]==[20,60]
   state=self.read(0x02003b04) or 0x4652414e;state=(state<<1)&0xffffffff
   if state&0x80000000:state^=0x1d872b41
   state=((state&0x7fffffff)^0x06000000)&0x7fffffff
   self.write(0x02003b04,state);self.reg[0]=20+state%41
  else:super().external(t)

qualified={0x0800df8c,0x0800b6ec,0x08011490,0x0802277c,0x08021c10}
boundaries={0x080227ac,0x0801ee28}
class Probe(ns['Probe']):
 def __init__(self,*args):self.calls=[];self.car_initializations=[];super().__init__(*args)
 def external(self,t):
  if t==0x0800abc4 or t in qualified:
   self.calls.append(t);self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x08014a58:
   self.calls.append(t);g=SharedSceneProbe((0,0,0,0,0,1,0,0,0,0));g.city_address=0x087ebd98;g.point=point
   g.reg=self.reg;g.read=self.read;g.write=self.write;g.run(entry=t,stop=self.reg[14]&~1);self.steps+=g.steps
   actor=bytes(self.read(0x02002634+i,1) for i in range(0xea0))
   if seed==0 and settings==128 and not self.car_initializations:assert actor==captured_after
   self.car_initializations.append({'actor_sha256':hashlib.sha256(actor).hexdigest(),'rng_state':self.read(0x02003b04),'instructions':g.steps})
  elif t in boundaries:self.calls.append(t)
  elif t in (0x0800df9c,0x08033f2c):self.run(entry=t,stop=self.reg[14]&~1)
  else:super().external(t)
rows=[]
expected=[0x0800abc4,0x08014a58,0x0800b6ec,0x0800df8c,0x080227ac,0x08011490,0x0801ee28,0x0802277c,0x08021c10]
for seed,settings in itertools.product([0,165,255],[128,129,159,255]):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x02000000:bytearray(262144),0x03000000:bytearray(65536)};p.scalars={}
 p.regions[0x02000000][0x2634:0x34d4]=bytes.fromhex(capture[9])
 p.write(0x03006810,0x087ebd98);p.write(0x03004ca0,0x02002634);p.write(0x020034d4,1);p.write(0x020034d8,1);p.write(0x02003b04,seed)
 for off in range(66):p.write(0x020034dc+off,(seed+off*13)&255,1)
 p.write(0x02001eb0,settings,1);p.write(0x02001537,5,1);p.write(0x02007542,240,2);p.write(0x02001ea8,seed,1)
 for off in range(528):p.write(0x0202b8ec+off,(seed+off*37)&255,1)
 p.write(0x02001768,seed);p.write(0x0200176c,seed^0xabcdef01)
 block_tail=bytes(p.regions[0x02000000][0x2baec:0x2bafc])
 before=bytes(p.regions[0x02000000][0x34dc:0x351e]);p.writes=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 saved=p.reg[4:12].copy();p.run(entry=0x0800aad0,stop=0x10020000)
 assert p.calls==expected,[hex(t) for t in p.calls]
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 assert p.read(0x02001660,2)==2400 and p.read(0x0200165e,1)==0 and p.read(0x02005f2f,1)==4
 assert p.read(0x02001768)==0xffffffff and p.read(0x0200176c)==0
 assert bytes(p.regions[0x02000000][0x2b8ec:0x2baec])==bytes(512)
 assert bytes(p.regions[0x02000000][0x2baec:0x2bafc])==block_tail
 # B6EC owns outer state; only 2277C alone retains it.
 assert before==bytes(p.regions[0x02000000][0x34dc:0x351e])
 assert not any(a<0x0200351e and a+n>0x020034dc for a,n in p.writes)
 p.calls=[];p.writes=[]
 p.reg[14]=0x10020001;p.run(entry=0x0800aad0,stop=0x10020000)
 assert p.calls==expected and p.reg[13]==0x10000400 and p.reg[4:12]==saved
 assert p.read(0x02001660,2)==2400 and p.read(0x0200165e,1)==0 and p.read(0x02005f2f,1)==4
 assert p.read(0x02001768)==0xffffffff and p.read(0x0200176c)==0
 assert bytes(p.regions[0x02000000][0x2b8ec:0x2baec])==bytes(512)
 assert bytes(p.regions[0x02000000][0x2baec:0x2bafc])==block_tail
 assert before==bytes(p.regions[0x02000000][0x34dc:0x351e])
 assert not any(a<0x0200351e and a+n>0x020034dc for a,n in p.writes)
 rows.append({'seed':seed,'settings':settings,'instructions':p.steps,'ordered_calls':[hex(t) for t in p.calls],'car_initializations':p.car_initializations})
report={'status':'PASS_CALLBACK_CAR_JOIN','cases':len(rows),'callback_invocations':2*len(rows),'thumb_instructions':sum(x['instructions'] for x in rows),'observations':rows,'rom_sha256':hashlib.sha256(original['r']).hexdigest(),'executed_helpers':[hex(t) for t in sorted(qualified|{0x08014a58})],'unexecuted_no_state_boundaries':[hex(t) for t in sorted(boundaries)],'scope':'Actual AAD0 prologue/ABC4 dispatcher/return/postcall stores joined on retained RAM with complete DF8C/DF9C(-1)/33F2C 512-byte fill and B6EC,11490,2277C,21C10 and their original settings-copy/byte-fill helpers. Two successive callback invocations retain the same memory without reseeding. Two listed bootstrap routines remain explicit no-state boundaries. Car14A58 and its original template/reset/fills execute against the same memory and registers. Original ground query runs in separate bounded memory with returned hit bytes copied through guarded shared stores; string/arithmetic/containment and RNG state/value remain explicit contracts. First seed0/settings128 car result also matches all3744bytes of captured native result; later results recorded with hashes, not native comparisons. One captured car pre-owner used with twelve input/settings fixtures. Input preservation proves only this partial join; not complete callback, warm restart or authorization to promote input retention.'}
(root/'reports/bound-restart-callback-car-join-rom.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],report['cases'],report['thumb_instructions'])
