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


# Execute the complete original strcmp body; only stack writes are permitted.
saved_string_instructions=original['allowed_thumb'].copy()
string_runner=runner.replace("   elif m=='cmp':", "   elif m=='bics':\n    dest=rn(ops[0]);value=self.reg[dest]&~val(ops[1])&MASK;self.reg[dest]=value;self.nz(value)\n   elif m=='cmp':")
exec(string_runner,original);original['allowed_thumb'].update(saved_string_instructions)
string_todo=[0x08033f80];string_seen=set()
while string_todo:
 address=string_todo.pop()
 if address in string_seen:continue
 assert 0x08033f80<=address<0x08033fda,hex(address)
 string_seen.add(address);i=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));original['allowed_thumb'][address]=bytes(i.bytes)
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='b':string_todo.append(int(i.op_str[1:],16));continue
 string_todo.append(address+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):string_todo.append(int(i.op_str[1:],16))
string_calls=[]
def execute_original_string(owner,target):
 assert target==0x08033f80
 args=owner.reg[:2].copy();sp=owner.reg[13];saved=owner.reg[4:12].copy();stores=[]
 def text_at(address):
  value=bytearray()
  for i in range(256):
   c=owner.read(address+i,1)
   if not c:return bytes(value)
   value.append(c)
  raise AssertionError(('unbounded original string fixture',hex(address)))
 left,right=map(text_at,args);expected=0
 for a,b in zip(left+b'\0',right+b'\0'):
  if a!=b:expected=a-b;break
 def write_stack(address,value,size=4):
  assert sp-12<=address and address+size<=sp,('original strcmp nonstack store',hex(address),size)
  stores.append([hex(address),size]);owner.write(address,value,size)
 p=ns['Probe']((0,0,0,0,0,1,0,0,0,0));p.reg=owner.reg;p.read=owner.read;p.write=write_stack;p.run=original['MotionProbe'].run.__get__(p,type(p))
 p.flags=owner.flags
 p.run(entry=target,stop=owner.reg[14]&~1);owner.flags=p.flags
 assert owner.reg[13]==sp and owner.reg[4:12]==saved
 assert original['signed'](owner.reg[0])==expected,(args,expected,original['signed'](owner.reg[0]))
 owner.steps+=p.steps;string_calls.append({'arguments':[hex(a) for a in args],'result':expected,'instructions':p.steps,'stack_stores':stores})


# Original signed division, nonzero divisor domain used by this prefix.
div_todo=[0x08032488];div_seen=set()
while div_todo:
 address=div_todo.pop()
 if address==0x08032510 or address in div_seen:continue
 assert 0x08032488<=address<0x08032510,hex(address)
 div_seen.add(address);i=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));rng_ns['allowed_thumb'][address]=bytes(i.bytes)
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='b':div_todo.append(int(i.op_str[1:],16));continue
 div_todo.append(address+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):div_todo.append(int(i.op_str[1:],16))
division_calls=[]
def execute_original_division(owner,target):
 assert target==0x08032488 and owner.reg[1]!=0
 args=owner.reg[:2].copy();sp=owner.reg[13];saved=owner.reg[4:12].copy();stores=[]
 expected=original['trunc'](original['signed'](args[0]),original['signed'](args[1]))&0xffffffff
 def write_stack(address,value,size=4):
  assert sp-4<=address and address+size<=sp,('original division nonstack store',hex(address),size)
  stores.append([hex(address),size]);owner.write(address,value,size)
 p=SharedRandomProbe((0,0,0,0,0,1,0,0,0,0));p.reg=owner.reg;p.read=owner.read;p.write=write_stack;p.flags=owner.flags
 p.run(entry=target,stop=owner.reg[14]&~1);owner.flags=p.flags
 assert owner.reg[13]==sp and owner.reg[4:12]==saved and owner.reg[0]==expected,(args,expected,owner.reg[0])
 owner.steps+=p.steps;division_calls.append({'arguments':args,'result':original['signed'](owner.reg[0]),'instructions':p.steps,'stack_stores':stores})


# Carry the original ARM quotient register effects into the actual caller.
arm_source=(root/'tools/compare_projection_divide_rom.py').read_text().split('\nvalues=')[0]
arm_source=arm_source.replace('def original(a,b):','def original(registers):').replace("regs={f'r{i}':0 for i in range(16)}", "regs={f'r{i}':registers[i] for i in range(16)}").replace('regs.update(r0=a,r1=b,lr=0x12345678)','regs.update(ip=registers[12],sp=registers[13],lr=registers[14],pc=registers[15])').replace("return regs['r0']",'return regs')
arm_registers={};exec(arm_source,arm_registers)
arm_quotient_calls=[]
def execute_original_arm_quotient(owner):
 target=0x03000008+0x29fc-0x700;assert owner.reg[2]==target
 assert original['r'][0x32454:0x32456]==bytes.fromhex('1047')
 code=original['r'][0x29fc:0x2ce8]
 assert bytes(owner.read(target+i,1) for i in range(len(code)))==code,'relocated quotient body differs from original'
 before=owner.reg.copy();start=arm_registers['steps'];out=arm_registers['original'](before)
 assert [out['r'+str(i)] for i in range(4,12)]==before[4:12] and out['sp']==before[13] and out['lr']==before[14]
 # Every decoded operation is arithmetic/control, with no memory instruction.
 for address,(mnemonic,args) in arm_registers['code'].items():
  stem=mnemonic[:-2] if mnemonic[-2:] in ('mi','eq','ne','hi','hs') else mnemonic
  assert stem in ('bx','b','cmp','tst','ands','eor','rsb','mov','adc','sub'),(hex(address),mnemonic)
 owner.reg[:4]=[out['r'+str(i)] for i in range(4)];owner.reg[12]=out['ip']
 arm_quotient_calls.append({'arguments':before[:2],'scratch_before':before[2:4]+[before[12]],'scratch_after':owner.reg[:4]+[owner.reg[12]],'instructions':arm_registers['steps']-start,'memory_stores':0,'relocated_identity_bytes':len(code)})

rng_calls=[]
class SharedSceneProbe(scene_ns['SceneProbe']):
 def external(self,t):
  if t==0x0802aba0:execute_shared_ground(self,t)
  elif t==0x08032488:execute_original_division(self,t)
  elif t==0x08033f80:execute_original_string(self,t)
  elif t==0x0801e72c:
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
branch_start=actor_external.index('elif t==0x8029b44:')
branch_end=actor_external.index('elif t==0x800d764:',branch_start)
actor_external=actor_external[:branch_start]+'elif t==0x8029b44:self.shared_ground_height(t)\n '+actor_external[branch_end:]
actor_external=actor_external.replace("a,b=ns['ns']['signed'](self.reg[0]),ns['ns']['signed'](self.reg[1]);assert b;self.reg[0]=(a%b)&0xffffffff","self.shared_remainder(t)")
actor_ctx['execute_original_division']=execute_original_division
actor_external=actor_external.replace("a,b=ns['ns']['signed'](self.reg[0]),ns['ns']['signed'](self.reg[1]);assert b;self.reg[0]=ns['ns']['trunc'](a,b)&0xffffffff",'execute_original_division(self,t)')
exec('def external(self,t):'+actor_external,actor_ctx)
actor_ctx['requests']=[]

shared_ground_calls=[]
class SharedGroundWords:
 def __init__(self,owner,stores):self.owner=owner;self.stores=stores
 def get(self,address,default=0):return self.owner.read(address,4)
 def __getitem__(self,address):return self.owner.read(address,4)
 def __setitem__(self,address,value):
  assert not(address<0x0200351e and address+4>0x020034dc),('ground ARM input store',hex(address))
  self.stores.append([hex(address),4]);self.owner.write(address,value,4)
def execute_shared_ground(owner,target):
 args=owner.reg[:4].copy();sp=owner.reg[13];stores=[];calls=[]
 if target==0x0802aba0:p=scene_ns['wn']['WorldProbe']((0,0,0,0,0,0,0,1))
 elif target==0x08029b44:p=actor_ctx['ground']['GroundProbe']((0,0,0,0,0,1),0)
 else:raise AssertionError(('unsupported shared ground entry',hex(target)))
 def write(address,value,size=4):
  assert not(address<0x0200351e and address+size>0x020034dc),('ground Thumb input store',hex(address),size)
  stores.append([hex(address),size]);owner.write(address,value,size)
 p.reg=owner.reg;p.flags=owner.flags;p.read=owner.read;p.write=write;p.arm.mem=SharedGroundWords(owner,stores);p.arm.read=lambda address:owner.read(address,4)
 prior_external=p.external
 def external(callee):
  calls.append({'target':hex(callee),'arguments':p.reg[:4].copy()})
  if callee==0x08032488:execute_original_division(p,callee)
  elif callee==0x08032454 and p.reg[2]==0x03002304:execute_original_arm_quotient(p)
  elif callee in (0x08032454,0x08032458):
   index=2 if callee==0x08032454 else 3;entry=p.reg[index]
   canonical=entry+0x08000700-0x03000008 if 0x03000008<=entry<0x03004964 else entry
   assert canonical in (0x08000b78,0x08000a1c),('unqualified shared ground ARM target',hex(entry),hex(canonical))
   p.arm.reg[13]=p.reg[13];p.arm.run(canonical,p.reg[:4]);p.reg[:4]=p.arm.reg[:4]
  else:prior_external(callee)
 p.external=external
 try:p.run(entry=target,stop=owner.reg[14]&~1)
 except Exception as exc:
  (root/'reports/bound-shared-ground-next-dependency.json').write_text(json.dumps({'status':'SHARED_GROUND_DEPENDENCY','entry':hex(target),'arguments':args,'instructions':p.steps,'calls':calls,'stores':stores,'error':str(exc)},indent=2)+'\n');raise
 assert p.reg[13]==sp
 owner.flags=p.flags;owner.steps+=p.steps
 if target==0x08029b44:actor_ctx['ground_calls'].append({'point':[original['signed'](owner.read(args[0]+off)) for off in (0,4,8)],'height':original['signed'](p.reg[0]),'instructions':p.steps})
 shared_ground_calls.append({'entry':hex(target),'arguments':args,'instructions':p.steps,'calls':calls,'stores':stores,'result':p.reg[0]})


clear_ns={};exec((root/'tools/compare_world_clear_rom.py').read_text().split('\nrects=')[0],clear_ns)
clear_calls=[];passenger_remainder_calls=[]
def original_clear(self,t):
 if t==0x08033f2c:self.fills+=1;self.run(entry=t,stop=self.reg[14]&~1)
 else:raise AssertionError(('unexpectedclearhelper',hex(t)))


hud_ns={};exec((root/'tools/compare_hud_start_owned_rom.py').read_text().split('\ncases=')[0],hud_ns)
group_bases=[int.from_bytes((root/'reports/ordinary-native-assets.bin').read_bytes()[i*8:i*8+4],'little') for i in range(11)]
hud_calls=[]



music_ns={};exec((root/'tools/verify_bound_restart_music_input_rom.py').read_text().split('\nrows=[]')[0],music_ns)
for i in original['md'].disasm(original['r'][0x74f6:0x7522],0x080074f6):original['allowed_thumb'][i.address]=bytes(i.bytes)
music_fragment_calls=[]


background_ns={};exec((root/'tools/verify_bound_background_bootstrap_input_rom.py').read_text().split('\nrows=[]')[0],background_ns)

bg_globals=background_ns['Probe'].run.__globals__;bg_saved=bg_globals['allowed_thumb'].copy()
bg_runner=bg_globals['source'].replace('self.steps<50000','self.steps<4000000').replace('self.steps<200000','self.steps<4000000').replace("'bne':not z","'bhs':c,'blo':not c,'bne':not z")
exec(bg_runner,bg_globals);bg_globals['allowed_thumb'].update(bg_saved);background_ns['Probe'].run=bg_globals['MotionProbe'].run

background_calls=[]


pkg_ctx={};exec((root/'tools/verify_bound_package_joined_object_input_rom.py').read_text().split('\nresults=[]')[0],pkg_ctx)
exec(pkg_ctx['code'].split('\np=TitleAssetProbe')[0],pkg_ctx)
for i in pkg_ctx['md'].disasm(pkg_ctx['r'][0xfec8:0xff18],0x0800fec8):pkg_ctx['allowed_thumb'][i.address]=bytes(i.bytes)
package_calls=[]
display_ns={};exec((root/'tools/compare_display_mode_rom.py').read_text().split('\ncases=')[0],display_ns)
display_calls=[]

qualified={0x0800df8c,0x0800b6ec,0x08011490,0x0802277c,0x08021c10}
boundaries=set()
class Probe(ns['Probe']):
 def __init__(self,*args):self.calls=[];self.car_initializations=[];self.passenger_initializations=[];super().__init__(*args)
 def shared_display(self):
  d=display_ns['DisplayProbe']((0,0,0,0,0,1,0,0,0,0));d.reg=self.reg;d.read=self.read;d.write=self.write;self.reg[:2]=[4,1]
  d.run(entry=0x08025c18,stop=self.reg[14]&~1);self.steps+=d.steps
  display_calls.append({'instructions':d.steps,'front':hex(self.read(0x0200754c)),'back':hex(self.read(0x02007550))})
 def shared_package(self):
  self.run(entry=0x0800f5e8,stop=self.reg[14]&~1)
  a=pkg_ctx['TitleAssetProbe']((0,0,0,0,0,1,0,0,0,0));a.reg=self.reg;a.read=self.read;a.palettes=[];a.images=[];a.transfers=[];a.obj_setup=0
  def package_write(address,value,size=4):
   self.write(address,value,size)
   if address==0x040000dc:
    assert value&0xffff0000==0x84000000
    src,dst=self.read(0x040000d4),self.read(0x040000d8);count=(value&65535)*4
    assert dst+count<=0x020034dc or dst>=0x0200351e
    for i in range(0,count,4):self.write(dst+i,self.read(src+i))
    a.transfers.append({'source':hex(src),'destination':hex(dst),'bytes':count})
  a.write=package_write;a.run(entry=0x080073c0,stop=self.reg[14]&~1)
  self.reg[:2]=[0x08070e50,1];a.run(entry=0x08027994,stop=self.reg[14]&~1)
  for args in [(0,32,0,0x08415bbc,1,0),(1,32,3,0,3,0)]:
   self.reg[:4]=args[:4];self.reg[14]=0x10020001;self.write(self.reg[13],args[4]);self.write(self.reg[13]+4,args[5]);a.run(entry=0x0800fec8,stop=0x10020000)
  self.steps+=a.steps
  assert self.read(0x02001780,2)==16
  assert [self.read(0x02007e3c+4*i) for i in range(11)]==group_bases
  package_calls.append({'instructions':a.steps,'palette_count':self.read(0x02001780,2),'group_bases':[hex(self.read(0x02007e3c+4*i)) for i in range(11)],'transfers':a.transfers,'object_setup_calls':a.obj_setup})
 def shared_background(self):
  b=background_ns['Probe']((0,0,0,0,0,1,0,0,0,0));b.reg=self.reg;b.read=self.read;b.stores=[];b.published_copy=False;b.calls=[];b.contracts=[]
  def background_write(a,v,n=4):
   assert a!=0,('background null destination',hex(self.reg[15]-4),n)
   self.write(a,v,n)
   if a==0x040000dc:
    assert v==0x84001257
    src,dst=self.read(0x040000d4),self.read(0x040000d8);count=(v&65535)*4
    assert (src,dst,count)==(0x08000700,0x03000008,18780)
    for i in range(count):self.write(dst+i,self.read(src+i,1),1)
    b.published_copy=True
  b.write=background_write
  background_external=b.external
  def shared_background_external(target):
   if target==0x08032454:
    b.calls.append((target,b.reg[:4].copy()));execute_original_arm_quotient(b)
   elif target==0x08032488:
    b.calls.append((target,b.reg[:4].copy()));execute_original_division(b,target)
   else:background_external(target)
  b.external=shared_background_external;self.reg[:4]=[0x087ebd98,0,0,240]
  for i,v in enumerate([160,256,0x96<<8]):self.write(self.reg[13]+4*i,v)
  b.run(entry=0x0802ac6c,stop=self.reg[14]&~1);self.steps+=b.steps
  assert not b.contracts and self.read(0x03006810)==0x030069c0
  assert bytes(self.read(0x030069c0+i,1) for i in range(104))==original['r'][0x7ebd98:0x7ebe00]
  background_calls.append({'instructions':b.steps,'calls':[hex(t) for t,args in b.calls],'scene_owner':hex(self.read(0x03006810)),'allocator_cursor':self.read(0x0202c4ec),'frame_pointer':hex(self.read(0x02007550)),'fill_byte':self.read(clear_ns['fill_slot'])&255})
 def shared_remainder(self,t):
  args=self.reg[:2].copy();assert args==[0,5],args
  sp=self.reg[13];saved=self.reg[4:12].copy();writes=[]
  rp=SharedRandomProbe((0,0,0,0,0,1,0,0,0,0));rp.reg=self.reg;rp.read=self.read
  def guarded_write(a,v,n=4):
   assert sp-8<=a and a+n<=sp,(hex(a),n,hex(sp))
   writes.append([hex(a),n]);self.write(a,v,n)
  rp.write=guarded_write;rp.run(entry=t,stop=self.reg[14]&~1);self.steps+=rp.steps
  assert self.reg[0]==0 and self.reg[13]==sp and self.reg[4:12]==saved
  passenger_remainder_calls.append({'args':args,'result':self.reg[0],'instructions':rp.steps,'writes':writes})
 def shared_ground_height(self,t):execute_shared_ground(self,t)
 def shared_clear(self,t):
  c=clear_ns['ClearProbe']((0,0,0,0,0,1,0,0,0,0));c.reg=self.reg;c.read=self.read;c.write=self.write;c.fills=0
  args=self.reg[:4].copy();assert args==[0,0,240,160],args
  c.external=actor_ctx['types'].MethodType(original_clear,c);c.run(entry=t,stop=self.reg[14]&~1);self.steps+=c.steps
  clear_calls.append({'args':args,'fills':c.fills,'instructions':c.steps})
 def external(self,t):
  if t in (0x08027730,0x08026ef8,0x08027110):
   args=self.reg[:2].copy();m=music_ns['Probe']((0,0,0,0,0,1,0,0,0,0));m.reg=self.reg;m.read=self.read;m.write=self.write;m.run(entry=t,stop=self.reg[14]&~1);self.steps+=m.steps
   music_fragment_calls.append({'target':hex(t),'args':args,'instructions':m.steps})
  elif t==0x0801e72c:
   args=self.reg[:2].copy();assert args==[0,65535]
   prior=self.read(0x02003b04);rp=SharedRandomProbe((0,0,0,0,0,1,0,0,0,0));rp.reg=self.reg;rp.read=self.read;rp.write=self.write;rp.run(entry=t,stop=self.reg[14]&~1);self.steps+=rp.steps
   native=subprocess.run([str(root/'bin/random-range-rom-compare.exe')],input=f'{prior} 0 65535\n',text=True,capture_output=True,check=True,timeout=10)
   assert [self.reg[0],self.read(0x02003b04)]==list(map(int,native.stdout.split()))
   music_fragment_calls.append({'target':hex(t),'args':args,'instructions':rp.steps,'value':self.reg[0],'state':self.read(0x02003b04)})
  elif t==0x0800abc4 or t in qualified:
   self.calls.append(t);self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x08014a58:
   self.calls.append(t);g=SharedSceneProbe((0,0,0,0,0,1,0,0,0,0));g.city_address=0x087ebd98;g.point=point
   g.reg=self.reg;g.read=self.read;g.write=self.write;g.run(entry=t,stop=self.reg[14]&~1);self.steps+=g.steps
   actor=bytes(self.read(0x02002634+i,1) for i in range(0xea0))
   if seed==0 and settings==128 and not self.car_initializations:assert actor==captured_after
   self.car_initializations.append({'actor_sha256':hashlib.sha256(actor).hexdigest(),'rng_state':self.read(0x02003b04),'instructions':g.steps})
  elif t==0x080227ac:
   self.calls.append(t);prior_actor=[self.read(33578796+i,1) for i in range(44)];actor_ctx['requests']=[];actor_ctx['ground_calls']=[]
   a=actor_ctx['ns']['Probe']((0,0,0,0,0,1,0,0,0,0));a.reg=self.reg;a.read=self.read;a.write=self.write;a.shared_clear=self.shared_clear;a.shared_remainder=self.shared_remainder
   a.shared_ground_height=lambda target:execute_shared_ground(a,target);a.external=actor_ctx['types'].MethodType(actor_ctx['external'],a);a.run(entry=t,stop=self.reg[14]&~1);self.steps+=a.steps
   if not self.passenger_initializations and settings==128:
    expected_actor=subprocess.run([str(root/'bin/bound-actor-init-rom-compare.exe')],input=f"{seed} {actor_ctx['ground_calls'][0]['height']}\n",text=True,capture_output=True,check=True,timeout=10)
    assert [self.read(33578796+i,1) for i in range(44)]==list(map(int,expected_actor.stdout.split())),dict(prior=prior_actor,got=[self.read(33578796+i,1) for i in range(44)],expected=expected_actor.stdout)
   self.passenger_initializations.append({'actor_bytes':[self.read(33578796+i,1) for i in range(44)],'ground':actor_ctx['ground_calls'].copy(),'requests':actor_ctx['requests'].copy(),'instructions':a.steps})
  elif t==0x0801ee28:
   self.calls.append(t);pre_owned=b''.join(bytes(self.read(address+i,1) for i in range(n)) for address,n in ((0x020016c0,32),(0x02005300,256),(0x03005490,8),(0x02001790,2560),(0x06014000,32768)))
   metadata=bytes(self.read(a,1) for a in (0x02001708,0x02001eb0,0x0200388d,0x02001539,0x02001690))+self.read(0x02001780,2).to_bytes(2,'little')+self.read(0x020094ec,2).to_bytes(2,'little')+b''.join(self.read(a).to_bytes(4,'little') for a in (0x040000d4,0x040000d8,0x040000dc))+b''.join(self.read(0x02007e3c+4*i).to_bytes(4,'little') for i in range(11))
   fixture=root/'reports'/f'whole-prefix-hud-prior-{len(hud_calls):02d}.bin';fixture.write_bytes(metadata+pre_owned)
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
palette_ctx={};exec((root/'tools/verify_bound_restart_palette_chain_input_rom.py').read_text().split('\nrows=[]')[0],palette_ctx)
window_ctx={};exec((root/'tools/verify_bound_restart_window_fade_input_rom.py').read_text().split('\nrows=[]')[0],window_ctx)
terrain_ctx={};exec((root/'tools/verify_bound_terrain_resource_input_rom.py').read_text().split('\nrows=[]')[0],terrain_ctx)
import math
for i in original['md'].disasm(original['r'][0x743c:0x7538],0x0800743c):original['allowed_thumb'][i.address]=bytes(i.bytes)
original['allowed_thumb'][0x0803244c]=original['r'][0x3244c:0x3244e]
roots=[0x0800f5e8,0x08025c18,0x08007378,0x080073c0,0x08027994,0x0800fec8,0x0800e154,0x0800e5a8,0x0802ac6c,0x0802c834,0x0802c688,0x080203c4,0x0803244c,0x08027730,0x08026ef8,0x0801e72c,0x08027110,0x08026440]
class PrefixProbe(Probe):
 def __init__(self,*args):self.prefix_calls=[];super().__init__(*args)
 def dma_write(self,a,v,n=4):
  self.write(a,v,n)
  if a==0x040000dc:
   assert v&0xffff0000==0x84000000
   src,dst=self.read(0x040000d4),self.read(0x040000d8);count=(v&65535)*4
   assert dst+count<=0x020034dc or dst>=0x0200351e
   data=bytes(self.read(src+i,1) for i in range(count))
   for i,b in enumerate(data):self.write(dst+i,b,1)
 def external(self,t):
  if t in roots:self.prefix_calls.append({'target':hex(t),'args':self.reg[:4].copy(),'stack_args':[self.read(self.reg[13]+4*i) for i in range(3)]})
  if t in (0x0800f5e8,0x08007378,0x080073c0,0x08026440,0x0803244c):self.run(entry=t,stop=self.reg[14]&~1)
  elif t==0x08025c18:
   assert self.reg[:2]==[4,1];d=display_ns['DisplayProbe']((0,0,0,0,0,1,0,0,0,0));d.reg=self.reg;d.read=self.read;d.write=self.write;d.run(entry=t,stop=self.reg[14]&~1);self.steps+=d.steps
  elif t in (0x08027994,0x0800fec8,0x0800e154):
   a=pkg_ctx['TitleAssetProbe']((0,0,0,0,0,1,0,0,0,0));a.reg=self.reg;a.read=self.read;a.write=self.dma_write;a.palettes=[];a.images=[];a.transfers=[];a.obj_setup=0;a.run(entry=t,stop=self.reg[14]&~1);self.steps+=a.steps
  elif t==0x0800e5a8:
   a=palette_ctx['ns']['Probe']((0,0,0,0,0,1,0,0,0,0));a.reg=self.reg;a.read=self.read;a.write=self.dma_write;a.dma_copies=[];a.run(entry=t,stop=self.reg[14]&~1);self.steps+=a.steps
  elif t==0x0802ac6c:
   assert self.reg[:4]==[0x087ebd98,0,0,240]
   assert [self.read(self.reg[13]+4*i) for i in range(3)]==[160,256,38400]
   self.shared_background()
  elif t in (0x0802c834,0x0802c688):
   a=window_ctx['Probe']((0,0,0,0,0,1,0,0,0,0));a.reg=self.reg;a.read=self.read;a.write=self.write;a.run(entry=t,stop=self.reg[14]&~1);self.steps+=a.steps
  elif t==0x080203c4:
   a=terrain_ctx['ns']['Probe']((0,0,0,0,0,1,0,0,0,0));a.reg=self.reg;a.read=self.read;a.write=self.write
   def terrain_external(g,target):
    if target==0x08033f80:execute_original_string(g,target)
    elif target==0x08032038:g.reg[0]=math.isqrt(g.reg[0])
    elif target==0x08032488:execute_original_division(g,target)
    else:raise AssertionError(('unqualifiedterrainhelper',hex(target)))
   a.external=actor_ctx['types'].MethodType(terrain_external,a);a.run(entry=t,stop=self.reg[14]&~1);self.steps+=a.steps
  else:super().external(t)
rows=[]
for seed,settings in [(0,128),(165,128),(255,128)]:
 p=PrefixProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x02000000:bytearray(262144),0x03000000:bytearray(65536),0x05000000:bytearray(1024),0x06000000:bytearray(131072),0x07000000:bytearray(1024)};p.scalars={}
 p.regions[0x0600a000]=memoryview(p.regions[0x06000000])[0xa000:0xa000+38416];p.regions[0x06014000]=memoryview(p.regions[0x06000000])[0x14000:0x1c000]
 p.write(0x03004c9c,0x084a4ff4);p.write(0x03004ca0,0x02002634);p.write(0x03004c40,0x10030001);p.write(0x02001770,4,1);p.write(0x02007600,1,1)
 p.regions[0x02000000][0x2634:0x34d4]=bytes.fromhex(capture[9])
 for i in range(44):p.write(33578796+i,(seed+i*37)&255,1)
 for i in range(66):p.write(0x020034dc+i,(seed+i*13)&255,1)
 p.write(0x02001eb0,settings,1);p.write(0x02001537,5,1);p.write(0x02003b04,seed)
 before=bytes(p.regions[0x02000000][0x34dc:0x351e]);p.reg[13]=0x10000400;p.reg[10]=1;p.write(0x10000444,1)
 for invocation in range(2):
  p.prefix_calls=[];p.writes=[];start=p.steps;p.run(entry=0x0800743c,stop=0x08007538)
  assert [q['target'] for q in p.prefix_calls]==[hex(t) for t in roots]
  assert before==bytes(p.regions[0x02000000][0x34dc:0x351e])
  assert not any(a<0x0200351e and a+n>0x020034dc for a,n in p.writes)
  assert p.reg[13]==0x10000400 and p.reg[10]==1
  assert p.read(0x020034d4)==p.read(0x020034d8)==1
  assert [p.read(a,1) for a in (0x02001691,0x0200153c,0x0200153d,0x0200153a)]==[0]*4
  rows.append({'seed':seed,'settings':settings,'invocation':invocation,'instructions':p.steps-start,'prefix_calls':p.prefix_calls,'palette_count':p.read(0x02001780,2),'front':hex(p.read(0x0200754c)),'back':hex(p.read(0x02007550))})
report={'status':'PASS_ACTUAL_743C_SHARED_GROUND_JOIN','cases':len(rows),'observations':rows,'instructions':sum(q['instructions'] for q in rows),'shared_ground_calls':shared_ground_calls,'original_arm_quotient_calls':arm_quotient_calls,'original_division_calls':division_calls,'original_string_calls':string_calls,'hud_calls':hud_calls,'rng_calls':rng_calls,'passenger_remainder_calls':passenger_remainder_calls,'rom_sha256':hashlib.sha256(original['r']).hexdigest(),'scope':'Actual743C..7536 caller and18directhelpers plusactual3244C bx-r0/AAD0/ABC4 callback execute onsharedRAM, twice withoutreseeding. One capturedcarpreowner/settings128 andthreeinputsentinels0/165/255; originalmode4display/package/font/palettefade/background/window/terrain/musicbody ordering. SourceactualoneFEC8 call andfar38400, supersedingassembledtwofont/far24576fixtures. LogicalDMA/installedcopyidentity/callerSL1savedframe andpaletteflags4/musicgate1 explicitfixtures. Fulloriginal33F80 string comparison and confined12byte stackwrites joined onsharedRAM; Original nonzero32488 signed division joined forscene/background/passenger/terrain with4byte stack guard; Original relocated029FC quotient identity andfullr0-r3/r12 registeroutputs replacepoisoned scratch contract; arithmetic-only body hasno memory operations. ARMflags/interworking timing remaincontract boundaries; sqrt BIOS andground containment/ARM flags contracts remain. Original29B44 and2ABA0 bodies now use shared caller RAM; all observed ground stores are guarded. Ground ARM callbacks retain the existing bounded arithmetic runner; full callback ABI is not yet qualified. Zero divisors excluded. Notactualcapturedwarmgameplaymemory/fullrestartbeyond7538/runtimeinputpromotion.'}
(root/'reports/bound-restart-shared-ground-join-rom.json').write_text(json.dumps(report,indent=2)+'\n');print(report['status'],report['cases'],report['instructions'])
