"""Whole271E0 actual four-channel module rows, retained fixture owners, mainmode1."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_note_rom.py').read().split('\ncases=')[0])
pattern_offsets=[];off=0x106
for index in range(module[0]):pattern_offsets.append(off);off+=1+module[off]*module[4]*5
assert module[4]==4
cases=[(pattern,row,seed) for pattern,offset in enumerate(pattern_offsets) for row in range(module[offset]) for seed in [0,165]]
rows=subprocess.run([str(root/'bin/music-mixed-row-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0;commands=set()
for case,line in zip(cases,rows):
 pattern,row,seed=case;p=NoteProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray([seed]*16),0x2007618:bytearray([seed]*512),pitch_address:bytearray(pitch)};p.scalars={sample_table+k*4:0x8421fc0+offset for k,offset in enumerate(samples)};p.scalars.update({rate_owner:21024,division_owner:0x80029fc,0x2007828:0x8421fc0,0x2001ea6:1,0x2003874:0})
 p.write(0x200760a,4,1);p.write(0x200760e,row,1);p.write(0x2007614,0x8421fc0+pattern_offsets[pattern])
 for k in range(16):p.write(0x2007618+k*32+28,0x8421fc0+samples[0])
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x80271e0,stop=0x10020000)
 parts=line.split();assert bytes(p.regions[0x2007608][:12])==bytes.fromhex(parts[0]),(case,'state',p.regions[0x2007608].hex(),parts[0])
 assert p.read(0x2007614)-0x8421fc0==int(parts[1])
 for k in range(4):
  actual=bytes(p.regions[0x2007618][k*32:k*32+28]);assert actual==bytes.fromhex(parts[2+k*2]),(case,k,'voice',actual.hex(),parts[2+k*2])
  assert p.read(0x2007618+k*32+28)-0x8421fc0==int(parts[3+k*2]),(case,k,'sample')
  commands.add(module[pattern_offsets[pattern]+1+row*20+k*5+3])
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'nested_divider_ARM_instructions':division_namespace['steps'],'patterns':len(pattern_offsets),'commands':sorted(commands),'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'WHOLE271E0 entry/all4channels/return for EVERYoriginalmodulepattern row,2retainedbytefixtures,initialsample0 eachvoice,mainmode1/soundinactive. Exact12trackerbytes,pointeroffsets,28nonpointervoicebytes perchannel. ActualARM029FC nested,32668remainder contract. Not recovered continuousoriginaltrajectory, mainmode0 restart/all16voiceoutputs/mixer/waveform/IRQ/display proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/music_tracker.c',root/'src/music_tracker.h',Path(__file__)]}}
(root/'reports/music-mixed-row-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
