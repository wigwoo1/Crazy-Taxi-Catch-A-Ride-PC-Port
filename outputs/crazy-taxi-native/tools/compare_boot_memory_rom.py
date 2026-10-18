"""Original boot RAM prefix1E2E4..1E33E, DMA fill contracts explicit."""
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
branches={'beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl'}
todo=[0x801e2e4];seen=set()
while todo:
 a=todo.pop()
 if a==0x801e33e or a in seen:continue
 assert 0x801e2e4<=a<0x801e33e,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in branches:todo.append(i.operands[0].imm)
class BootProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):raise AssertionError(('unexpectedboothelper',hex(target)))
 def write(self,a,v,size=4):
  super().write(a,v,size)
  if a==0x40000dc:
   assert v in [0x85010000,0x85001f80]
   src,dst=self.read(0x40000d4),self.read(0x40000d8);assert self.read(src)==0
   count=((v&65535) or 65536)*4;region=self.regions[dst];region[:count]=bytes(count);self.fills.append([dst,count])
results=[];steps=0;binary=root/'bin/boot-memory-compare.exe'
for flag in [0,1,128,255]:
 for seed in [0,165,255]:
  p=BootProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray((seed+i*13)&255 for i in range(262144)),0x3000000:bytearray((seed+i*19)&255 for i in range(32768)),0x4000000:bytearray(1024)};p.scalars={};p.fills=[];p.write(0x2001ea5,flag,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001
  p.run(entry=0x801e2e4,stop=0x801e33e)
  assert p.reg[13]==0x10000400-24
  assert p.fills==([[0x2000000,262144],[0x3000000,32256]] if not flag else []),p.fills
  expected=bytes(p.regions[0x2000000])+bytes(p.regions[0x3000000])+struct.pack('<H',p.read(0x4000204,2))
  capture=root/'reports/boot-memory-native.bin';subprocess.run([str(binary),str(flag),str(seed),str(capture)],check=True)
  assert capture.read_bytes()==expected,(flag,seed)
  results.append({'flag':flag,'seed':seed,'routing_flag':p.read(0x2003899,1)});steps+=p.steps
report={'status':'PASS','cases':len(results),'original_Thumb_instructions':steps,'compared_bytes_per_case':294914,'results':results,'scope':'Actual1E2E4..1E33E prefix beforefirsthelper2DB54. DMA85010000/85001F80 logical fixed-source zero fill contracts; exactexternal262144/internal32768bytes includingretainedtop512 andWAITCNT4014 compared. Coldflag1EA5zero clears3899; warm retains3899 andclears1EAA only. No laterwholeboot/reset/frame/RNG/audio/liveparent proof. PrefixSPdelta24checked, notwholefunctionreturn.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [root/'src/boot_memory.c',root/'src/boot_memory.h',root/'tests/boot-memory-compare.c',binary,Path(__file__)]}}
(root/'reports/boot-memory-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS original boot RAM prefix',len(results),'cases',steps,'Thumb')
