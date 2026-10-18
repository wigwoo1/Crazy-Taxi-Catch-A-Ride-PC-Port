from pathlib import Path
import json,hashlib,itertools,subprocess
root=Path('outputs/crazy-taxi-native')
ns={};exec((root/'tools/verify_bound_restart_session_input_rom.py').read_text().split('\nrows=[]')[0],ns)
original=ns['ns']
block_ns={};exec((root/'tools/compare_restart_block_reset_rom.py').read_text().split('\nseeds=')[0],block_ns)
original['allowed_thumb'].update(block_ns['allowed_thumb'])
saved_instructions=original['allowed_thumb'].copy()
runner=original['source'].replace("   elif m=='pop':", "   elif m=='stm':\n    base=rn(ops[0]);address=self.reg[base];registers=sorted(rn(o) for o in ops[1:])\n    for j,k in enumerate(registers):self.write(address+4*j,self.reg[k])\n    self.reg[base]=(address+4*len(registers))&MASK\n   elif m=='pop':")
runner=runner.replace("   elif m=='pop':", "   elif m=='ldm':\n    base=rn(ops[0]);address=self.reg[base];registers=sorted(rn(o) for o in ops[1:])\n    assert base<8 and registers and all(k<8 for k in registers)\n    values=[self.read(address+4*j) for j in range(len(registers))]\n    for k,value in zip(registers,values):self.reg[k]=value\n    if base not in registers:self.reg[base]=(address+4*len(registers))&MASK\n   elif m=='pop':")
runner=runner.replace("'bhi':c and not z","'bhs':c,'blo':not c,'bhi':c and not z")
runner=runner.replace('    self.reg[13]+=4*len(regs)','    self.reg[13]+=4*len(regs)\n    if 15 in regs:nextpc=self.reg[15]&~1')
runner=runner.replace('self.steps<50000','self.steps<2000000')

bl_before="elif m=='bl':self.reg[14]=nextpc;self.external(val(ops[0]))"
assert bl_before in runner
runner=runner.replace(bl_before,"elif m=='bl':"+chr(10)+"    self.reg[14]=nextpc"+chr(10)+"    if val(ops[0])==0x0800c524:nextpc=0x0800c524"+chr(10)+"    else:self.external(val(ops[0]))")

runner=runner.replace("    if m=='movs':self.nz(value)","    if m=='movs':self.nz(value)\n    if rn(ops[0])==15:nextpc=value&~1")
runner=runner.replace("elif m=='cmp':self.arithmetic(val(ops[0]),val(ops[1]),True)","elif m in ['cmp','cmn']:self.arithmetic(val(ops[0]),val(ops[1]),m=='cmp')")
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
string_runner=runner.replace("   elif m in ['cmp','cmn']:", "   elif m=='bics':\n    dest=rn(ops[0]);value=self.reg[dest]&~val(ops[1])&MASK;self.reg[dest]=value;self.nz(value)\n   elif m in ['cmp','cmn']:")
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
# ARM arithmetic flag effects; CMP overwrites NZCV, logical immediates set shifter carry.
arm_source=arm_source.replace('def original(registers):','def original(registers,flags):').replace('N=Z=C=False','N,Z,C,V=flags')
arm_source=arm_source.replace("return regs", "return regs,(N,Z,C,V)")
arm_source=arm_source.replace('N=bool(d&0x80000000);Z=d==0;C=x>=y', 'N=bool(d&0x80000000);Z=d==0;C=x>=y;V=bool((x^y)&(x^d)&0x80000000)')
arm_source=arm_source.replace("elif mnemonic=='tst':Z=(value(parts[0])&value(', '.join(parts[1:])))==0", "elif mnemonic=='tst':\n    operand=value(', '.join(parts[1:]));result=value(parts[0])&operand;N=bool(result&0x80000000);Z=result==0\n    assert operand in (0x80000000,0x40000000);C=bool(operand&0x80000000)")
arm_source=arm_source.replace("if mnemonic=='ands':v=x&y;N=bool(v&0x80000000);Z=v==0", "if mnemonic=='ands':\n      assert y==0x80000000;v=x&y;N=bool(v&0x80000000);Z=v==0;C=True")
arm_registers={};exec(arm_source,arm_registers)
arm_quotient_calls=[]
def execute_original_arm_quotient(owner):
 target=0x03000008+0x29fc-0x700;assert owner.reg[2]==target
 assert original['r'][0x32454:0x32456]==bytes.fromhex('1047')
 code=original['r'][0x29fc:0x2ce8]
 assert bytes(owner.read(target+i,1) for i in range(len(code)))==code,'relocated quotient body differs from original'
 before=owner.reg.copy();start=arm_registers['steps'];flags_before=tuple(owner.flags);out,flags_after=arm_registers['original'](before,flags_before);owner.flags=list(flags_after)
 assert [out['r'+str(i)] for i in range(4,12)]==before[4:12] and out['sp']==before[13] and out['lr']==before[14]
 # Every decoded operation is arithmetic/control, with no memory instruction.
 for address,(mnemonic,args) in arm_registers['code'].items():
  stem=mnemonic[:-2] if mnemonic[-2:] in ('mi','eq','ne','hi','hs') else mnemonic
  assert stem in ('bx','b','cmp','tst','ands','eor','rsb','mov','adc','sub'),(hex(address),mnemonic)
 owner.reg[:4]=[out['r'+str(i)] for i in range(4)];owner.reg[12]=out['ip']
 arm_quotient_calls.append({'arguments':before[:2],'scratch_before':before[2:4]+[before[12]],'scratch_after':owner.reg[:4]+[owner.reg[12]],'instructions':arm_registers['steps']-start,'flags_before':flags_before,'flags_after':flags_after,'memory_stores':0,'relocated_identity_bytes':len(code)})

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


# Execute the original Thumb wrapper and ROM ARM body, including shared stack saves.
containment_calls=[]
for i in rng_ns['md'].disasm(original['r'][0x2df0c:0x2df16],0x0802df0c):rng_ns['allowed_thumb'][i.address]=bytes(i.bytes)
def execute_shared_containment(owner):
 sp=owner.reg[13];before=owner.reg.copy();stores=[];arm_steps=0
 case=tuple(original['signed'](owner.read(ptr+off,4)) for ptr in owner.reg[:4] for off in (0,8))
 expected=actor_ctx['ground']['containment'](case,owner.flags[3])[0]
 def write(address,value,size=4):
  assert sp-44<=address and address+size<=sp,('containment nonstack store',hex(address),size)
  stores.append([hex(address),size]);owner.write(address,value,size)
 p=SharedRandomProbe((0,0,0,0,0,1,0,0,0,0));p.reg=owner.reg;p.read=owner.read;p.write=write;p.flags=owner.flags
 def arm_thunk(target):
  nonlocal arm_steps
  assert target==0x0803408c
  assert original['r'][0x3408c:0x34090]==bytes.fromhex('7847c046')
  branch=int.from_bytes(original['r'][0x34090:0x34094],'little');assert branch>>24==0xea
  displacement=(branch&0xffffff);displacement=displacement-(1<<24) if displacement&(1<<23) else displacement
  assert 0x08034098+(displacement<<2)==0x080051cc
  reg=p.reg;n,z,c,v=p.flags
  for pc in range(0x51cc,0x52b4,4):
   w=int.from_bytes(original['r'][pc:pc+4],'little');arm_steps+=1;cond=w>>28
   assert cond in (14,0,12,13)
   if not {14:True,0:z,12:not z and n==v,13:z or n!=v}[cond]:continue
   if (w&0x0ffffff0)==0x012fff10:
    assert (w&15)==14;p.flags=[n,z,c,v];return
   if (w>>25)&7==4:
    assert ((w>>16)&15)==13 and w&0xffff==0x5ff0
    regs=[j for j in range(16) if w&(1<<j)];load=bool(w&(1<<20))
    if load:
     assert not(w&(1<<24)) and w&(1<<23) and w&(1<<21)
     base=reg[13]
     for j,k in enumerate(regs):reg[k]=p.read(base+4*j,4)
     reg[13]=base+4*len(regs)
    else:
     assert w&(1<<24) and not(w&(1<<23)) and w&(1<<21)
     base=reg[13]-4*len(regs)
     for j,k in enumerate(regs):p.write(base+4*j,reg[k],4)
     reg[13]=base
    continue
   if (w&0x0c000000)==0x04000000:
    assert not(w&(1<<25)) and w&(1<<20) and w&(1<<24) and w&(1<<23)
    reg[(w>>12)&15]=p.read(reg[(w>>16)&15]+(w&4095),4);continue
   if (w&0x0fc000f0)==0x00000090:
    dest=(w>>16)&15;value=reg[w&15]*reg[(w>>8)&15]
    if w&(1<<21):value+=reg[(w>>12)&15]
    reg[dest]=value&0xffffffff
    if w&(1<<20):n=bool(reg[dest]&0x80000000);z=reg[dest]==0
    continue
   opcode=(w>>21)&15;rd=(w>>12)&15;left=reg[(w>>16)&15]
   if w&(1<<25):
    imm=w&255;shift=((w>>8)&15)*2;operand=((imm>>shift)|(imm<<(32-shift)))&0xffffffff if shift else imm
   else:
    operand=reg[w&15];shift=(w>>7)&31;typ=(w>>5)&3;assert not(w&16)
    if shift:assert typ==2;operand=(original['signed'](operand)>>shift)&0xffffffff
   if opcode==10:
    out=(left-operand)&0xffffffff;n=bool(out&0x80000000);z=out==0;c=left>=operand;v=bool((left^operand)&(left^out)&0x80000000)
   else:
    assert opcode in (2,4,13) and not(w&(1<<20))
    reg[rd]=({2:lambda:left-operand,4:lambda:left+operand,13:lambda:operand}[opcode]())&0xffffffff
  raise AssertionError('containment did not return')
 p.external=arm_thunk;p.run(entry=0x0802df0c,stop=owner.reg[14]&~1)
 assert p.reg[13]==sp and p.reg[4:13]==before[4:13]
 assert original['signed'](p.reg[0])==expected
 owner.flags=p.flags;owner.steps+=p.steps
 containment_calls.append({'arguments':before[:4],'result':expected,'thumb_instructions':p.steps,'arm_instructions':arm_steps,'stack_stores':stores,'flags_after':p.flags})

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
 if target in (0x0802aba0,0x0802a27c):p=scene_ns['wn']['WorldProbe']((0,0,0,0,0,0,0,1))
 elif target==0x08029b44:p=actor_ctx['ground']['GroundProbe']((0,0,0,0,0,1),0)
 else:raise AssertionError(('unsupported shared ground entry',hex(target)))
 def write(address,value,size=4):
  assert not(address<0x0200351e and address+size>0x020034dc),('ground Thumb input store',hex(address),size)
  stores.append([hex(address),size]);owner.write(address,value,size)
 p.reg=owner.reg;p.flags=owner.flags;p.read=owner.read;p.write=write;p.arm.mem=SharedGroundWords(owner,stores);p.arm.read=lambda address:owner.read(address,4)
 prior_external=p.external
 def external(callee):
  calls.append({'target':hex(callee),'arguments':p.reg[:4].copy()})
  if callee==0x0802df0c:execute_shared_containment(p)
  elif callee==0x08032488:execute_original_division(p,callee)
  elif callee==0x08032454 and p.reg[2]==0x03002304:execute_original_arm_quotient(p)
  elif callee in (0x08032454,0x08032458):
   index=2 if callee==0x08032454 else 3;entry=p.reg[index]
   canonical=entry+0x08000700-0x03000008 if 0x03000008<=entry<0x03004964 else entry
   assert canonical in (0x08000b78,0x08000a1c),('unqualified shared ground ARM target',hex(entry),hex(canonical))
   raise AssertionError(('shared ground ARM full ABI not qualified',hex(entry),hex(canonical),p.reg[:4]))
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

bootstrap_ns={};exec((root/'tools/compare_audio_bootstrap_rom.py').read_text().split(chr(10)+'cases=')[0],bootstrap_ns)

input_ns={};exec((root/'tools/compare_input_sampler_rom.py').read_text().split(chr(10)+'cases=')[0],input_ns)
sampled_fixture=[]
def original_sampled_fixture(owner):
 saved=owner.reg.copy();saved_flags=list(owner.flags);stores=[]
 # Explicit initialized-input/idle platform fixture; never claimed a captured warm game.
 for i in range(66):owner.write(0x020034dc+i,0,1)
 owner.write(0x04000130,1023,2);owner.write(0x02003b00,1,4)
 q=input_ns['InputProbe']((0,0,0,0,0,1),0);q.reg=owner.reg;q.flags=owner.flags;q.read=owner.read
 def write(address,value,size=4):
  assert (0x020034dc<=address and address+size<=0x02003520) or (saved[13]-64<=address and address+size<=saved[13]),('input sampler write outside qualified owner',hex(address),size)
  stores.append([hex(address),size]);owner.write(address,value,size)
 q.write=write;q.reg[14]=0x10020001
 try:q.run(entry=0x08015c3c,stop=0x10020000)
 except Exception as exc:
  (root/'reports/bound-first-frame-sampler-next-dependency.json').write_text(json.dumps({'status':'SAMPLER_DEPENDENCY','instructions':q.steps,'stores':stores,'error':str(exc)},indent=2)+chr(10));raise
 assert q.reg[13]==saved[13]
 state=bytes(owner.read(0x020034dc+i,1) for i in range(66))
 sampled_fixture.append({'raw_active_low_keys':1023,'delta':1,'instructions':q.steps,'state_hex':state.hex(),'stores':stores})
 owner.reg[:]=saved;owner.flags=saved_flags;owner.steps+=q.steps

matrix_ns={};matrix_source=(root/'tools/compare_bound_pivot.py').read_text().split('# Source motion')[0]
matrix_source=matrix_source.replace('self.reg[:len(args)]=args;self.reg[14]=0xffffffff;pc=entry','self.reg[:len(args)]=args;stop=self.reg[14];pc=entry').replace('while pc!=0xffffffff:','while pc!=stop:')
matrix_source=matrix_source.replace("assert any(0x8000000+a<=pc<0x8000000+b for a,b in allowed),hex(pc)","assert entry<=pc<entry+104,hex(pc)")
exec(matrix_source,matrix_ns)
matrix_calls=[];trig_loader_calls=[]
def execute_original_yaw_matrix(owner):
 entry=0x03000480;assert owner.reg[2]==entry
 code=original['r'][0xb78:0xbe0];assert bytes(owner.read(entry+i,1) for i in range(104))==code
 # This complete routine has no flag-setting instructions, multiply or internal call.
 for off in range(0,104,4):
  w=int.from_bytes(code[off:off+4],'little');assert w>>28==14
  if w&0x0c000000==0 and w&0x0ffffff0!=0x012fff10:assert not(w&(1<<20))
 before=owner.reg.copy();flags=tuple(owner.flags);out=before[0];sp=before[13];stores=[]
 q=matrix_ns['Probe']();q.reg=before.copy();q.read=lambda address:owner.read(address,4)
 def write(address,value):
  assert (sp-40<=address and address+4<=sp) or (out<=address and address+4<=out+64),('matrix nonowner store',hex(address))
  stores.append([hex(address),4]);owner.write(address,value,4)
 q.write=write;q.run(entry,before[:4])
 assert q.reg[4:15]==before[4:15] and tuple(owner.flags)==flags
 owner.reg[:15]=q.reg[:15]
 matrix_calls.append({'entry':hex(entry),'before':before[:15],'after':q.reg[:15],'flags_preserved':list(flags),'arm_instructions':q.steps,'stores':stores,'matrix_hex':bytes(owner.read(out+i,1) for i in range(64)).hex(),'code_identity_bytes':104})
for i in original['md'].disasm(original['r'][0x2db54:0x2dbb6],0x0802db54):original['allowed_thumb'][i.address]=bytes(i.bytes)
def original_trig_loader(owner):
 saved=owner.reg.copy();saved_flags=list(owner.flags);start=owner.steps;owner.reg[14]=0x10020001
 owner.run(entry=0x0802db54,stop=0x10020000);assert owner.reg[13]==saved[13]
 tables=[]
 for dest,src in ((0x0202c4f0,0x084a0d08),(0x020304f0,0x084a2d08)):
  expected=b''.join((original['signed'](int.from_bytes(original['r'][src-0x08000000+2*i:src-0x08000000+2*i+2],'little')<<16)>>20&0xffffffff).to_bytes(4,'little') for i in range(4096))
  actual=bytes(owner.read(dest+i,1) for i in range(16384));assert actual==expected
  tables.append({'destination':hex(dest),'source':hex(src),'bytes':16384,'sha256':hashlib.sha256(actual).hexdigest()})
 trig_loader_calls.append({'instructions':owner.steps-start,'tables':tables});owner.reg[:]=saved;owner.flags=saved_flags

vector_ns={}
vector_source=matrix_source.replace('assert entry<=pc<entry+104,hex(pc)', 'assert any(a<=pc<b for a,b in ((0x03000020,0x03000040),(0x0300025c,0x030002c4),(0x03000324,0x03000348))),hex(pc)')
vector_source=vector_source.replace('rd=(w>>16)&15;rn=(w>>12)&15;rs=(w>>8)&15;rm=w&15', "assert not w&(1<<20),('multiply flags',hex(pc));rd=(w>>16)&15;rn=(w>>12)&15;rs=(w>>8)&15;rm=w&15")
exec(vector_source,vector_ns)
vector_calls=[]
def execute_original_vector(owner):
 entry=0x03000324;assert owner.reg[3]==entry
 for lo,hi in ((0x718,0x738),(0x954,0x9bc),(0xa1c,0xa40)):
  assert bytes(owner.read(0x03000000+lo-0x6f8+i,1) for i in range(hi-lo))==original['r'][lo:hi]
 before=owner.reg.copy();flags=tuple(owner.flags);sp=before[13];dest=before[2];stores=[]
 q=vector_ns['Probe']();q.reg=before.copy();q.read=lambda address:owner.read(address,4)
 def write(address,value):
  assert (sp-64<=address and address+4<=sp) or (0x03006a50<=address and address+4<=0x03006a90) or (dest<=address and address+4<=dest+12),('vector nonowner store',hex(address))
  stores.append([hex(address),4]);owner.write(address,value,4)
 q.write=write;q.run(entry,before[:4])
 assert q.reg[4:12]==before[4:12] and q.reg[13:15]==before[13:15] and tuple(owner.flags)==flags
 owner.reg[:15]=q.reg[:15]
 vector_calls.append({'entry':hex(entry),'before':before[:15],'after':q.reg[:15],'flags_preserved':list(flags),'arm_instructions':q.steps,'stores':stores,'output_hex':bytes(owner.read(dest+i,1) for i in range(12)).hex()})


for ins in json.loads((root/'reports/effect-pool-update-cfg.json').read_text())['instructions']:
 address=int(ins['address'],16);data=bytes.fromhex(ins['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data;original['allowed_thumb'][address]=data
frame_pool_calls=[]
def original_effect_pool(owner):
 before=owner.reg.copy();start=owner.steps
 owner.run(entry=0x080208d4,stop=owner.reg[14]&~1)
 assert owner.reg[13]==before[13] and owner.reg[4:8]==before[4:8]
 frame_pool_calls.append({'instructions':owner.steps-start,'arguments':before[:4]})


for ins in json.loads((root/'reports/camera-view-cfg.json').read_text())['instructions']:
 address=int(ins['address'],16);data=bytes.fromhex(ins['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data;original['allowed_thumb'][address]=data
frame_camera_calls=[]
def original_camera_update(owner):
 before=owner.reg.copy();start=owner.steps;actor=before[1]
 camera_before=bytes(owner.read(actor+0x498+i,1) for i in range(24)).hex()
 owner.run(entry=0x08014dd8,stop=owner.reg[14]&~1)
 assert owner.reg[13]==before[13] and owner.reg[4:12]==before[4:12]
 frame_camera_calls.append({'instructions':owner.steps-start,'arguments':before[:4],'camera_before':camera_before,'camera_after':bytes(owner.read(actor+0x498+i,1) for i in range(24)).hex()})


for ins in json.loads((root/'reports/bound-first-frame-camera-angle-dependency.json').read_text())['instructions']:
 address=int(ins['address'],16);data=bytes.fromhex(ins['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data
 if 0x0802dd5c<=address<0x0802dd66:original['allowed_thumb'][address]=data
angle_calls=[]
def original_angle_arm(owner):
 assert original['r'][0x34094:0x3409c].hex()=='7847c0468544ffea'
 before=owner.reg.copy();pc=0x080052b4;steps=0;n,z,c,v=owner.flags
 while True:
  assert 0x080052b4<=pc<0x0800531c and pc%4==0
  steps+=1;assert steps<=40
  w=owner.read(pc,4);condition=w>>28
  execute={14:True,11:n!=v,12:not z and n==v,10:n==v}[condition]
  if not execute:pc+=4;continue
  owner.reg[15]=pc+8
  if w&0x0ffffff0==0x012fff10:
   assert w&15==14;break
  if w&0x0e000000==0x0a000000:
   assert not w&(1<<24);off=w&0xffffff;off=off-0x1000000 if off&0x800000 else off;pc+=8+off*4;continue
  if w==0xe51f20f4:owner.reg[2]=owner.read(pc+8-0xf4,4);pc+=4;continue
  assert w&0x0c000000==0
  rn=(w>>16)&15;rd=(w>>12)&15;op=(w>>21)&15
  if w&(1<<25):
   value=w&255;rot=((w>>8)&15)*2;value=((value>>rot)|(value<<(32-rot)))&0xffffffff if rot else value
  else:assert w&0xff0==0;value=owner.reg[w&15]
  a=owner.reg[rn]
  if op==10:
   assert w&(1<<20);result=(a-value)&0xffffffff;n=bool(result&0x80000000);z=result==0;c=a>=value;v=bool((a^value)&(a^result)&0x80000000)
  else:
   assert not w&(1<<20)
   if op==0:result=a&value
   elif op==2:result=a-value
   elif op==3:result=value-a
   elif op==4:result=a+value
   elif op==13:result=value
   else:raise AssertionError(('angle op',hex(pc),op))
   owner.reg[rd]=result&0xffffffff
  pc+=4
 owner.flags=[n,z,c,v]
 assert owner.reg[4:15]==before[4:15]
 angle_calls.append({'before':before[:15],'after':owner.reg[:15],'flags_after':owner.flags.copy(),'arm_instructions':steps,'writes':[]})
def original_angle_wrapper(owner):
 before=owner.reg.copy();owner.run(entry=0x0802dd5c,stop=owner.reg[14]&~1)
 assert owner.reg[13]==before[13] and owner.reg[4:12]==before[4:12]


rotation_ns={}
rotation_source=vector_source.replace("assert any(a<=pc<b for a,b in ((0x03000020,0x03000040),(0x0300025c,0x030002c4),(0x03000324,0x03000348))),hex(pc)", "assert any(a-0x6f8+0x03000000<=pc<b-0x6f8+0x03000000 for a,b in ((0x738,0x954),(0xa40,0xb10),(0xb10,0xc48))),hex(pc)")
exec(rotation_source,rotation_ns)
rotation_calls=[]
def original_camera_rotation(owner):
 entry=owner.reg[4];assert entry in (0x03000348,0x030003b0)
 for lo,hi in ((0x738,0x954),(0xa40,0xb10),(0xb10,0xc48)):
  assert bytes(owner.read(0x03000000+lo-0x6f8+i,1) for i in range(hi-lo))==original['r'][lo:hi]
 before=owner.reg.copy();flags=tuple(owner.flags);sp=before[13];dest=before[0];stores=[]
 q=rotation_ns['Probe']();q.reg=before.copy();q.read=lambda address:owner.read(address,4)
 def write(address,value):
  assert (sp-316<=address and address+4<=sp) or (dest<=address and address+4<=dest+64),('rotation nonowner store',hex(address))
  stores.append([hex(address),4]);owner.write(address,value,4)
 q.write=write;q.run(entry,before[:4])
 assert q.reg[4:12]==before[4:12] and q.reg[13:15]==before[13:15] and tuple(owner.flags)==flags
 owner.reg[:15]=q.reg[:15]
 rotation_calls.append({'before':before[:15],'after':q.reg[:15],'arm_instructions':q.steps,'stores':stores,'matrix_hex':bytes(owner.read(dest+i,1) for i in range(64)).hex(),'flags_preserved':list(flags)})


vector_io_ns={}
vector_io_source=vector_source.replace("assert any(a<=pc<b for a,b in ((0x03000020,0x03000040),(0x0300025c,0x030002c4),(0x03000324,0x03000348))),hex(pc)", "assert any(a-0x6f8+0x03000000<=pc<b-0x6f8+0x03000000 for a,b in ((0x718,0x738),(0x954,0xa04))),hex(pc)")
exec(vector_io_source,vector_io_ns)
vector_io_calls=[]
def original_camera_vector_io(owner,target):
 entry=owner.reg[0 if target==0x0803244c else 1]
 canonical=entry-0x03000000+0x6f8;assert canonical in (0x718,0x9bc,0x9d4,0x9ec)
 for lo,hi in ((0x718,0x738),(0x954,0xa04)):
  assert bytes(owner.read(0x03000000+lo-0x6f8+i,1) for i in range(hi-lo))==original['r'][lo:hi]
 before=owner.reg.copy();flags=tuple(owner.flags);sp=before[13];stores=[]
 vector_base=int.from_bytes(original['r'][0x704:0x708],'little')
 assert vector_base==0x03006aa0
 q=vector_io_ns['Probe']();q.reg=before.copy();q.read=lambda address:owner.read(address,4)
 def write(address,value):
  stack=sp-48<=address and address+4<=sp
  matrix=canonical==0x718 and 0x03006a50<=address and address+4<=0x03006a90
  vector=canonical in (0x9bc,0x9d4) and vector_base<=address and address+4<=vector_base+12
  output=canonical==0x9ec and before[0]<=address and address+4<=before[0]+12
  assert stack or matrix or vector or output,('vector IO nonowner store',hex(address))
  stores.append([hex(address),4]);owner.write(address,value,4)
 q.write=write;q.run(entry,before[:4])
 assert q.reg[4:12]==before[4:12] and q.reg[13:15]==before[13:15] and tuple(owner.flags)==flags
 owner.reg[:15]=q.reg[:15]
 vector_io_calls.append({'entry':hex(entry),'canonical':hex(canonical),'before':before[:15],'after':q.reg[:15],'arm_instructions':q.steps,'stores':stores,'flags_preserved':list(flags)})


for entry,end in ((0x080295a8,0x0802964e),(0x0802965c,0x08029662)):
 address=entry
 while True:
  assert address<=end;ins=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));original['allowed_thumb'][address]=bytes(ins.bytes)
  if ins.mnemonic=='bx':break
  assert ins.mnemonic not in ('b','bl');address+=ins.size
frame_view_constructor_calls=[]
def original_view_constructor(owner,target):
 before=owner.reg.copy();start=owner.steps;dest=before[0];stores_before=len(owner.writes)
 prior=bytes(owner.read(dest+i,1) for i in range(72)).hex()
 owner.run(entry=target,stop=owner.reg[14]&~1)
 assert owner.reg[13:15]==before[13:15] and owner.reg[4:12]==before[4:12]
 stores=owner.writes[stores_before:]
 for address,size in stores:
  assert (dest<=address and address+size<=dest+72) or (before[13]-32<=address and address+size<=before[13]),('constructor store',hex(address),size)
 frame_view_constructor_calls.append({'entry':hex(target),'arguments':before[:4],'instructions':owner.steps-start,'before':prior,'after':bytes(owner.read(dest+i,1) for i in range(72)).hex(),'stores':stores})


for name in ('taxi-renderer','taxi-half-transform'):
 for ins in json.loads((root/('reports/'+name+'-cfg.json')).read_text())['instructions']:
  address=int(ins['address'],16);data=bytes.fromhex(ins['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data;original['allowed_thumb'][address]=data
frame_taxi_renderer_calls=[]
def original_taxi_renderer(owner,target):
 before=owner.reg.copy();start=owner.steps;stores_before=len(owner.writes)
 owner.run(entry=target,stop=owner.reg[14]&~1)
 assert owner.reg[13]==before[13] and owner.reg[4:12]==before[4:12]
 frame_taxi_renderer_calls.append({'entry':hex(target),'arguments':before[:4],'instructions':owner.steps-start,'stores':owner.writes[stores_before:]})


for ins in original['md'].disasm(original['r'][0x3201c:0x32022],0x0803201c):original['allowed_thumb'][ins.address]=bytes(ins.bytes)
frame_depth_setter_calls=[]
def original_depth_setter(owner):
 before=owner.reg.copy();start=owner.steps;dest=int.from_bytes(original['r'][0x32024:0x32028],'little');assert dest==0x020368f0
 prior=owner.read(dest,4);stores_before=len(owner.writes)
 owner.run(entry=0x0803201c,stop=owner.reg[14]&~1)
 assert owner.read(dest,4)==before[0] and owner.reg[2:15]==before[2:15]
 assert owner.writes[stores_before:]==[(dest,4)]
 frame_depth_setter_calls.append({'instructions':owner.steps-start,'destination':hex(dest),'before':prior,'after':owner.read(dest,4)})


for lo,hi in ((0x31a50,0x31ee4),(0x2dbc8,0x2dc12),(0x2dc20,0x2dc6a)):
 for ins in original['md'].disasm(original['r'][lo:hi],0x08000000+lo):original['allowed_thumb'][ins.address]=bytes(ins.bytes)
frame_shadow_packet_calls=[]
def original_shadow_packet(owner,target):
 before=owner.reg.copy();start=owner.steps;stores_before=len(owner.writes)
 owner.run(entry=target,stop=owner.reg[14]&~1)
 assert owner.reg[13]==before[13] and owner.reg[4:12]==before[4:12]
 frame_shadow_packet_calls.append({'entry':hex(target),'arguments':before[:4],'instructions':owner.steps-start,'stores':owner.writes[stores_before:]})


original['allowed_thumb'].update(hud_ns['ns']['ns']['allowed_thumb'])
frame_sprite_resource_calls=[]
def original_sprite_resource(owner):
 before=owner.reg.copy();start=owner.steps;stores_before=len(owner.writes);saved_write=owner.write;copies=[]
 def write(address,value,size=4):
  saved_write(address,value,size)
  if address==0x040000dc:
   assert size==4 and value>>16==0x8400
   src=owner.read(0x040000d4,4);dst=owner.read(0x040000d8,4);count=(value&65535)*4
   assert 0<count<=32768 and 0x06000000<=dst and dst+count<=0x06020000
   assert (0x08000000<=src and src+count<=0x08800000) or (0x02000000<=src and src+count<=0x02040000)
   data=bytes(owner.read(src+i,1) for i in range(count))
   for i,b in enumerate(data):saved_write(dst+i,b,1)
   assert bytes(owner.read(dst+i,1) for i in range(count))==data
   copies.append({'source':hex(src),'destination':hex(dst),'bytes':count,'sha256':hashlib.sha256(data).hexdigest()})
 owner.write=write
 try:owner.run(entry=0x08027fb8,stop=owner.reg[14]&~1)
 finally:owner.write=saved_write
 assert owner.reg[13]==before[13] and owner.reg[4:12]==before[4:12]
 frame_sprite_resource_calls.append({'arguments':before[:4],'instructions':owner.steps-start,'result':hex(owner.reg[0]),'copies':copies,'stores':owner.writes[stores_before:]})


queue_ns={};exec((root/'tools/compare_startup_obj_scaled_rom.py').read_text().split('assets=json.loads')[0],queue_ns)
for address,data in queue_ns['allowed_thumb'].items():
 assert original['r'][address-0x08000000:address-0x08000000+len(data)]==bytes(data)
 original['allowed_thumb'][address]=bytes(data)

frame_object_submit_calls=[]
def original_object_submit(owner,target):
 before=owner.reg.copy();start=owner.steps;stores_before=len(owner.writes)
 owner.run(entry=target,stop=owner.reg[14]&~1)
 assert owner.reg[13]==before[13] and owner.reg[4:12]==before[4:12]
 frame_object_submit_calls.append({'entry':hex(target),'arguments':before[:4],'instructions':owner.steps-start,'stores':owner.writes[stores_before:]})

bootstrap_calls=[]


def shared_audio_bootstrap(owner):
 saved=owner.reg.copy();saved_flags=list(owner.flags);stores=[];calls=[];boot_dma=[]
 b=bootstrap_ns['BootstrapProbe']((0,0,0,0,0,1,0,0,0,0));b.reg=owner.reg;b.flags=owner.flags
 b.regions=owner.regions;b.scalars=owner.scalars;b.arm.mem=SharedGroundWords(owner,stores);b.arm.read=lambda address:owner.read(address,4)
 b.awaiting=False;b.events=b.fifo_writes=b.last_dispatch=0;b.arm_steps=b.callbacks=b.draws=0
 def read(address,size=4):
  if address==0x02003528 and b.awaiting:raise AssertionError('bootstrap requires explicit interrupt service')
  return owner.read(address,size)
 def write(address,value,size=4):
  stores.append([hex(address),size]);owner.write(address,value,size)
  if address==0x040000dc and value&0x80000000:
   src=owner.read(0x040000d4,4);dst=owner.read(0x040000d8,4);width=4 if value&0x04000000 else 2;count=(value&65535)*width
   assert value>>16==0x8400 and width==4 and 0<count<=4096 and 0x08000000<=src and src+count<=0x08800000 and 0x03000000<=dst and dst+count<=0x03010000, ('unsupported bootstrap DMA mode/bounds',hex(src),hex(dst),count,hex(value))
   data=bytes(owner.read(src+i,1) for i in range(count))
   for i,v in enumerate(data):owner.write(dst+i,v,1)
   assert bytes(owner.read(dst+i,1) for i in range(count))==data
   boot_dma.append({'source':hex(src),'destination':hex(dst),'bytes':count,'sha256':hashlib.sha256(data).hexdigest()})
  if address==0x02003528 and not(value&1):b.awaiting=True
  if address==0x040000a0:b.fifo_writes+=1
 b.read=read;b.write=write;prior=b.external
 def external(target):
  calls.append({'target':hex(target),'arguments':b.reg[:4].copy()})
  if target==0x08032454:execute_original_arm_quotient(b)
  else:prior(target)
 b.external=external
 # Explicit interrupt-disabled platform fixture, not original asynchronous system startup.
 owner.write(0x04000208,0,2);b.reg[0]=2;b.reg[14]=0x10020001
 try:b.run(entry=0x08026488,stop=0x10020000)
 except Exception as exc:
  (root/'reports/bound-shared-audio-bootstrap-next-dependency.json').write_text(json.dumps({'status':'BOOTSTRAP_DEPENDENCY','instructions':b.steps,'calls':calls,'stores':stores,'error':str(exc)},indent=2)+chr(10));raise
 assert b.reg[13]==saved[13]
 assert owner.read(0x02007564,4)==21024 and owner.read(0x02007568,4)==352
 bootstrap_calls.append({'instructions':b.steps,'calls':calls,'stores':stores,'rate':owner.read(0x02007564,4),'count':owner.read(0x02007568,4),'interrupt_fixture':0,'logical_dma_copies':boot_dma})
 owner.reg[:]=saved;owner.flags=saved_flags;owner.steps+=b.steps
rows=[]
for seed,settings in [(0,128)]:
 p=PrefixProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x02000000:bytearray(262144),0x03000000:bytearray(65536),0x05000000:bytearray(1024),0x06000000:bytearray(131072),0x07000000:bytearray(1024)};p.scalars={}
 p.regions[0x0600a000]=memoryview(p.regions[0x06000000])[0xa000:0xa000+38416];p.regions[0x06014000]=memoryview(p.regions[0x06000000])[0x14000:0x1c000]
 p.write(0x03004c9c,0x084a4ff4);p.write(0x03004ca0,0x02002634);p.write(0x03004c40,0x10030001);p.write(0x02001770,4,1);p.write(0x02007600,1,1)
 p.regions[0x02000000][0x2634:0x34d4]=bytes.fromhex(capture[9])
 for i in range(44):p.write(33578796+i,(seed+i*37)&255,1)
 for i in range(66):p.write(0x020034dc+i,(seed+i*13)&255,1)
 p.write(0x02001eb0,settings,1);p.write(0x02001537,5,1);p.write(0x02003b04,seed)
 p.reg[13]=0x10000400;p.reg[10]=1
 for ins in original['md'].disasm(original['r'][0x7436:0x743c],0x08007436):original['allowed_thumb'][ins.address]=bytes(ins.bytes)
 p.run(entry=0x08007436,stop=0x0800743c)
 assert p.read(p.reg[13]+0x44,4)==p.reg[13]+0x28
 original_trig_loader(p)
 original_sampled_fixture(p)
 before=bytes(p.regions[0x02000000][0x34dc:0x351e])
 for invocation in range(2):
  if invocation==1:shared_audio_bootstrap(p)
  p.prefix_calls=[];p.writes=[];start=p.steps;p.run(entry=0x0800743c,stop=0x08007538)
  assert [q['target'] for q in p.prefix_calls]==[hex(t) for t in roots]
  assert before==bytes(p.regions[0x02000000][0x34dc:0x351e])
  assert not any(a<0x0200351e and a+n>0x020034dc for a,n in p.writes)
  assert p.reg[13]==0x10000400 and p.reg[10]==0x10000428
  assert p.read(0x020034d4)==p.read(0x020034d8)==1
  assert [p.read(a,1) for a in (0x02001691,0x0200153c,0x0200153d,0x0200153a)]==[0]*4
  rows.append({'seed':seed,'settings':settings,'invocation':invocation,'instructions':p.steps-start,'prefix_calls':p.prefix_calls,'palette_count':p.read(0x02001780,2),'front':hex(p.read(0x0200754c)),'back':hex(p.read(0x02007550))})
# Resume actual caller memory; fail visibly at the first unqualified frame dependency.
flow=json.loads((root/'reports/bound-restart-frame-flow.json').read_text())
for row in flow['instructions']:
 address=int(row['address'],16);i=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));original['allowed_thumb'][address]=bytes(i.bytes)
todo=[0x08008d60];seen=set()
while todo:
 address=todo.pop()
 if address in seen:continue
 assert 0x08008d60<=address<0x08008f28,('frame callback CFG outside bound',hex(address))
 seen.add(address);i=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));original['allowed_thumb'][address]=bytes(i.bytes)
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='b':todo.append(int(i.op_str[1:],16));continue
 todo.append(address+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):todo.append(int(i.op_str[1:],16))

todo=[0x080206bc];seen_guidance=set()
while todo:
 address=todo.pop()
 if address in seen_guidance:continue
 assert 0x080206bc<=address<0x08020824,('guidance CFG outside bound',hex(address))
 seen_guidance.add(address);i=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));original['allowed_thumb'][address]=bytes(i.bytes)
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='b':todo.append(int(i.op_str[1:],16));continue
 todo.append(address+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):todo.append(int(i.op_str[1:],16))

frame_audio_ns={};exec((root/'tools/compare_game_audio_controller_rom.py').read_text().split('\nrng=random.Random')[0],frame_audio_ns)
original['allowed_thumb'].update(frame_audio_ns['allowed_thumb'])

todo=[0x080267c8];seen_channel=set()
while todo:
 address=todo.pop()
 if address in seen_channel:continue
 assert 0x080267c8<=address<0x08026886,('channel CFG outside bound',hex(address))
 seen_channel.add(address);i=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));original['allowed_thumb'][address]=bytes(i.bytes)
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='b':todo.append(int(i.op_str[1:],16));continue
 todo.append(address+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):todo.append(int(i.op_str[1:],16))

for lo,hi in ((0x276cc,0x276da),(0x276dc,0x276fa)):
 for i in original['md'].disasm(original['r'][lo:hi],0x08000000+lo):original['allowed_thumb'][i.address]=bytes(i.bytes)

action_cfg=json.loads((root/'reports/challenge-bound-acceleration-action-query-cfg.json').read_text())
original['allowed_thumb'].update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in action_cfg['instructions']})

todo=[0x0800dec8];seen_recorded=set()
while todo:
 address=todo.pop()
 if address in seen_recorded:continue
 assert 0x0800dec8<=address<0x0800df8c,('recorded-input CFG outside bound',hex(address))
 seen_recorded.add(address);i=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));original['allowed_thumb'][address]=bytes(i.bytes)
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='b':todo.append(int(i.op_str[1:],16));continue
 todo.append(address+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):todo.append(int(i.op_str[1:],16))
frame_recorded_calls=[]

pause_function=json.loads((root/'reports/pause-palette-request-owners-audit.json').read_text())['functions'][0]
assert int(pause_function['entry'],16)==0x08021c5c
for row in pause_function['instructions']:
 address=int(row['address'],16);data=bytes.fromhex(row['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data
 original['allowed_thumb'][address]=data

todo=[0x0800ba04];seen_countdown=set()
while todo:
 address=todo.pop()
 if address in seen_countdown:continue
 assert 0x0800ba04<=address<0x0800c534,('countdown CFG outside bound',hex(address))
 seen_countdown.add(address);i=next(original['md'].disasm(original['r'][address-0x08000000:address-0x08000000+4],address,count=1));original['allowed_thumb'][address]=bytes(i.bytes)
 if i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 if i.mnemonic=='bl' and i.op_str=='#0x800c524':todo.append(0x0800c524);continue
 if i.mnemonic=='b':todo.append(int(i.op_str[1:],16));continue
 todo.append(address+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic not in ('bl','blx','bic','bics') and i.op_str.startswith('#'):todo.append(int(i.op_str[1:],16))

for name in ('gear','rpm','clamp'):
 cfg=json.loads((root/f'reports/challenge-bound-{name}-cfg.json').read_text())
 for row in cfg['instructions']:
  address=int(row['address'],16);data=bytes.fromhex(row['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data;original['allowed_thumb'][address]=data

for i in original['md'].disasm(original['r'][0x15ac8:0x15af8],0x08015ac8):original['allowed_thumb'][i.address]=bytes(i.bytes)

world_cfg=json.loads((root/'reports/challenge-bound-world-cfg.json').read_text());assert int(world_cfg['entry'],16)==0x08011f80
for row in world_cfg['instructions']:
 address=int(row['address'],16);data=bytes.fromhex(row['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data;original['allowed_thumb'][address]=data

cfg=json.loads((root/'reports/challenge-bound-gravity-cfg.json').read_text())
for row in cfg['instructions']:
 address=int(row['address'],16);data=bytes.fromhex(row['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data;original['allowed_thumb'][address]=data

for name in ('decay','springs','align'):
 cfg=json.loads((root/f'reports/challenge-bound-{name}-cfg.json').read_text())
 for row in cfg['instructions']:
  address=int(row['address'],16);data=bytes.fromhex(row['bytes']);assert original['r'][address-0x08000000:address-0x08000000+len(data)]==data;original['allowed_thumb'][address]=data
frame_world_response_calls=[]
frame_gravity_calls=[]

frame_world_update_calls=[]

frame_actor_dispatch_calls=[]

frame_intro_gear_calls=[]

frame_countdown_calls=[]

frame_pause_calls=[]

frame_action_calls=[]



# The standalone controller comparator stopped before its real return epilogue.
for i in original['md'].disasm(original['r'][0x1146a:0x11490],0x0801146a):
 original['allowed_thumb'][i.address]=bytes(i.bytes)
 if i.mnemonic=='bx':break
frame_audio_calls=[]




guidance_calls=[]

calls=[];prior=p.external

def external(target):
 calls.append({'target':hex(target),'arguments':p.reg[:4].copy(),'sp':hex(p.reg[13])})
 if target in (0x08015b54,0x0801488c,0x08014968):
  args=p.reg[:4].copy();sp=p.reg[13];saved=p.reg[4:12].copy();start_response=p.steps;stores_before=len(p.writes)
  p.run(entry=target,stop=p.reg[14]&~1);assert p.reg[13]==sp and p.reg[4:12]==saved
  frame_world_response_calls.append({'entry':hex(target),'arguments':args,'instructions':p.steps-start_response,'stores':p.writes[stores_before:]})
 elif target==0x08015b00:
  args=p.reg[:4].copy();sp=p.reg[13];saved=p.reg[4:9].copy();start_gravity=p.steps;stores_before=len(p.writes);delta=p.read(0x030053c0,4)
  p.run(entry=target,stop=p.reg[14]&~1);assert p.reg[13]==sp and p.reg[4:9]==saved
  frame_gravity_calls.append({'arguments':args,'cached_delta':delta,'instructions':p.steps-start_gravity,'stores':p.writes[stores_before:]})
 elif target in (0x08029b44,0x0802a27c,0x0802aba0):execute_shared_ground(p,target)
 elif target==0x08011f80:
  sp=p.reg[13];saved=p.reg[4:11].copy();start_world=p.steps;stores_before=len(p.writes);args=p.reg[:4].copy()
  p.run(entry=target,stop=p.reg[14]&~1);assert p.reg[13]==sp and p.reg[4:11]==saved
  frame_world_update_calls.append({'arguments':args,'instructions':p.steps-start_world,'stores':p.writes[stores_before:]})
 elif target==0x08015ac8:
  sp=p.reg[13];saved=p.reg[4:7].copy();start_dispatch=p.steps
  p.run(entry=target,stop=p.reg[14]&~1);assert p.reg[13]==sp and p.reg[4:7]==saved
  frame_actor_dispatch_calls.append({'instructions':p.steps-start_dispatch,'count':p.read(0x020034d4,4)})
 elif target in (0x080130ac,0x08013610,0x080136d4):
  sp=p.reg[13];start_gear=p.steps;stores_before=len(p.writes);args=p.reg[:4].copy()
  p.run(entry=target,stop=p.reg[14]&~1);assert p.reg[13]==sp
  frame_intro_gear_calls.append({'entry':hex(target),'arguments':args,'instructions':p.steps-start_gear,'stores':p.writes[stores_before:]})
 elif target==0x0800ba04:
  sp=p.reg[13];saved=p.reg[4:10].copy();start_countdown=p.steps;stores_before=len(p.writes)
  p.run(entry=target,stop=p.reg[14]&~1)
  assert p.reg[13]==sp and p.reg[4:10]==saved
  frame_countdown_calls.append({'instructions':p.steps-start_countdown,'stores':p.writes[stores_before:]})
 elif target==0x08021c5c:
  sp=p.reg[13];saved=p.reg[4:7].copy();start_pause=p.steps;stores_before=len(p.writes);state_before=bytes(p.read(0x02005f18+i,1) for i in range(16))
  p.run(entry=target,stop=p.reg[14]&~1)
  assert p.reg[13]==sp and p.reg[4:7]==saved
  frame_pause_calls.append({'instructions':p.steps-start_pause,'stores':p.writes[stores_before:],'before_hex':state_before.hex(),'after_hex':bytes(p.read(0x02005f18+i,1) for i in range(16)).hex()})
 elif target==0x0800dec8:
  sp=p.reg[13];saved=p.reg[4:8].copy();start_recorded=p.steps;stores_before=len(p.writes)
  p.run(entry=target,stop=p.reg[14]&~1)
  assert p.reg[13]==sp and p.reg[4:8]==saved
  frame_recorded_calls.append({'instructions':p.steps-start_recorded,'stores':p.writes[stores_before:]})
 elif target==0x08015d98:
  sp=p.reg[13];saved=p.reg[4:6].copy();action=p.reg[0];start_action=p.steps;input_before=bytes(p.read(0x020034dc+i,1) for i in range(66))
  p.run(entry=target,stop=p.reg[14]&~1)
  assert p.reg[13]==sp and p.reg[4:6]==saved and bytes(p.read(0x020034dc+i,1) for i in range(66))==input_before
  frame_action_calls.append({'action':action,'result':p.reg[0],'instructions':p.steps-start_action,'input_sha256':hashlib.sha256(input_before).hexdigest()})
 elif target in (0x080276cc,0x080276dc):
  sp=p.reg[13];p.run(entry=target,stop=p.reg[14]&~1);assert p.reg[13]==sp
 elif target==0x080267c8:
  sp=p.reg[13];saved=p.reg[4:10].copy();p.run(entry=target,stop=p.reg[14]&~1)
  assert p.reg[13]==sp and p.reg[4:10]==saved
 elif target in (0x08028658,0x08028288):original_object_submit(p,target)
 elif target==0x08027fb8:original_sprite_resource(p)
 elif target in (0x08031a50,0x0802dbc8,0x0802dc20):original_shadow_packet(p,target)
 elif target==0x0803201c:original_depth_setter(p)
 elif target in (0x0801556c,0x0802dc78):original_taxi_renderer(p,target)
 elif target in (0x0802965c,0x080295a8):original_view_constructor(p,target)
 elif (target==0x0803244c and p.reg[0]==0x030002dc) or (target==0x08032450 and p.reg[1] in (0x03000020,0x030002c4,0x030002f4)):original_camera_vector_io(p,target)
 elif target==0x0803245c and p.reg[4] in (0x03000348,0x030003b0):original_camera_rotation(p)
 elif target==0x0802dd5c:original_angle_wrapper(p)
 elif target==0x08034094:original_angle_arm(p)
 elif target==0x08014dd8:original_camera_update(p)
 elif target==0x080208d4:original_effect_pool(p)
 elif target==0x08032458 and p.reg[3]==0x03000324:execute_original_vector(p)
 elif target==0x08032454 and p.reg[2]==0x03000480:execute_original_yaw_matrix(p)
 elif target==0x08032454:
  assert p.reg[2]==0x03002304,('audio quotient callback/bootstrap owner unqualified',hex(p.reg[2]),p.reg[:4],p.read(0x02007564,4))
  execute_original_arm_quotient(p)
 elif target==0x08032488:execute_original_division(p,target)
 elif target==0x08010b54:
  start_audio=p.steps;sp=p.reg[13];saved=p.reg[4:12].copy();stores_before=len(p.writes)
  before_audio={k:p.read(a,n) for k,(a,n,sgn) in frame_audio_ns['smap'].items()}
  p.run(entry=target,stop=p.reg[14]&~1)
  assert p.reg[13]==sp and p.reg[4:12]==saved
  after_audio={k:p.read(a,n) for k,(a,n,sgn) in frame_audio_ns['smap'].items()}
  frame_audio_calls.append({'instructions':p.steps-start_audio,'stores':p.writes[stores_before:],'before':before_audio,'after':after_audio})
 elif target==0x080206bc:
  start_guidance=p.steps;sp=p.reg[13];saved=p.reg[4:7].copy();stores_before=len(p.writes)
  p.run(entry=target,stop=p.reg[14]&~1)
  assert p.reg[13]==sp and p.reg[4:7]==saved
  guidance_calls.append({'instructions':p.steps-start_guidance,'stores':p.writes[stores_before:],'sp':hex(sp)})
 elif target==0x08008d60:p.run(entry=target,stop=p.reg[14]&~1)
 else:prior(target)
p.external=external;start=p.steps;error=None
try:p.run(entry=0x08007538,stop=0x080079d2)
except Exception as exc:error=repr(exc)
for base,region in p.regions.items():
 if base in (0x02000000,0x03000000):(root/f'reports/bound-first-frame-object-cmn-dependency-{base:08x}.bin').write_bytes(region)
report={'status':'FIRST_FRAME_DEPENDENCY' if error is not None else 'FIRST_FRAME_REACHED_END_REQUIRES_AUDIT','prefix_cases':len(rows),'trig_loader_calls':trig_loader_calls,'matrix_calls':matrix_calls,'vector_calls':vector_calls,'frame_pool_calls':frame_pool_calls,'frame_camera_calls':frame_camera_calls,'angle_calls':angle_calls,'rotation_calls':rotation_calls,'vector_io_calls':vector_io_calls,'frame_view_constructor_calls':frame_view_constructor_calls,'frame_taxi_renderer_calls':frame_taxi_renderer_calls,'frame_depth_setter_calls':frame_depth_setter_calls,'frame_shadow_packet_calls':frame_shadow_packet_calls,'frame_sprite_resource_calls':frame_sprite_resource_calls,'frame_object_submit_calls':frame_object_submit_calls,'sampled_fixture':sampled_fixture,'bootstrap_calls':bootstrap_calls,'frame_recorded_calls':frame_recorded_calls,'shared_ground_calls':shared_ground_calls,'frame_world_response_calls':frame_world_response_calls,'frame_gravity_calls':frame_gravity_calls,'frame_world_update_calls':frame_world_update_calls,'frame_actor_dispatch_calls':frame_actor_dispatch_calls,'frame_intro_gear_calls':frame_intro_gear_calls,'frame_countdown_calls':frame_countdown_calls,'frame_pause_calls':frame_pause_calls,'frame_action_calls':frame_action_calls,'frame_audio_calls':frame_audio_calls,'guidance_calls':guidance_calls,'frame_instructions':p.steps-start,'calls':calls,'registers':p.reg,'flags':p.flags,'error':error,'scope':'Actual7538 caller and8D60 reachable instructions resumed from second retained original743C prefix after source26488 audio bootstrap; first prefix establishes background callback owners, an explicit constructed ordering fixture; no fabricated helper fallback. Input sampler executes original idle KEYINPUT1023 fixture; active input and warm trajectory remain unqualified. Cached030053C0 world elapsed owner still constructed/unqualified prior presentation state; BIOS and hardware service contracts remain. Not complete frame/runtime/warm-gameplay proof.'}
(root/'reports/bound-first-frame-object-cmn-next-dependency.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],report['frame_instructions'],error,calls[-3:])
