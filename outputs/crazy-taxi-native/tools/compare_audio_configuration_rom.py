"""Whole27584, explicit2776C wait and26704 apply/reset boundaries."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
for entry,limit in [(0x8027584,0x8027602)]:
 todo=[entry];seen=set()
 while todo:
  address=todo.pop()
  if address in seen:continue
  assert entry<=address<=limit,hex(address)
  seen.add(address);i=next(md.disasm(r[address-0x8000000:address-0x8000000+4],address,count=1));allowed_thumb[address]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(address+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class ConfigurationProbe(InitProbe):
 def external(self,target):
  if target==0x802776c:self.waits+=1
  elif target==0x8026704:self.applies+=1
  else:raise AssertionError(hex(target))
cases=[(selector,processing,effects) for selector in list(range(256))+[256,257,258,0xffffffff] for processing in [0,1,255] for effects in [0,1,128,255]]
rows=subprocess.run([str(root/'bin/audio-configuration-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 selector,processing,effects=case;p=ConfigurationProbe((0,0,0,0,0,1,0,0,0,0));p.regions={};p.scalars={0x2007560:processing,0x2007564:0xa5a5a5a5,0x2007568:0x12345678,0x2007602:effects,0x2007604:165};p.waits=p.applies=0;p.reg[0]=selector;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x8027584,stop=0x10020000)
 got=[p.read(a) for a in [0x2007564,0x2007568]]+[p.waits,p.applies];assert got==list(map(int,line.split())),(case,got,line);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_Thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'WHOLE27584 selector-byte handling/rate/count retention/conditional2776C wait and unconditional26704 apply/reset request. All256selectors and highword truncation, processing0/1/255 and4effects bytes. 2776C wait and26704 apply/reset bodies remain explicitcontracts, hardwareclock/mixer/live binding excluded.'}
(root/'reports/audio-configuration-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
