"""Original27262..272E0 note prefix, actual samples/pitch, explicit arithmetic helper contracts."""
from pathlib import Path
exec(open('outputs/crazy-taxi-native/tools/compare_music_sample_rom.py').read().split('\ncases=')[0])
def literal(a):
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));ptr=((a+4)&~3)+int(i.op_str.split('#')[1].split(']')[0],0);return int.from_bytes(r[ptr-0x8000000:ptr-0x8000000+4],'little')
pitch_address=literal(0x80272b4);rate_owner=literal(0x80272d4);division_owner=literal(0x80272a2)
pitch=(root/'assets/music/pitch-table.bin').read_bytes();assert len(pitch)==1536 and pitch_address==0x3006160 and pitch==r[0x49d780:0x49d780+1536]
saved_allowed=allowed_thumb.copy()
source=source.replace("a=val(ops[1]);n=val(ops[2]);value=", "a=val(ops[1]) if len(ops)==3 else self.reg[rn(ops[0])];n=val(ops[2]) if len(ops)==3 else (val(ops[1])&255);value=")
exec(source);allowed_thumb.update(saved_allowed);RowProbe.run=MotionProbe.run
division_namespace={}
exec(open('outputs/crazy-taxi-native/tools/compare_projection_divide_rom.py').read().split('\nvalues=')[0],division_namespace)
original_divide=division_namespace['original']
class NoteProbe(RowProbe):
 def external(self,t):
  assert t in [0x8032454,0x8032668],hex(t)
  assert self.reg[1]>0
  if t==0x8032454:
   assert self.reg[2]==0x80029fc
   self.reg[0]=original_divide(self.reg[0],self.reg[1])
  else:self.reg[0]=(self.reg[0]%self.reg[1])&MASK
cases=[(sample,note,rate,seed) for sample in range(len(samples)) for note in [0,1,12,48,96,127,128,129,255] for rate in [21024,32768] for seed in [0,165]]
rows=subprocess.run([str(root/'bin/music-note-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,line in zip(cases,rows):
 sample,note,rate,seed=case;p=NoteProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2007618:bytearray([seed]*32),0x10011000:bytearray([note,0,0,0,0]),pitch_address:bytearray(pitch)};p.scalars={rate_owner:rate,division_owner:0x80029fc}
 p.write(0x2007634,0x8421fc0+samples[sample]);p.reg[7]=0x2007618;p.reg[9]=0x10011000;p.reg[13]=0x10000400;p.run(entry=0x8027262,stop=0x80272e2)
 assert bytes(p.regions[0x2007618][:28])==bytes.fromhex(line),(case,p.regions[0x2007618].hex(),line);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'sample_count':len(samples),'nested_divider_ARM_instructions':division_namespace['steps'],'rom_sha256':hashlib.sha256(r).hexdigest(),'pitch_address':hex(pitch_address),'pitch_ROM_origin':'0x0849D780','rate_owner':hex(rate_owner),'scope':'Actual27262..272E0 entire note gate/transpose/tuning/period/frequency/step vs native event_prefix; ALL31actualsamples,9representativenotes incl0/128skip andwrap255,2diagnosticrates,2retainedvoicepatterns, all28nonpointervoicebytes exact. Pitch RAM fixture uses ROM0849D780 bytes, original26506 copy previously recovered. Each32454 invokes actual originalARM029FC signed divider in isolated offline execution; callback identity normalized to ROM address, bootstrap relocation not executed here. Remainder32668 remains explicit contract. All tested arguments positive/inrange; native ordinary unsigned arithmetic equivalent for this scope. No waveform/multichannel/device proof.','hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/music_note.c',root/'src/music_event.c',Path(__file__)]}}
(root/'reports/music-note-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
