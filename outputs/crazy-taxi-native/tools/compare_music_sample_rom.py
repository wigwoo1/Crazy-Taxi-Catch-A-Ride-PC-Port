"""Whole271E0 one-channel sample/volume prefix with zero note and no row command."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_row_effects_rom.py').read().split('\ncases=')[0])
module=(root/'assets/music/original-module.bin').read_bytes();assert module==r[0x421fc0:0x421fc0+181241]
off=0x106
for index in range(module[0]):off+=1+module[off]*module[4]*5
samples=[]
for index in range(module[3]):samples.append(off);off+=15+int.from_bytes(module[off:off+4],'little')
a=0x8027230;i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));literal=((a+4)&~3)+int(i.op_str.split('#')[1].split(']')[0],0);sample_table=int.from_bytes(r[literal-0x8000000:literal-0x8000000+4],'little')
cases=[(sample,volume,seed) for sample in range(len(samples)+1) for volume in [0,15,16,17,80,81,255] for seed in [0,165,255]]
rows=subprocess.run([str(root/'bin/music-sample-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 sample,volume,seed=case;p=RowProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007608:bytearray(16),0x2007618:bytearray([seed]*32),0x10011000:bytearray([0,0,sample,volume,0,0])};p.scalars={sample_table+index*4:0x8421fc0+offset for index,offset in enumerate(samples)}
 p.write(0x200760a,1,1);p.write(0x2007614,0x10011000);p.write(0x2007634,0x8421fc0+samples[0])
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=entry,stop=0x10020000)
 raw,offset=line.split();assert bytes(p.regions[0x2007618][:28])==bytes.fromhex(raw),(case,p.regions[0x2007618].hex(),raw)
 assert p.read(0x2007634)-0x8421fc0==int(offset),(case,offset);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'sample_count':len(samples),'rom_sha256':hashlib.sha256(r).hexdigest(),'sample_table_owner':hex(sample_table),'scope':'Whole271E0 entry/onechannel/return with every actual module sample identity plus retained sample, volume byte boundaries0/15/16/17/80/81/255 and3retainedvoicepatterns. Zero note/command diagnostic. All28nonpointer voicebytes and source samplepointer-to-native offset exact; no waveform/fullnote/multichannel/IRQ/device proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/music_event.c',Path(__file__)]}}
(root/'reports/music-sample-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
