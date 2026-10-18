from pathlib import Path
"""Original AC70 setup prefix with actual RNG; other helpers explicit contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_restart_rom.py').read().split('\ncases=')[0])
source=source.replace("['muls','ands','orrs']","['muls','ands','orrs','eors']").replace("a|b if m=='orrs' else a&b","a|b if m=='orrs' else (a^b if m=='eors' else a&b)")
saved_allowed=allowed_thumb.copy();exec(source);allowed_thumb.update(saved_allowed)
for item in json.loads((root/'reports/random-range-cfg.json').read_text())['instructions']:allowed_thumb[int(item['address'],16)]=bytes.fromhex(item['bytes'])
class RngRestartProbe(RestartProbe):
 run=MotionProbe.run
 def external(self,target):
  if target==0x8032520:
   a,b=map(signed,self.reg[:2]);self.reg[0]=(a-trunc(a,b)*b if b else 0)&MASK;return
  if target==0x801e72c:
   self.calls.append([target,self.reg[0],self.reg[1],*[self.read(a,1) for a in owners]])
   self.run(entry=target,stop=self.reg[14]&~1);return
  super().external(target)
cases=[(seed,*flags) for seed in [0,1,0x4652414e,0x3fffffff,0x40000000,0x7fffffff,0x80000000,0xffffffff] for flags in [(0,0,0),(1,255,128),(255,1,254)]]
binary=root/'bin/normal-restart-rng-rom-compare.exe';rows=subprocess.run([str(binary)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 p=RngRestartProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.reg[13]=0x10000400;p.write(0x2003b04,case[0])
 for address,value in zip(owners,case[1:]):p.write(address,value,1)
 p.run(entry=0x800ac70,stop=0x800acc6)
 actual=[len(p.calls),*[v for call in p.calls for v in call],*[p.read(a,1) for a in owners],p.read(0x2003b04)]
 assert actual==list(map(int,row.split())),(case,actual,row);steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'OriginalAC70..ACC6 with actual1E72C RNG, native sharedRNG/music order choice/call order/three owner bytes at every call/final. Signedremainder and all audio/resource/loading helper effects explicit contracts; no full gameplay setup/frame proof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/normal_restart.c',root/'src/random.c',binary]}}
(root/'reports/normal-restart-rng-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS native restart prefix with source RNG',len(cases),'cases',steps,'Thumb')
