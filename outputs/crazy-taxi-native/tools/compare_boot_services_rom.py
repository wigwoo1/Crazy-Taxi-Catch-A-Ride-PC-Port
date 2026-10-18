"""Original boot timing and shade helper output ownership."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
branches={'beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl'}
for entry,end in [(0x8016140,0x801614e),(0x80161e0,0x80161ea),(0x800f69c,0x800f730)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<=end,hex(a)
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic in branches:todo.append(i.operands[0].imm)
class ServicesProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):
  if target==0x80161e0:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032488:
   assert self.reg[1]==31 and self.reg[0]<=961;self.reg[0]//=31;self.divides+=1
  else:raise AssertionError(('unexpectedboothelper',hex(target)))
results=[];steps=0;binary=root/'bin/boot-services-compare.exe'
for mode in [0,1]:
 for seed in [0,165,255]:
  p=ServicesProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray((seed+i*13)&255 for i in range(262144))};p.scalars={};p.divides=0;retained=p.read(0x2003899,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy()
  p.run(entry=0x800f69c if mode else 0x8016140,stop=0x10020000)
  assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
  assert p.divides==(1024 if mode else 0)
  capture=root/'reports/boot-services-native.bin';subprocess.run([str(binary),str(mode),str(seed),str(capture)],check=True)
  assert capture.read_bytes()==bytes(p.regions[0x2000000]);assert p.read(0x2003899,1)==retained
  results.append({'mode':mode,'seed':seed,'retained_parent_flag':retained,'division_contract_calls':p.divides});steps+=p.steps
report={'status':'PASS','cases':len(results),'original_Thumb_instructions':steps,'compared_bytes_per_case':262144,'results':results,'scope':'Actualwhole16140+161E0 initializes flags3528/callback352C=080161DD nohelpercontracts; actualwholeF69C produces32x32byte channeltable1A10 with32488unsigneddivide31 explicitarithmeticcontract. AllsurroundingexternalRAM including3899/SP/savedregs compared. No interrupt/VBlankhardware/service execution/wholeboot/rendererconfiguration/liveparent proof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [root/'src/boot_services.c',root/'src/boot_services.h',root/'tests/boot-services-compare.c',binary,Path(__file__)]}}
(root/'reports/boot-services-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS original boot service helpers',len(results),'cases',steps,'Thumb')
