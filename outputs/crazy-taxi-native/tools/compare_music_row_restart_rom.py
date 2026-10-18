"""Actual2697E..26A1C row completion and conditional restart; helper contracts explicit."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
module=(root/'assets/music/original-module.bin').read_bytes();assert module==r[0x421fc0:0x421fc0+181241]
offsets=[];off=0x106
for index in range(module[0]):offsets.append(off);off+=1+module[off]*module[4]*5
entry=0x802697e;stop=0x8026a30;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a==stop or a in seen:continue
 assert entry<=a<=0x8026a1c,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
class RestartProbe(InitProbe):
 def external(self,t):
  self.calls.append(t)
  if t==0x801e72c:
   assert self.reg[:2]==[0,65535];self.draws+=1;self.reg[0]=self.random
  elif t==0x8027730:pass
  elif t==0x8026ef8:assert self.reg[0]==0x8421fc0
  elif t==0x8027110:
   order=self.reg[0];assert order in [63,88,115,34,5]
   self.write(0x2007609,module[5],1);self.write(0x200760a,module[4],1)
   for n in [3,4,6]:self.write(0x2007608+n,0,1)
   self.write(0x200760d,order,1);self.write(0x2007610,65535,2);self.write(0x2007612,65535,2)
   self.write(0x2007614,0x8421fc0+offsets[module[6+order]])
  else:raise AssertionError(hex(t))
orders=[0,module[1]-1]
cases=[(order,row,pending,mode,value) for order in orders for row in [0,module[offsets[module[6+order]]]-1,255] for pending in [0,1,32767,32768,65535] for mode in [0,1,255] for value in [0,1,2,3,65535]]
rows=subprocess.run([str(root/'bin/music-row-restart-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0;restarts=0
for case,line in zip(cases,rows):
 order,row,pending,mode,value=case;p=RestartProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([165]*16)};p.calls=[];p.draws=0;p.random=value;p.scalars={0x2007828:0x8421fc0,0x2001ea6:mode}
 p.write(0x200760d,order,1);p.write(0x200760e,row,1);p.write(0x2007612,pending,2);initial_offset=offsets[module[6+order]];p.write(0x2007614,0x8421fc0+initial_offset)
 p.reg[4]=0x2007608;p.reg[5]=0;p.reg[13]=0x10000400;p.run(entry=entry,stop=stop)
 raw,offset,draws=line.split();got=bytes(p.regions[0x2007608][:12]);assert got[1:]==bytes.fromhex(raw)[1:],(case,got.hex(),raw)
 assert p.read(0x2007614)-0x8421fc0==int(offset) and p.draws==int(draws),(case,offset,draws)
 if p.calls:
  assert p.calls==([0x801e72c] if mode==0 else [])+[0x8027730,0x8026ef8,0x8027110];restarts+=1
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'restart_cases':restarts,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Actual2697E..26A1C conditional row completion versus existing tracker completion/refactor. Original module first/lastorders, rows0/last/255, pending signedhalfword boundaries, mainmode0/1/255 and RNGvalues0/1/2/3/FFFF. Statebytes1..11, patternoffset and conditional RNGdraw exact. Native byte0 not compared: loader-loaded flag has separate native representation. RNG algorithm/stop/moduleload/start/preceding rowcallback are explicit contracts; no full audioIRQ or loading stream proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/music_tracker.c',root/'src/music_tracker.h',Path(__file__)]}}
(root/'reports/music-row-restart-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
