"""Original attract ABEC prefix to AC62, actual RNG with remainder contract."""
exec(open('outputs/crazy-taxi-native/tools/compare_title_parent_rom.py').read().split('class ParentProbe')[0])
source=source.replace("['muls','ands','orrs']","['muls','ands','orrs','eors']").replace("a|b if m=='orrs' else a&b","a|b if m=='orrs' else (a^b if m=='eors' else a&b)")
saved_allowed=allowed_thumb.copy();exec(source);allowed_thumb.update(saved_allowed)
for start,end in [(0xabec,0xac06),(0xac38,0xac62)]:
 for i in md.disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
for i in json.loads((root/'reports/random-range-cfg.json').read_text())['instructions']:allowed_thumb[int(i['address'],16)]=bytes.fromhex(i['bytes'])
class EntryProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):
  if target==0x801e72c:self.run(entry=target,stop=self.reg[14]&~1);return
  assert target==0x8032520
  a,b=map(signed,self.reg[:2]);self.reg[0]=(a% b if a>=0 and b>0 else a-trunc(a,b)*b if b else 0)&MASK
cases=[(seed,fill) for seed in [0,1,0x4652414e,0x3fffffff,0x40000000,0x7fffffff,0x80000000,0xffffffff] for fill in [0,1,31,32,127,255]]
binary=root/'bin/attract-entry-rom-compare.exe';rows=subprocess.run([str(binary)],input=''.join(f'{s} {f}\n' for s,f in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for (seed,fill),row in zip(cases,rows):
 p=EntryProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768)};p.scalars={};ram=p.regions[0x2000000]
 ram[0x1708]=1;ram[0x1eb0:0x1eb8]=bytes((fill+j*19)&255 for j in range(8));ram[0x153c]=ram[0x1697]=165;p.write(0x2003b04,seed)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800abec,stop=0x800ac62)
 parts=row.split();assert bytes.fromhex(parts[0])==bytes(ram[0x1eb0:0x1eb8]);assert list(map(int,parts[1:]))==[ram[0x153c],ram[0x1697],p.read(p.reg[13]+32),p.read(0x3004ca0),p.read(0x2003b04)]
 assert p.reg[13]==0x10000400-72;steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'ActualABEC attract-only prefix toAC62 with actual1E72C RNG; signedremainder32520 explicit contract. Settings8/twoflags/savedcitystack/native sceneidentity/sharedRNG match. Common AC62 onward/gameplay not executed.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/attract_entry.c',binary]}}
(root/'reports/attract-entry-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS attract entry prefix',len(cases),'cases',steps,'Thumb')
