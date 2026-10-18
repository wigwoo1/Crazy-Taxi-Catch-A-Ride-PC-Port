from pathlib import Path
import json,hashlib,itertools,subprocess
root=Path('outputs/crazy-taxi-native')
ns={};exec((root/'tools/verify_bound_restart_session_input_rom.py').read_text().split('\nrows=[]')[0],ns)
original=ns['ns']
block_ns={};exec((root/'tools/compare_restart_block_reset_rom.py').read_text().split('\nseeds=')[0],block_ns)
original['allowed_thumb'].update(block_ns['allowed_thumb'])
saved_instructions=original['allowed_thumb'].copy()
runner=original['source'].replace("   elif m=='pop':", "   elif m=='stm':\n    base=rn(ops[0]);address=self.reg[base];registers=sorted(rn(o) for o in ops[1:])\n    for j,k in enumerate(registers):self.write(address+4*j,self.reg[k])\n    self.reg[base]=(address+4*len(registers))&MASK\n   elif m=='pop':")
runner=runner.replace("'bhi':c and not z","'bhs':c,'blo':not c,'bhi':c and not z")
runner=runner.replace('    self.reg[13]+=4*len(regs)','    self.reg[13]+=4*len(regs)\n    if 15 in regs:nextpc=self.reg[15]&~1')
runner=runner.replace('self.steps<50000','self.steps<2000000')
exec(runner,original);original['allowed_thumb'].update(saved_instructions)
ns['Probe'].run=original['MotionProbe'].run

for lo,hi in [(0xaad0,0xaaf0),(0xabc4,0xabec)]:
 for i in original['md'].disasm(original['r'][lo:hi],0x08000000+lo): original['allowed_thumb'][i.address]=bytes(i.bytes)

scene_ns={};exec((root/'tools/compare_vehicle_start_scene_original_markers_rom.py').read_text().split('steps=0;total=0;points=[]')[0],scene_ns)
capture=(root/'reports/bound-whole-startup-preview-owner.txt').read_text().splitlines()[0].split()
x,z,heading=map(int,capture[2:5]);captured_after=bytes.fromhex(capture[10])
point=[x,z,int.from_bytes(captured_after[4:8],'little',signed=True)-64,heading,captured_after[0x374],captured_after[0x375],captured_after[0x376],int(capture[8])]
rng_ns={};exec((root/'tools/compare_random_range_rom.py').read_text().split('\ncases=')[0],rng_ns)

saved_rng=rng_ns['allowed_thumb'].copy()
rng_runner=rng_ns['source'].replace("    if m=='movs':self.nz(value)","    if m=='movs':self.nz(value)\n    if rn(ops[0])==15:nextpc=value&~1")
rng_runner=rng_runner.replace("   elif m=='cmp':", "   elif m=='rors':\n    dest=rn(ops[0]);a=self.reg[dest];n=val(ops[1])&31;value=((a>>n)|(a<<(32-n)))&MASK if n else a;self.reg[dest]=value;self.nz(value)\n   elif m=='tst':self.nz(val(ops[0])&val(ops[1]))\n   elif m=='cmp':")
rng_runner=rng_runner.replace("'bhi':c and not z","'bhs':c,'blo':not c,'bpl':not n,'bhi':c and not z")
exec(rng_runner,rng_ns);rng_ns['allowed_thumb'].update(saved_rng)
for i in rng_ns['md'].disasm(rng_ns['r'][0x32520:0x325e4],0x08032520):rng_ns['allowed_thumb'][i.address]=bytes(i.bytes)
class SharedRandomProbe(rng_ns['RandomProbe']):
 run=rng_ns['MotionProbe'].run
 def external(self,t):
  assert t==0x08032520 and self.reg[1]!=0
  self.run(entry=t,stop=self.reg[14]&~1)

rng_calls=[]
class SharedSceneProbe(scene_ns['SceneProbe']):
 def external(self,t):
  if t==0x0801e72c:
   assert self.reg[:2]==[20,60]
   prior_state=self.read(0x02003b04)
   rp=SharedRandomProbe((0,0,0,0,0,1,0,0,0,0));rp.reg=self.reg;rp.read=self.read;rp.write=self.write
   rp.run(entry=t,stop=self.reg[14]&~1);self.steps+=rp.steps
   native=subprocess.run([str(root/'bin/random-range-rom-compare.exe')],input=f'{prior_state} 20 60\n',text=True,capture_output=True,check=True,timeout=10)
   assert [self.reg[0],self.read(0x02003b04)]==list(map(int,native.stdout.split()))
   rng_calls.append({'prior_state':prior_state,'state':self.read(0x02003b04),'value':self.reg[0],'instructions':rp.steps})
  else:super().external(t)


actor_source=(root/'tools/probe_bound_actor_init_dependencies.py').read_text()
actor_ctx={};exec(actor_source.split("p=ns['Probe']",1)[0],actor_ctx)
actor_external=actor_source.split('def external(self,t):',1)[1].split('p.external=',1)[0]
actor_external=actor_external.replace('elif t==0x802c9f0:return','elif t==0x802c9f0:self.shared_clear(t)')
exec('def external(self,t):'+actor_external,actor_ctx)
actor_ctx['requests']=[]

clear_ns={};exec((root/'tools/compare_world_clear_rom.py').read_text().split('\nrects=')[0],clear_ns)
clear_calls=[]
def original_clear(self,t):
 if t==0x08033f2c:self.fills+=1;self.run(entry=t,stop=self.reg[14]&~1)
 else:raise AssertionError(('unexpectedclearhelper',hex(t)))


hud_ns={};exec((root/'tools/compare_hud_start_owned_rom.py').read_text().split('\ncases=')[0],hud_ns)
group_bases=[int.from_bytes((root/'reports/ordinary-native-assets.bin').read_bytes()[i*8:i*8+4],'little') for i in range(11)]
hud_calls=[]


qualified={0x0800df8c,0x0800b6ec,0x08011490,0x0802277c,0x08021c10}
boundaries=set()
class Probe(ns['Probe']):
 def __init__(self,*args):self.calls=[];self.car_initializations=[];self.passenger_initializations=[];super().__init__(*args)
 def shared_clear(self,t):
  c=clear_ns['ClearProbe']((0,0,0,0,0,1,0,0,0,0));c.reg=self.reg;c.read=self.read;c.write=self.write;c.fills=0
  args=self.reg[:4].copy();assert args==[0,0,240,160],args
  c.external=actor_ctx['types'].MethodType(original_clear,c);c.run(entry=t,stop=self.reg[14]&~1);self.steps+=c.steps
  clear_calls.append({'args':args,'fills':c.fills,'instructions':c.steps})
 def external(self,t):
  if t==0x0800abc4 or t in qualified:
   self.calls.append(t);self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x08014a58:
   self.calls.append(t);g=SharedSceneProbe((0,0,0,0,0,1,0,0,0,0));g.city_address=0x087ebd98;g.point=point
   g.reg=self.reg;g.read=self.read;g.write=self.write;g.run(entry=t,stop=self.reg[14]&~1);self.steps+=g.steps
   actor=bytes(self.read(0x02002634+i,1) for i in range(0xea0))
   if seed==0 and settings==128 and not self.car_initializations:assert actor==captured_after
   self.car_initializations.append({'actor_sha256':hashlib.sha256(actor).hexdigest(),'rng_state':self.read(0x02003b04),'instructions':g.steps})
  elif t==0x080227ac:
   self.calls.append(t);prior_actor=[self.read(33578796+i,1) for i in range(44)];actor_ctx['requests']=[];actor_ctx['ground_calls']=[]
   a=actor_ctx['ns']['Probe']((0,0,0,0,0,1,0,0,0,0));a.reg=self.reg;a.read=self.read;a.write=self.write;a.shared_clear=self.shared_clear
   a.external=actor_ctx['types'].MethodType(actor_ctx['external'],a);a.run(entry=t,stop=self.reg[14]&~1);self.steps+=a.steps
   if not self.passenger_initializations and settings==128:
    expected_actor=subprocess.run([str(root/'bin/bound-actor-init-rom-compare.exe')],input=f"{seed} {actor_ctx['ground_calls'][0]['height']}\n",text=True,capture_output=True,check=True,timeout=10)
    assert [self.read(33578796+i,1) for i in range(44)]==list(map(int,expected_actor.stdout.split())),dict(prior=prior_actor,got=[self.read(33578796+i,1) for i in range(44)],expected=expected_actor.stdout)
   self.passenger_initializations.append({'actor_bytes':[self.read(33578796+i,1) for i in range(44)],'ground':actor_ctx['ground_calls'].copy(),'requests':actor_ctx['requests'].copy(),'instructions':a.steps})
  elif t==0x0801ee28:
   self.calls.append(t);pre_owned=b''.join(bytes(self.read(address+i,1) for i in range(n)) for address,n in ((0x020016c0,32),(0x02005300,256),(0x03005490,8),(0x02001790,2560),(0x06014000,32768)))
   metadata=bytes(self.read(a,1) for a in (0x02001708,0x02001eb0,0x0200388d,0x02001539,0x02001690))+self.read(0x02001780,2).to_bytes(2,'little')+self.read(0x020094ec,2).to_bytes(2,'little')+b''.join(self.read(a).to_bytes(4,'little') for a in (0x040000d4,0x040000d8,0x040000dc))+b''.join(self.read(0x02007e3c+4*i).to_bytes(4,'little') for i in range(11))
   fixture=root/'reports'/f'joined-hud-retained-prior-{len(hud_calls):02d}.bin';fixture.write_bytes(metadata+pre_owned)
   expected_native=subprocess.run([str(root/'bin/hud-start-retained-owner-compare.exe'),str(fixture)],capture_output=True,check=True,timeout=10).stdout
   h=hud_ns['Probe']((0,0,0,0,0,1,0,0,0,0));h.reg=self.reg;h.read=self.read;h.requests=[]
   def hud_write(address,value,size=4):
    self.write(address,value,size)
    if address==0x040000dc:
     assert value>>16==0x8400
     src,dst=self.read(0x040000d4),self.read(0x040000d8);count=(value&65535)*4
     assert dst+count<=0x020034dc or dst>=0x0200351e
     data=bytes(self.read(src+i,1) for i in range(count))
     for i,b in enumerate(data):self.write(dst+i,b,1)
   h.write=hud_write;h.run(entry=t,stop=self.reg[14]&~1);self.steps+=h.steps
   owned=b''.join(bytes(self.read(address+i,1) for i in range(n)) for address,n in ((0x020016c0,32),(0x02005300,256),(0x03005490,8),(0x02001790,2560),(0x06014000,32768)))
   post=owned+self.read(0x020094ec,2).to_bytes(2,'little')+b''.join(self.read(a).to_bytes(4,'little') for a in (0x040000d4,0x040000d8,0x040000dc))
   assert post==expected_native,[(i,a,b) for i,(a,b) in enumerate(zip(post,expected_native)) if a!=b][:10]
   hud_calls.append({'owned_sha256':hashlib.sha256(owned).hexdigest(),'instructions':h.steps,'requests':h.requests,'native_bytes_compared':len(post),'prior_fixture':str(fixture),'prior_sha256':hashlib.sha256(fixture.read_bytes()).hexdigest()})
  elif t in boundaries:self.calls.append(t)
  elif t in (0x0800df9c,0x08033f2c):self.run(entry=t,stop=self.reg[14]&~1)
  else:super().external(t)
rows=[]
expected=[0x0800abc4,0x08014a58,0x0800b6ec,0x0800df8c,0x080227ac,0x08011490,0x0801ee28,0x0802277c,0x08021c10]
for seed,settings in itertools.product([0,165,255],[128,129,159]):
 p=Probe((0,0,0,0,0,1,0,0,0,0));p.regions={0x02000000:bytearray(262144),0x03000000:bytearray(65536)};p.scalars={};p.regions[0x0600a000]=bytearray([165])*38416
 p.regions[0x06014000]=bytearray(32768);p.write(0x02007e38,0x08070e50);p.write(0x020094ec,0,2);p.write(0x02001780,16,2)
 for i,base in enumerate(group_bases):p.write(0x02007e3c+i*4,base)
 p.write(clear_ns['frame_slot'],0x0600a000);p.write(clear_ns['fill_slot'],seed);p.write(0x02007544,160,2)
 p.regions[0x02000000][0x2634:0x34d4]=bytes.fromhex(capture[9])
 for i in range(44):p.write(33578796+i,(seed+i*37)&255,1)
 p.write(0x03006810,0x087ebd98);p.write(0x03004ca0,0x02002634);p.write(0x020034d4,1);p.write(0x020034d8,1);p.write(0x02003b04,seed)
 for off in range(66):p.write(0x020034dc+off,(seed+off*13)&255,1)
 p.write(0x02001eb0,settings,1);p.write(0x02001537,5,1);p.write(0x02007542,240,2);p.write(0x02001ea8,seed,1)
 for off in range(528):p.write(0x0202b8ec+off,(seed+off*37)&255,1)
 p.write(0x02001768,seed);p.write(0x0200176c,seed^0xabcdef01)
 block_tail=bytes(p.regions[0x02000000][0x2baec:0x2bafc])
 before=bytes(p.regions[0x02000000][0x34dc:0x351e]);p.writes=[];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 saved=p.reg[4:12].copy();p.run(entry=0x0800aad0,stop=0x10020000)
 assert bytes(p.regions[0x0600a000][:38400])==bytes([seed])*38400
 assert p.regions[0x0600a000][38400:]==bytes([165])*16
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
 assert bytes(p.regions[0x0600a000][:38400])==bytes([seed])*38400
 assert p.regions[0x0600a000][38400:]==bytes([165])*16
 assert p.calls==expected and p.reg[13]==0x10000400 and p.reg[4:12]==saved
 assert p.read(0x02001660,2)==2400 and p.read(0x0200165e,1)==0 and p.read(0x02005f2f,1)==4
 assert p.read(0x02001768)==0xffffffff and p.read(0x0200176c)==0
 assert bytes(p.regions[0x02000000][0x2b8ec:0x2baec])==bytes(512)
 assert bytes(p.regions[0x02000000][0x2baec:0x2bafc])==block_tail
 assert before==bytes(p.regions[0x02000000][0x34dc:0x351e])
 assert not any(a<0x0200351e and a+n>0x020034dc for a,n in p.writes)
 rows.append({'seed':seed,'settings':settings,'instructions':p.steps,'ordered_calls':[hex(t) for t in p.calls],'car_initializations':p.car_initializations,'passenger_initializations':p.passenger_initializations})
report={'status':'PASS_CALLBACK_ORIGINAL_RNG_REMAINDER_JOIN','cases':len(rows),'callback_invocations':2*len(rows),'thumb_instructions':sum(x['instructions'] for x in rows),'observations':rows,'display_clear_calls':clear_calls,'hud_calls':hud_calls,'rng_calls':rng_calls,'rom_sha256':hashlib.sha256(original['r']).hexdigest(),'executed_helpers':[hex(t) for t in sorted(qualified|{0x08014a58,0x080227ac,0x0801ee28})],'unexecuted_no_state_boundaries':[hex(t) for t in sorted(boundaries)],'scope':'Actual AAD0 prologue/ABC4 dispatcher/return/postcall stores joined on retained RAM with complete DF8C/DF9C(-1)/33F2C 512-byte fill and B6EC,11490,2277C,21C10 and their original settings-copy/byte-fill helpers. Two successive callback invocations retain the same memory without reseeding. All eight ABC4 top-level helpers now execute; 1EE28/27FB8/1E784 use same memory with real package/group bases and guarded logical DMA. All eighteen HUD invocations compare35624ownedbytes and14queue/DMAbytes against compilednative initializer using exact capturedpriorowners/input/groupbases. Explicitpalettecount16fixture supplied; fixturecomparison doesnotrecover livepalettecount/allocator. Asset allocator/group-base prior owners remain explicit fixtures, not full743C proof. Car14A58 and its original template/reset/fills execute against the same memory and registers. Original ground query runs in separate bounded memory with returned hit bytes copied through guarded shared stores; string/arithmetic/containment remain explicit contracts. Wholeoriginal1E72C executes on sharedRAM with guardedstate/stack writes; all18RNG state/value results compared to compilednative,Actual32520signedremainder nonzero-divisor body also executes on sharedRAM; RNG path divisor41, originalstack stores guarded. Zero-divisor3251C branch excluded, other bootstrap division callers not qualified. First seed0/settings128 car result also matches all3744bytes of captured native result; later results recorded with hashes, not native comparisons. One captured car pre-owner used with nine input/settings fixtures. Passenger227AC and D764 challenge branch execute on shared memory;29B44 ground height uses separate original bounded probe, modulo/division contracts remain explicit. Actual2C9F0 and33F2C now execute on shared framebuffer fixture0600A000 with160whole row fills per call; all38400bytes and16adjacentbytes checked. Framebufferpointer/fill owner explicitly supplied, not recovered warm framebuffer ownership. First passenger initialization settings128 also matches all44 native actor bytes for explicit prior-record pattern(seed+i*37); other passenger results recorded, not independently compared. Input preservation proves only this partial join; not complete callback, warm restart or authorization to promote input retention.'}
(root/'reports/bound-restart-callback-original-rng-remainder-join-rom.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],report['cases'],report['thumb_instructions'])
