"""Whole271E0 restart rows with actual nested loader/start/RNG source routines."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_mixed_row_rom.py').read().split('\ncases=')[0])
for function in json.loads((root/'reports/restart-music-dependency-audit.json').read_text())['functions']:
 if function['entry'] in ('0x8026ef8','0x8027110'):
  for item in function['instructions']:allowed_thumb[int(item['address'],16)]=bytes.fromhex(item['bytes'])
for item in json.loads((root/'reports/random-range-cfg.json').read_text())['instructions']:allowed_thumb[int(item['address'],16)]=bytes.fromhex(item['bytes'])
saved_allowed=allowed_thumb.copy()
source=source.replace("   elif m=='bx':nextpc=val(ops[0])&~1", "   elif m in ['ldm','stm']:\n    base=rn(ops[0]);address=self.reg[base]\n    for operand in ops[1:]:\n     if m=='ldm':self.reg[rn(operand)]=self.read(address)\n     else:self.write(address,self.reg[rn(operand)])\n     address+=4\n    if '!' in ins.op_str.split(',')[0]:self.reg[base]=address&MASK\n   elif m=='bx':nextpc=val(ops[0])&~1")
exec(source);allowed_thumb.update(saved_allowed);NoteProbe.run=MotionProbe.run
class RestartRowsProbe(NoteProbe):
 def external(self,t):
  if t in [0x8026ef8,0x8027110,0x801e72c]:
   if t==0x801e72c:self.draws+=1
   target_stop=self.reg[14]&~1;self.run(entry=t,stop=target_stop)
  elif t==0x8032520:
   a,b=signed(self.reg[0]),signed(self.reg[1]);self.reg[0]=(a-trunc(a,b)*b if b else 0)&MASK
  else:super().external(t)
restart_rows=[(pattern,row) for pattern,offset in enumerate(pattern_offsets) for row in range(module[offset]) if any(module[offset+1+row*20+k*5+3]==11 for k in range(4))]
cases=[(pattern,row,seed,mode,choice,rng) for pattern,row in restart_rows for seed in [0,165] for mode,choice,rng in [(0,255,0),(0,255,0x7fffffff),(1,0,0),(1,4,0)]]
rows=subprocess.run([str(root/'bin/music-restart-mixed-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 pattern,row,seed,mode,choice,rng=case;p=RestartRowsProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([seed]*16),0x2007618:bytearray([seed]*512),pitch_address:bytearray(pitch)};p.scalars={sample_table+k*4:0x8421fc0+offset for k,offset in enumerate(samples)};p.scalars.update({0x200782c+k*4:0x8421fc0+offset for k,offset in enumerate(pattern_offsets)});p.scalars.update({rate_owner:21024,division_owner:0x80029fc,0x2007828:0x8421fc0,0x2001ea6:mode,0x2003874:int(choice<5),0x2003887:3,0x2003888:choice,0x20075f6:1,0x2007600:1,0x2003b04:rng});p.draws=0
 p.write(0x2007608,1,1);p.write(0x200760a,4,1);p.write(0x200760e,row,1);p.write(0x2007614,0x8421fc0+pattern_offsets[pattern])
 for k in range(16):p.write(0x2007618+k*32+28,0x8421fc0+samples[0])
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x80271e0,stop=0x10020000)
 parts=line.split();assert bytes(p.regions[0x2007608][:12])==bytes.fromhex(parts[0]),(case,'state',p.regions[0x2007608].hex(),parts[0]);assert p.read(0x2007614)-0x8421fc0==int(parts[1])
 for k in range(16):
  actual=bytes(p.regions[0x2007618][k*32:k*32+28]);assert actual==bytes.fromhex(parts[2+k*2]),(case,k,'voice',actual.hex(),parts[2+k*2]);pointer=p.read(0x2007618+k*32+28);offset=pointer-0x8421fc0 if pointer else 0xffffffff;assert offset==int(parts[3+k*2]),(case,k,'sample')
 assert [p.read(0x2003b04),p.draws,p.read(0x20075f6)]==list(map(int,parts[-3:])),(case,'RNG')
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'original_restart_rows':len(restart_rows),'source_instruction_observations':steps,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Whole original271E0/all4channel continuation with nested actual26EF8 moduleloader/27110 start/1E72C RNG, all16voice owners, controlled retainedfixtures, actual module rows containing11. ARM029FC actual, remainder32668/32520 contracts; no continuousfullIRQ/waveform/device proof.'}
(root/'reports/music-restart-mixed-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
