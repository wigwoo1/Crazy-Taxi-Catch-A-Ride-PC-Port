"""Whole originalFFE0 including RAM clear and checksum; all external RAM retained effects."""
exec(open('outputs/crazy-taxi-native/tools/audit_graphics_bootstrap_real_allocation.py').read().split('\np=RealBindingsProbe')[0])
for entry,end in [(0x800ffe0,0x80100e6),(0x801e784,0x801e7d2),(0x8010a80,0x8010aa2)]:
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
saved_allowed=allowed_thumb.copy();exec(source.replace("'bne':not z","'bhs':c,'blo':not c,'bne':not z"));allowed_thumb.update(saved_allowed)
class DefaultsProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):
  assert target in [0x801e784,0x8010a80],hex(target)
  self.run(entry=target,stop=self.reg[14]&~1)
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   src,dst=self.read(0x40000d4),self.read(0x40000d8)
   assert v==0x80000028 and src==0x84a54c4 and dst in [0x2002058,0x20022b0]
   off=dst-0x2000000;self.regions[0x2000000][off:off+80]=r[0x4a54c4:0x4a5514]
binary=root/'bin/save-defaults-rom-compare.exe';capture=root/'reports/save-defaults-native.bin'
subprocess.run([str(binary),'Crazy Taxi - Catch a Ride (USA).gba',str(capture)],check=True);native=capture.read_bytes();assert len(native)==1312
results=[]
for seed in [0,165,255]:
 p=DefaultsProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray((seed+i*13)&255 for i in range(262144)),0x4000000:bytearray(1024)};p.scalars={};before=bytes(p.regions[0x2000000]);p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy()
 p.run(entry=0x800ffe0,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 expected=bytearray(before);expected[0x1eb0:0x20b0]=native[:512];expected[0x22b0:0x25d0]=native[512:];expected[0x75f4]=32;expected[0x75f5]=64;expected[0x388d]=0
 assert p.regions[0x2000000]==expected
 assert p.read(0x2003899,1)==before[0x3899]
 results.append({'seed':seed,'Thumb_instructions':p.steps,'retained_parent_flag':p.read(0x2003899,1)})
report={'status':'PASS','cases':results,'compared_external_bytes_per_case':262144,'scope':'ActualwholeFFE0 withactual1E784 RAM fills/actual10A80 checksum, logical80byte ROMDMA copies tosettings andruntime. Allnative512settings+800runtimebytes plusaudio volumes32/64 andlanguage388Dzero match; allsurroundingexternalRAMincluding3899 preserved. ROM084A54C4 initialtenrecords. Native save_block_defaults primitive intentionallydoesnotpublishexternalaudio/languageowners; callercompositionrequired. Notdevice persistence/whole1E1F0/screens/livehost proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/save_block.c',root/'src/save_block.h',root/'tests/save-defaults-rom-compare.c',binary]}}
(root/'reports/save-defaults-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS wholedefaults',len(results),'cases',sum(q['Thumb_instructions'] for q in results),'Thumb allRAM/native1312bytes')
