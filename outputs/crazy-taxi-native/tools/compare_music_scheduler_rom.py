"""Actual scheduler and post-dispatch tick finish; no tracker-body helper contracts needed."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for entry,limit,stops in [(0x8026918,0x8026a5c,{0x8026946,0x8026a5e}),(0x8026a30,0x8026a4c,{0x8026a5e})]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen or a in stops:continue
  assert entry<=a<=limit,hex(a)
  seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
cases=[(clock,255 if mask&1 else 0,128 if mask&2 else 0,1 if mask&4 else 0,clock,speed,delay) for clock in range(256) for mask in range(8) for speed,delay in [(0,0),(6,0),(255,0),(255,255),(1,255)]]
rows=subprocess.run([str(root/'bin/music-scheduler-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 clock,enabled,loaded,playing,phase,speed,delay=case;p=InitProbe((0,0,0,0,0,1,0,0,0,0));p.regions={};p.scalars={0x20075f9:clock,0x2007600:enabled,0x2007608:loaded,0x20075f6:playing,0x200760b:phase,0x2007609:speed,0x200760c:delay}
 p.reg[13]=0x10000400;p.run(entry=0x8026918,stop={0x8026946,0x8026a5e});got=[p.read(0x20075f9),int(p.stopped_pc==0x8026946)];steps+=p.steps
 p.run(entry=0x8026a30,stop=0x8026a5e);got+=[p.read(0x200760b),p.read(0x200760c)];steps+=p.steps
 assert got==list(map(int,row.split())),(case,got,row)
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Complete source26918..26946/26A58 skip/reset gate and26A30..26A4C post-dispatch finish. Every256divider/phase value with8nonzero gate combinations and5speed/delay boundaries incl fullinteger sum510 and bytewrap. Tracker body skipped explicitly; finish checked independently, not unconditional when music gated. No nativeproductionbehavior change or host pacing proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/music_tracker.c',root/'src/music_state.c',Path(__file__)]}}
(root/'reports/music-scheduler-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
