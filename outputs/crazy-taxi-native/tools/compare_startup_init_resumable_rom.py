"""Native startup initialization with actual defaults/font/input bodies and explicit child services."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_init_rom.py').read().split('\ncases=')[0])
startup_allowed=allowed_thumb.copy()
exec(open('outputs/crazy-taxi-native/tools/compare_save_defaults_rom.py').read().split('\nbinary=')[0]);allowed_thumb.update(startup_allowed)
for i in md.disasm(r[0x16100:0x1612e],0x8016100):allowed_thumb[i.address]=bytes(i.bytes)
def packed_state(p):
 return bytes([p.read(a,1) for a in [0x2001ea5,0x2001ea4,0x2003892,selected]])+p.read(0x2003878).to_bytes(4,'little')+bytes(p.regions[0x2000000][font_owner-0x2000000:font_owner-0x2000000+24])+bytes(p.regions[0x2000000][0x1eb0:0x20b0])+bytes(p.regions[0x2000000][0x22b0:0x25d0])+bytes(p.regions[0x2000000][0x34dc:0x351e])+bytes([p.read(a,1) for a in [0x20075f4,0x20075f5,0x200388d]])
class OwnedProbe(DefaultsProbe):
 def external(self,target):
  if target in [0x800ffe0,0x8016100,0x8028f64,0x8028f84,0x8028fb8,0x8028fa0,0x801e784,0x8010a80]:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8016250:
   self.pending_snapshots.append(bytes([1])+packed_state(self))
   assert self.reg[0]==0;self.write(0x2001ea5,self.screen_gate,1)
  elif target==0x8010298:
   self.pending_snapshots.append(bytes([2])+packed_state(self))
   assert self.reg[:2]==[1,1];self.write(0x2001ea5,self.save_gate,1)
  else:raise AssertionError(('unexpectedstartupservice',hex(target)))
results=[];binary=root/'bin/startup-init-owned-rom-compare.exe';capture=root/'reports/startup-init-owned-native.bin';rom_path='Crazy Taxi - Catch a Ride (USA).gba'
for seed in [0,165,255]:
 for gate,screen,save in [(0,0,0),(0,1,255),(1,0,0),(255,0,0)]:
  p=OwnedProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray([seed])*262144,0x4000000:bytearray(1024)};p.scalars={};p.pending_snapshots=[];p.screen_gate=screen;p.save_gate=save;p.write(0x2001ea5,gate,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001
  p.run(entry=0x801e1f0,stop=0x801e24c)
  subprocess.run([str(binary),rom_path,str(seed),str(gate),str(screen),str(save),str(capture),"--resumable"],check=True);native=capture.read_bytes();assert len(native)==1413
  expected=bytes([p.read(a,1) for a in [0x2001ea5,0x2001ea4,0x2003892,selected]])+p.read(0x2003878).to_bytes(4,'little')+bytes(p.regions[0x2000000][font_owner-0x2000000:font_owner-0x2000000+24])+bytes(p.regions[0x2000000][0x1eb0:0x20b0])+bytes(p.regions[0x2000000][0x22b0:0x25d0])+bytes(p.regions[0x2000000][0x34dc:0x351e])+bytes([p.read(a,1) for a in [0x20075f4,0x20075f5,0x200388d]])
  assert Path(str(capture)+'.pending').read_bytes()==b''.join(p.pending_snapshots),(seed,gate,screen,save,'pending boundaries')
  assert native==expected,(seed,gate,screen,save,next((i for i,(a,b) in enumerate(zip(native,expected)) if a!=b),None))
  assert p.read(0x2003899,1)==seed
  results.append({'seed':seed,'entry_gate':gate,'screen_gate':screen,'save_gate':save,'pending_handoffs':len(p.pending_snapshots),'Thumb_instructions':p.steps})
report={'status':'PASS_NATIVE_RESUMABLE_STARTUP','cases':results,'compared_native_owned_bytes_per_case':1413,'scope':'Actual1E1F0 finiteprefixbeforepersistentdispatcher plusactualFFE0/defaults/fills/checksum/fontreset/register/select/16100inputreset. Native resumable begin/resume composition matches both completeownedstate atEACHsourcechildhandoff andfinalstate; invalidresumeafterdone rejected. Composition matches settings512/runtime800/input66/audio/language3/init/font32 bytes acrosscoldentry,screenmutatedgate,warmgate1/255. Screen16250(0) andsave10298(1,1) returning gatecontractsremainexplicit; nochildframe/savehardware/livestartup/wholepersistentparent claim. Nativecallerownedstate, noGBAexecutionruntime.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/startup_init.c',root/'src/startup_init_owned.c',root/'src/startup_init_owned.h',root/'src/input.c',root/'src/save_block.c',root/'tests/startup-init-owned-rom-compare.c',binary]}}
(root/'reports/startup-init-owned-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS native resumable startup and handoff states',len(results),'cases',sum(q['Thumb_instructions'] for q in results),'Thumb')
