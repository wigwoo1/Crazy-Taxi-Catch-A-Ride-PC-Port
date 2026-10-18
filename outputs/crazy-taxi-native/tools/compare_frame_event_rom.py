"""Whole16174 logical source event; audio/callback/handshake explicit contracts."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
entry=0x8016174;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x80161ba,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class EventProbe(InitProbe):
 def external(self,t):
  assert t in (0x8026888,0x803244c,0x80161f4),hex(t)
  self.calls.extend([t,self.reg[0] if t==0x803244c else 0]);self.reg[0]=0
cases=[(count,line,keep,setting,callback) for setting in range(256) for count,line,keep,callback in [(0,0,0,0),(0xffffffff,159,0,0x80161dd),(0x7fffffff,160,1,0),(0x80000000,65535,255,0x80161dd)]]
rows=subprocess.run([str(root/'bin/frame-event-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 count,line,keep,settings,callback=case;p=EventProbe((0,0,0,0,0,1,0,0,0,0));p.regions={};p.calls=[]
 p.scalars={0x2003520:count,0x2004308:0xa5a5,0x4000006:line,0x2001ea7:keep,0x2001eb0:settings,0x200352c:callback,0x3007ff8:0xa5a5}
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 got=[p.read(a) for a in [0x2003520,0x2004308,0x2001eb0,0x3007ff8]]+[len(p.calls)//2]+p.calls
 assert got==list(map(int,row.split())),(case,got,row);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole16174 native event caller: every settingsbyte, retain flag zero/nonzero, counter wrap, scanline boundaries, optional callback exact ordered audio26888/callback3244C/handshake161F4 requests. Actual interrupt dispatch and helper bodies excluded. Not yet integrated into native loading scheduler.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/frame_event.c',root/'src/frame_event.h',Path(__file__)]}}
(root/'reports/frame-event-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
