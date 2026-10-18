"""Original per-descriptor reset and advance, with fade/stop service contracts."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for entry,ends,limit in [(0x8016390,[0x80163d4],0x80163d0),(0x8016660,[0x8016274,0x801668c],0x801668a)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in ends or a in seen:continue
  assert entry<=a<=limit,hex(a);seen.add(a)
  i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
base=0x10001000
owners=[(0x200359c,4),(0x2001771,1),(0x2001772,1),(0x2003898,1),(0x200755c,4),(0x2003570,4),(0x2003568,1),(0x200356a,1),(0x2003574,1),(0x200356c,2),(0x200356e,2)]
class SequenceProbe(InitProbe):
 def external(self,t):
  assert t in [0x800e5a8,0x802763c],hex(t)
  self.calls.append((t,self.reg[:4] if t==0x800e5a8 else [0]*4,(self.read(0x20035a0)-base)//28,self.reg[4]&65535));self.reg[0]=0
fixtures=[(0,0,0,0,0,0),(3,3,180,0x2001,0x2001,255),(65535,65535,0x7fffffff,65535,0,165),(255,255,0xffffffff,0,65535,12345)]
cases=[(op,index,slot,timer,bg,obj,image,typ,seed) for op in (0,1) for typ in range(256) for index,slot,timer,bg,obj,seed in fixtures for image in (0,0x808f208)]
rows=subprocess.run([str(root/'bin/screen-sequence-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 op,index,slot,timer,bg,obj,image,typ,seed=case;p=SequenceProbe((0,0,0,0,0,1,0,0,0,0));p.regions={a:bytearray(n) for a,n in owners};p.scalars={0x20035a0:base+slot*28};p.calls=[]
 for a,n in owners:p.write(a,seed,n)
 d=bytearray(28);struct.pack_into('<IIHH',d,0,image,timer,bg,obj);d[24]=typ
 p.regions[base+(slot+op)*28]=d;p.reg[10]=seed;p.reg[4]=index;p.reg[13]=0x10000400
 if op:p.write(p.reg[13]+0x10c,(index+1)&0xffffffff);stop=0x801668c if image==0 and typ==255 else 0x8016274
 else:stop=0x80163d4
 p.run(entry=0x8016660 if op else 0x8016390,stop=stop)
 call=p.calls[0];result=int(op and (image!=0 or typ!=255))
 got=[result,p.read(0x200359c),p.reg[10],p.read(0x2001771,1),p.read(0x2001772,1),p.read(0x2003898,1),(p.read(0x20035a0)-base)//28,p.reg[4]&65535,p.read(0x200755c),p.read(0x2003570),p.read(0x2003568,1),p.read(0x200356a,1),p.read(0x2003574,1),p.read(0x200356c,2),p.read(0x200356e,2),len(p.calls),call[0]]+call[1]+[call[2]]
 assert got==list(map(int,row.split())),(case,got,row);steps+=p.steps
report={'status':'PASS','cases':len(cases),'source_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Actual16390..63D4 per-descriptor reset and16660..668C descriptor/index advance. All256 type bytes, null/non-null image, retained seeds/timer/mask/index boundaries, exact retained fade/child bytes and stop-before-sentinel order. FadeE5A8 andstop2763C bodies contracts. Invoked advance assumes caller remaining<=0 andfadeinactive; no full setup/render/streaming/callback167D1/live viewer claim.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/screen_sequence.c',root/'src/screen_sequence.h',root/'tests/screen-sequence-rom-compare.c',Path(__file__),root/'bin/screen-sequence-rom-compare.exe']}}
(root/'reports/screen-sequence-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
