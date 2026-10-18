"""Whole original cold boot1E2E4 with bounded source renderer and platform DMA contracts."""
exec(open('outputs/crazy-taxi-native/tools/audit_graphics_bootstrap_cold_source.py').read().split('\np=BootstrapProbe')[0])
for entry,end in [(0x801e2e4,0x801e3e8),(0x8016140,0x8016150),(0x80161e0,0x80161ec),(0x800f69c,0x800f732),(0x8010a54,0x8010a6a)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<end,(hex(a),hex(entry))
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic in branches:todo.append(i.operands[0].imm)
class ColdBootProbe(BootstrapProbe):
 def external(self,target):
  if target in [0x802ac6c,0x8016140,0x80161e0,0x800f69c]:
   self.calls.append((target,self.reg[:4].copy()));self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8032488 and self.reg[1]==31:
   assert self.reg[0]<=961;self.reg[0]//=31;self.shade_divides+=1
  else:super().external(target)
 def write(self,a,v,size=4):
  if a!=0x40000dc or v==0x84001257:return super().write(a,v,size)
  InitProbe.write(self,a,v,size)
  src,dst=self.read(0x40000d4),self.read(0x40000d8)
  if v in [0x85010000,0x85001f80]:
   assert self.read(src)==0
   count=((v&65535) or 65536)*4;self.regions[dst][:count]=bytes(count);self.dma.append({'kind':'fixed-source-zero','destination':f'{dst:08X}','bytes':count})
  elif v==0x84000043:
   assert (src,dst)==(0x80005f4,0x30052b0)
   self.regions[0x3000000][0x52b0:0x53bc]=r[0x5f4:0x700];self.dma.append({'kind':'fixed1024-copy-code-install','destination':f'{dst:08X}','bytes':268})
  else:
   assert (v,src,dst)==(0x8000001c,0x849d528,0x2003530)
   self.regions[0x2000000][0x3530:0x35a0]=r[0x49d528:0x49d598];self.dma.append({'kind':'ROM-descriptor-copy','destination':f'{dst:08X}','bytes':112})
results=[]
for seed in [0,165,255]:
 p=ColdBootProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray([seed])*262144,0x3000000:bytearray([seed])*32768,0x4000000:bytearray(1024)};p.scalars={};p.stores=[];p.published_copy=False;p.calls=[];p.contracts=[];p.dma=[];p.shade_divides=0
 p.write(0x2001ea5,0,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x801e2e4,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 before_external=bytes(p.regions[0x2000000]);before_internal=bytes(p.regions[0x3000000]);p.reg[14]=0x10020001;p.run(entry=0x8010a54,stop=0x10020000)
 assert bytes(p.regions[0x2000000])==before_external
 expected_internal=bytearray(before_internal);expected_internal[0x52b0:0x53bc]=r[0x5f4:0x700];expected_internal[0x4c40:0x4c44]=(0x30052b0).to_bytes(4,'little')
 assert p.regions[0x3000000]==expected_internal and p.reg[13]==0x10000400 and p.reg[4:12]==saved
 assert p.read(0x2003899,1)==0 and p.read(0x202c4ec)==0 and p.shade_divides==1024
 assert p.read(0x200352c)==0x80161dd and p.read(0x2003528)&1==0
 assert p.read(0x2007564)==0x5220 and p.read(0x2007568)==352
 assert p.regions[0x2000000][0x3530:0x35a0]==r[0x49d528:0x49d598]
 assert p.regions[0x2000000][0x1a10:0x1e10]==bytes(row*col//31 for row in range(32) for col in range(32))
 assert p.regions[0x3000000][0x7e00:]==bytes([seed])*512
 expected_registers={0x204:0x4014,0x208:1,0x200:0x2001,4:8,0x20:256,0x22:0,0x24:0,0x26:256,0x30:256,0x32:0,0x34:0,0x36:256}
 for offset,value in expected_registers.items():assert p.read(0x4000000+offset,2)==value,(hex(offset),p.read(0x4000000+offset,2),value)
 results.append({'seed':seed,'Thumb_instructions':p.steps,'parent_flag_after':p.read(0x2003899,1),'DMA':p.dma,'shade_divides':p.shade_divides})
report={'status':'PASS_COLD_BOOT_AND_COPY_INSTALL','cases':results,'ARM_quotient_instructions':arm['steps'],'scope':'Wholeactual1E2E4 callerreturn followedbywhole10A54 install/callerreturn; ALLexternalRAM retainedby10A54 andALLinternalRAM matchedonly268codebytes030052B0 pluscallbackslot03004C40=030052B0. Sourcecopiedroutine080005F4 is fixed1024-byteforward32-bytechunkcopy (32 LDM/STM pairs), notsave-only. Nativeproductionmustbindcompiledcopy, neverexecutecopiedcode.  allnestedcoldrenderer/registration/frame/trig/timing/shade routines execute. Logicalfixed-source DMAclear andrawROM copies; signeddivide192 andunsigneddivide31 contracts; originalROM ARM029FC quotientresult withvolatile scratchpoison. DiagnosticstackoutsideIWRAM, so top512 DMAretention excludesactualGBAstack traffic. Coldflag1EA5=0 only; final3899=0 proven on this bootpath. Notwarmboot/5404 later initialization/liveparent/fullgame/nativeproductionboot proof. CopiedARMneverexecuted.','rom_sha256':hashlib.sha256(r).hexdigest(),'verifier_sha256':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
(root/'reports/boot-whole-cold-source-audit.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS wholeoriginal cold boot and copy-helper install',len(results),'cases',sum(q['Thumb_instructions'] for q in results),'Thumb')
