"""Whole original10298 logical lifecycle versus native; device/audio/wait bodies explicit contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_save_defaults_rom.py').read().split('\nbinary=')[0])
for entry,end in [(0x8010298,0x8010482),(0x801e7f0,0x801e862)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<end,hex(a)
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic in branches:todo.append(i.operands[0].imm)
saved_allowed=allowed_thumb.copy();exec(source.replace('self.steps<50000','self.steps<200000').replace("'bne':not z","'bhs':c,'blo':not c,'bne':not z"));allowed_thumb.update(saved_allowed)
class SaveProbe(DefaultsProbe):
 run=MotionProbe.run
 def write(self,a,v,size=4):
  if a==0x40000dc and v==0x80000028 and self.read(0x40000d4)==0x2002058:
   InitProbe.write(self,a,v,size);assert self.read(0x40000d8)==0x20022b0
   self.regions[0x2000000][0x22b0:0x2300]=self.regions[0x2000000][0x2058:0x20a8]
  else:super().write(a,v,size)
 def external(self,target):
  if target in [0x800ffe0,0x801e784,0x801e7f0,0x8010a80]:self.run(entry=target,stop=self.reg[14]&~1);return
  assert target in [0x802763c,0x8027730,0x8027558,0x8016208,0x803203c,0x80320a4,0x8032228,0x80323b4,0x80105bc,0x8026630,0x8026ef8,0x8027110],hex(target)
  a=self.reg[0] if target in [0x8016208,0x803203c,0x80320a4,0x8032228,0x80323b4,0x80105bc,0x8026630,0x8026ef8,0x8027110] else 0
  b=self.reg[1] if target in [0x80320a4,0x80105bc] else 0
  self.calls.extend([target,a,b])
  if target==0x8016208:self.write(0x2003520,self.read(0x2003520)+a)
  elif target==0x8032228:
   assert self.reg[1]==0x2001eb0+a*8
   self.regions[0x2000000][0x1eb0+a*8:0x1eb8+a*8]=self.incoming[a*8:a*8+8]
  elif target==0x80323b4:self.reg[0]=1 if self.scenario==0 else 0x8000 if self.scenario==1 else (0x8000 if a==63 else 0) if self.scenario==2 else 0
binary=root/'bin/save-load-rom-compare.exe';capture=root/'reports/save-load-native.bin';baseline=root/'reports/save-load-defaults-baseline.bin';rom_path='Crazy Taxi - Catch a Ride (USA).gba'
subprocess.run([str(root/'bin/save-defaults-rom-compare.exe'),rom_path,str(baseline)],check=True);initial=baseline.read_bytes();results=[]
for scenario in range(4):
 for fallback in [0,1]:
  p=SaveProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x4000000:bytearray(1024)};p.scalars={};p.calls=[];p.scenario=scenario
  p.regions[0x2000000][0x1eb0:0x20b0]=initial[:512];p.regions[0x2000000][0x22b0:0x25d0]=initial[512:]
  incoming=bytearray(initial[:512]);incoming[0]=1;incoming[508:512]=sum(incoming[:504]).to_bytes(4,'little');p.incoming=bytes(512) if scenario<2 else bytes(incoming)
  for address,value,size in [(0x2007600,1,1),(0x2007602,1,1),(0x20075f4,32,1),(0x20075f5,64,1),(0x200388d,3,1),(0x2003520,15,4),(0x2003524,13,4),(0x2003b00,9,4)]:p.write(address,value,size)
  p.reg[:2]=[65537,fallback];p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x8010298,stop=0x10020000)
  assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
  subprocess.run([str(binary),rom_path,str(scenario),str(fallback),str(capture)],check=True)
  expected=bytes(p.regions[0x2000000][0x1eb0:0x20b0])+bytes(p.regions[0x2000000][0x22b0:0x25d0])+bytes(p.read(a,1) for a in [0x2001ea7,0x2007600,0x2007602,0x20075f4,0x20075f5,0x200388d])+b''.join(p.read(a).to_bytes(4,'little') for a in [0x2003520,0x2003524,0x2003b00,0x2003548])+p.read(0x4000204,2).to_bytes(2,'little')+len(p.calls).to_bytes(4,'little')+b''.join(v.to_bytes(4,'little') for v in p.calls)
  native=capture.read_bytes();assert native==expected,(scenario,fallback,next((i for i,(a,b) in enumerate(zip(native,expected)) if a!=b),None),len(native),len(expected))
  results.append({'scenario':scenario,'fallback':fallback,'Thumb_instructions':p.steps,'device_reads':p.calls.count(0x8032228),'service_calls':len(p.calls)//3})
report={'status':'PASS_LOGICAL_SAVE_LIFECYCLE','cases':results,'scope':'Wholeactual10298 callerreturn includingactualsettings snapshot/restore1E7F0/defaultsFFE0/fills/checksum10A80/logicalDMA80byte runtimecopy. Nativeallsettings512/runtime800/controlaudio-language6/clocks-callback-wait18 plusEXACTorderedservicearguments match. 64blocks/50retry limit,status0/1/8000, defaults-vsretained fallback, nonzeroacceptedchecksum andzeroinvalid, slotlow16. Actualsource8-byte blockdestinations checked for64sectors; returningdevice fixtures read8bytespersector. DeviceEEPROMserial/read/verify/write/platform stop/drain/audioreset/wait/musicservice bodies explicit returningcontracts; noactualPCsavebackend/asyncaudio/fullstartup/game proof. ROM/savepreserved; diagnostic captures only.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/save_load.c',root/'src/save_load.h',root/'src/save_block.c',root/'tests/save-load-rom-compare.c',binary]}}
(root/'reports/save-load-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS wholelogical save lifecycle',len(results),'cases',sum(q['Thumb_instructions'] for q in results),'Thumb')
