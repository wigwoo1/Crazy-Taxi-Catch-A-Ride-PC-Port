"""Complete originalE0F4 destination material acceptance, no opaque helper."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
entry=0x0800e0f4;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x0800e152,hex(a);seen.add(a);i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in ['beq','bne','blt','ble','bgt','bge','blo','bhi','bls','bhs']:todo.append(i.operands[0].imm)
cases=[(selected,present,material,*slots) for selected in [-1,0,1,-2,2147483647] for present in [0,1] for material in [0,1,17,65534,65535,65536,0xffffffff] for slots in [(65535,65535,65535,65535),(17,0,1,65534),(0,17,65535,1),(65535,65535,17,65535),(65535,65535,65535,17)]]
rows=subprocess.run([str(root/'bin/destination-accept-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 selected,present,material,*slots=case;p=InitProbe((0,0,0,0,0,1,0,0,0,0));record=0x10010000
 p.regions[record]=bytearray(range(48));p.scalars[0x02001768]=selected&MASK;p.scalars[0x0200176c]=record if present else 0
 for j,v in enumerate(slots):p.write(record+40+j*2,v,2)
 prior=bytes(p.regions[record]);p.reg[0]=material;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000);steps+=p.steps
 assert p.reg[0]==int(row) and bytes(p.regions[record])==prior,case
report=dict(cases=len(cases),thumb_instructions=steps,pass_match=True,scope='Complete originalE0F4 vs native destination_accepts; all four material slots, sentinel/absent record/-1selection and full32bitmaterial boundaries; no opaque source helper. DF9C destination transform31860 and full227AC startup remain unqualified.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/destination-accept-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
