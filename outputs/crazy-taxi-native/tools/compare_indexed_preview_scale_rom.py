"""Complete original2C918 instructions, signed divide boundary contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
entry=0x0802c918;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x0802c9ee,hex(a);seen.add(a);i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in ['beq','bne','blt','ble','bgt','bge','blo','bls','bhi','bhs']:todo.append(i.operands[0].imm)
class ScaleProbe(InitProbe):
 def external(self,t):
  assert t==0x08032488,hex(t);self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK if self.reg[1] else 0
sizes=[(240,160,64,64),(240,160,63,63),(240,160,2,1),(1,1,64,64),(7,5,3,4),(5,3,7,9),(64,64,64,64)]+[(17,13,w,h) for w in range(1,10) for h in [1,2,3,7]]
cases=[(*size,seed) for size in sizes for seed in [0,165,255]]
rows=subprocess.run([str(root/'bin/indexed-preview-scale-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 sw,sh,dw,dh,seed=case;p=ScaleProbe((0,0,0,0,0,1,0,0,0,0));src=0x10010000;dst=0x10030000
 p.regions[src]=bytearray((seed+j*37+(j//sw)*11)&255 for j in range(sw*sh));p.regions[dst]=bytearray([165])*(dw*dh+16);before=bytes(p.regions[src])
 p.reg[:4]=[src,sw,sh,dst];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(p.reg[13],dw);p.write(p.reg[13]+4,dh)
 p.run(entry=entry,stop=0x10020000);steps+=p.steps
 assert list(p.regions[dst])==list(map(int,row.split())) and bytes(p.regions[src])==before,case
report=dict(cases=len(cases),thumb_instructions=steps,pass_match=True,scope='Complete2C918 vs native indexed16.16 scaling, all destination bytes plus16 untouched guard bytes/source retention. Ordinary240x160 to64x64, odd dimensions, shrink/enlarge, widths1..9 all unroll remainders. Signed32488 divide contract; native valid positive dimensions<=32767, invalid/zero/negative source paths unclaimed. Not complete31860 scene rendering.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/indexed-preview-scale-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
