"""Original31902..31A26 destination preview border, full indexed owner."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
entry=0x08031902;stop=0x08031a26;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a==stop or a in seen:continue
 assert entry<=a<stop,hex(a);seen.add(a);i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in ['beq','bne','blt','ble','bgt','bge','blo','bls','bhi','bhs']:todo.append(i.operands[0].imm)
class BorderProbe(InitProbe):
 def external(self,t):
  assert t==0x0801e784,hex(t);self.run(entry=t,stop=self.reg[14]&~1)
cases=[(w,h,c,seed) for w in [2,3,4,5,16,63,64] for h in [1,2,3,4,5,16,63,64] for c in [0,111,255] for seed in [0,165,255]]
rows=subprocess.run([str(root/'bin/destination-preview-border-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 w,h,c,seed=case;p=BorderProbe((0,0,0,0,0,1,0,0,0,0));buffer=0x020358f0
 p.regions[buffer]=bytearray((seed+j*37)&255 for j in range(4096));p.regions[0x02001790]=bytearray(20);p.write(0x02001790+14,c,1);p.scalars[0x02001780]=1
 p.reg[7]=w;p.reg[5]=buffer;p.reg[13]=0x10000400;p.write(p.reg[13]+0x148,h)
 p.run(entry=entry,stop=stop);steps+=p.steps
 assert list(p.regions[buffer])==list(map(int,row.split())),case
report=dict(cases=len(cases),thumb_instructions=steps,pass_match=True,scope='Original31902..31A26 preview border and nested1E784 byte fill. All4096bytes retained/changed compared, odd/even widths2..64 and heights1..64. Full31860 scene rendering/graphics snapshot restoration still unqualified; width0/1 and height0 invalid native guard not source fatal-path claim.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/destination-preview-border-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
