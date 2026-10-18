"""Whole original1E274 title parent with explicit returning child contracts."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_startup_obj_queue_rom.py').read().split('\nfixtures=')[0])
branches={'beq','bne','bhs','blo','bhi','bls','bge','blt','bgt','ble','bmi','bpl','bvs','bvc'}
todo=[0x801e274];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert 0x801e274<=a<=0x801e2d0,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic in branches:todo.append(i.operands[0].imm)
class ParentProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):
  assert target in [0x8016250,0x800de64,0x800abec]
  arg=self.reg[0] if target==0x8016250 else 0;self.events.append(f'{target}:{arg},');n=len(self.events)
  assert n<=64
  self.write(0x2003899,(self.seed+n*13)&255,1)
  self.write(0x2003898,0 if n>=self.limit else [255,128,1][n%3],1)
  self.reg[0]=0
cases=[(seed,limit,active,(seed^165)&255,(seed+57)&255,(seed*0x1000101)&MASK) for seed in [0,1,2,3,127,128,255,256] for limit in [1,2,3,4,5,8,13] for active in [0,128,255]]
binary=root/'bin/title-parent-rom-compare.exe';rows=subprocess.run([str(binary)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 seed,limit,active,flags,menu,callback=case;p=ParentProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2003800:bytearray(256)};p.scalars={};p.seed=seed;p.limit=limit;p.events=[]
 p.write(0x2003898,active,1);p.write(0x2003899,flags,1);p.write(0x2003897,menu,1);p.write(0x2003878,callback)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x801e274,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 parts=row.split();assert parts[0]==''.join(p.events),(case,p.events,parts[0])
 owners=[p.read(0x2003898,1),p.read(0x2003899,1),p.read(0x2003897,1),p.read(0x2003878)]
 assert owners==list(map(int,parts[1:])),(case,owners,parts[1:]);steps+=p.steps
report={'status':'PASS','cases':len(cases),'original_Thumb_instructions':steps,'scope':'Actualwhole1E274 title outerparent exact child-call order/state5 title,state0 intro replay orDE64 thenABEC attract, mutablegate3898/flags3899 and finalmenu3897/callback174DD/SP/savedregs. Child services explicitly returning contracts that mutate shared gates; no whole16250/DE64/ABEC/frame/RNG/audio/gameplay proof. Testcontracts terminate after1..13callbacks; native routine otherwise follows original indefinite loop.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [root/'src/title_parent.c',root/'src/title_parent.h',root/'tests/title-parent-rom-compare.c',binary,Path(__file__)]}}
(root/'reports/title-parent-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS native title outerparent',len(cases),'cases',steps,'Thumb')
