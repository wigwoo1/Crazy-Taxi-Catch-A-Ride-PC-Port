"""Complete26318 source caller; frame/palette/input helper bodies explicit contracts."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
entry=0x8026318;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x8026420,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class ServiceProbe(InitProbe):
 def external(self,t):
  assert t in (0x8016208,0x8022508,0x8015c3c,0x800ee10,0x8027cd0,0x8028ee0),hex(t)
  self.scalars[0x2003520]=(self.scalars[0x2003520]+1)&0xffffffff;self.calls.extend([t,self.reg[0] if t in (0x8016208,0x8022508) else 0]);self.reg[0]=0
clocks=[(0,0),(1,0),(60,0),(61,0),(0,0xffffffff),(0xffffffff,0),(0x80000000,0),(0x7fffffff,0xffffffff)]
cases=[(pending,mode,bank,frozen,*clocks[(pending+bank)%len(clocks)]) for pending in range(256) for mode,bank,frozen in [(0,0,0),(1,255,1),(3,0,255),(3,255,0),(4,0,0),(4,1,1),(4,255,0),(255,128,0)]]
rows=subprocess.run([str(root/'bin/presentation-service-clock-order.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 pending,mode,bank,frozen,now,prior=case;p=ServiceProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.regions={}
 addresses=[0x2005f24,0x2007541,0x2007548,0x2005f20,0x2007540,0x4000000,0x200754c,0x2007550,0x2003520,0x2003524,0x2003b00,0x200755c,0x2001e10]
 values=[pending,mode,bank,frozen,255,0xa5a5,0x12345678,0xabcdef01,now,prior,77,0xfffffff0,0xfffffffe]
 p.scalars=dict(zip(addresses,values));p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 got=[p.read(a) for a in [0x2005f24,0x2007548,0x4000000,0x200754c,0x2007550,0x2003524,0x2003b00,0x200755c,0x2001e10,0x2007540]]+[len(p.calls)//2]+p.calls
 assert got==list(map(int,row.split())),(case,got,row);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole26318 source caller vs native presentation_service: every pending byte, display0/1/3/4/255, bank wrap/parity, signed-wrap clock clamp/freeze/counter wrap, ordered helper requests. Hardware display is a retained logical fixture. Helper bodies16208/22508/15C3C/EE10/27CD0/28EE0 explicit contracts, not audio/RNG/display pacing proof. Each helper callback increments counter3520 explicitly: source must sample counter after every helper, including copy flush. Contracts model elapsed time only, not actual helper cost. Viewer integration unfinished.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/presentation_service.c',root/'src/presentation_service.h',Path(__file__)]}}
(root/'reports/presentation-service-clock-order-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)

