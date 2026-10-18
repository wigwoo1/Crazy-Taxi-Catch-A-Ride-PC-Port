"""Original rolling cash branch versus native phase1; valid cash indices only."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
import itertools,subprocess,json
for a,b in [(0x23df8,0x23e54),(0x23e60,0x23eb8)]:
 for i in md.disasm(r[a:b],0x8000000+a):allowed_thumb[i.address]=bytes(i.bytes)
h=struct.unpack_from('<H',r,0x23e12)[0];audio_address=struct.unpack_from('<I',r,((0x23e12+4)&~3)+(h&255)*4)[0]
cases=list(itertools.product((1,3,60),(0,2,32767),(0,1,30),(44,46,48,57),(0,48,56,57,255)))
cp=subprocess.run([str(root/'bin/results-cash-source-probe.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(cases)
def s16(v):v&=65535;return v if v<32768 else v-65536
steps=0
for case,native in zip(cases,rows):
 d,t,index,target,current=case;p=InitProbe((0,0,0,0,0,1,0,0,0,0));sp=0x10000400;p.reg[13]=sp;p.reg[9]=1;p.reg[10]=t&65535
 p.scalars={0x2003b00:d,sp+0x88:index,audio_address:0};p.write(audio_address,0,1)
 p.regions[sp+0x34]=bytearray(32);p.regions[sp+0x54]=bytearray(32)
 p.write(sp+0x34+index,target,1);p.write(sp+0x54+index,current,1)
 p.run(entry=0x8023df8,stop=0x802403e)
 expected=[s16(p.reg[10]),s16(p.read(sp+0x88)),p.reg[9],p.read(sp+0x54+index,1),int(s16(t+d)>2)]
 assert native==expected,(case,native,expected);steps+=p.steps
report={'status':'PASS_RESULTS_CASH_BRANCH','cases':len(cases),'original_thumb_instructions':steps,'scope':'Original phase1 rolling digits/punctuation vs native timer,digit,phase,selectedbyte and logical tick gate; explicit stack target/cash strings and audio-disabled fixture. Valid0/1/30 indices, digit init/already-equal/increment/wrap, punctuation, signed16 timer wrap. Full dispatch, formatting, audio requests, palette/rendering excluded.'}
(root/'reports/results-cash-source-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
