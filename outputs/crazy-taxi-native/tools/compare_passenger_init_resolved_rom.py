"""Source ordinary eligible-customer reset22B6C..22C16. Route/ground resolved externally."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
entry=0x08022b6c;stop=0x08022ca4;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a==stop or a in seen:continue
 assert entry<=a<=0x08022c16,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
for lo,hi in [(0x08022c18,0x08022c28),(0x080229fe,0x08022a02)]:
 for i in md.disasm(r[lo-0x08000000:hi-0x08000000+4],lo):allowed_thumb[i.address]=bytes(i.bytes)
class PassengerProbe(InitProbe):
 def external(self,t):
  if t==0x0801e9d0:self.spawn_requests+=1
  elif t==0x08029b44:self.reg[0]=self.height&MASK
  elif t==0x08032488:self.reg[0]=trunc(signed(self.reg[0]),signed(self.reg[1]))&MASK if self.reg[1] else 0
  else:raise AssertionError(hex(t))
cases=[(n,m,seed,kind,h) for n,m in [(1,1),(1,2),(2,20),(10,100),(100,10),(255,256),(0,1),(1,0),(0,0)] for seed in [0,165,255] for kind in [0,9,10,255] for h in [-2147483648,19200,53760]]
rows=subprocess.run([str(root/'bin/passenger-init-resolved-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 n,m,seed,kind,h=case;got=[]
 for index,length in enumerate([n*256,m*256]):
  p=PassengerProbe((0,0,0,0,0,1,0,0,0,0));p.height=h;p.spawn_requests=0
  actor=0x10010000;source_actor=0x10011000;stack=0x10000400
  p.regions[actor]=bytearray((seed+j*37+index*13)&255 for j in range(44));p.regions[source_actor]=bytearray(32)
  if kind<=9:
   if length:p.write(actor,1,1)
   else:p.write(actor,p.read(actor,1)|2,1)
  p.write(source_actor+3,kind,1)
  p.reg[7]=actor;p.reg[8]=source_actor;p.reg[10]=length;p.reg[5]=64;p.reg[13]=stack
  p.write(stack+0x2c,min([x*256 for x in (n,m) if x] or [2147483647]));p.write(stack+0x34,((max(n,m)*256-min([x*256 for x in (n,m) if x] or [2147483647]))//5))
  if kind>9:
   p.reg[2]=kind;p.run(entry=0x080229fe,stop=0x08022a06);assert p.spawn_requests==1
  elif not length:p.run(entry=0x08022c18,stop=stop)
  else:p.run(entry=entry,stop=stop)
  steps+=p.steps;got+=list(p.regions[actor])
 assert got==list(map(int,row.split())),dict(case=case,original=got,native=row)
report=dict(cases=len(cases),thumb_instructions=steps,pass_match=True,scope='Original22B6C..22C16 eligible reset/category,22C18..22C28 ineligible reset,229FE..22A02 noncustomer spawn request; all44bytes for two records. Resolved route lengths/ground helper contract, not complete227AC route selection/frame service/sharedRNG or actor spawning.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/passenger-init-resolved-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
