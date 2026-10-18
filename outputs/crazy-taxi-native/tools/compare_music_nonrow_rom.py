"""Whole274BC all4channels/return, retained states and volume boundaries."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_row_effects_rom.py').read().split('\ncases=')[0])
entry=0x80274bc;todo=[entry];seen=set()
while todo:
 a=todo.pop()
 if a in seen:continue
 assert entry<=a<=0x8027556,hex(a)
 seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
volumes=[0,1,63,64,65,32767,32768,65535]
cases=[(cmd,param,v) for cmd in [6,10] for param in range(256) for v in volumes]+[(cmd,param,v) for cmd in [0,11,12,255] for param in [0,255] for v in volumes]
rows=subprocess.run([str(root/'bin/music-nonrow-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 command,param,volume=case;p=RowProbe((0,0,0,0,0,1,0,0,0,0));events=bytearray([64,0,0,0,command,param,0,0,0,10,param^15,0,0,0,6,((param<<4)|(param>>4))&255,0,0,0,255,255]);p.regions={0x2007608:bytearray([165]*16),0x2007618:bytearray([165]*512),0x10011000:events};p.scalars={};p.write(0x200760a,4,1);p.write(0x200760e,0,1);p.write(0x2007614,0x10011000)
 for k,v in enumerate([volume,0,64,65535]):p.write(0x2007618+k*32+22,v,2)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 state,voices=line.split();assert bytes(p.regions[0x2007608][:12])==bytes.fromhex(state),(case,'state');assert bytes(p.regions[0x2007618])==bytes.fromhex(voices),(case,'voices');steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'WHOLE274BC entry/all4channels/return vs native shared eventloop through nonrow API. Commands6/10 ALLparameterbytes/8volume boundaries, mixedchannelcommands/parameterrotations/unsupportedcommands, all12trackerbytes and512voicebytes. Controlled diagnostic events/retainedowners; no audioIRQ/continuousmodule/waveform/device proof. No opaque helper calls.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/music_tracker.c',root/'src/music_tracker.h',root/'src/music_effects.c',Path(__file__)]}}
(root/'reports/music-nonrow-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
