"""Original customer count and phase2/4 delay branches, explicit input contracts."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
import itertools,subprocess,json
for a,b in [(0x23d5e,0x23db6),(0x23dec,0x23df2),(0x23ebe,0x23ee2),(0x23f86,0x23fa8)]:
 for i in md.disasm(r[a:b],0x8000000+a):allowed_thumb[i.address]=bytes(i.bytes)
def literal(a):h=struct.unpack_from('<H',r,a)[0];return struct.unpack_from('<I',r,((a+4)&~3)+(h&255)*4)[0]
customer_address=literal(0x23d7c);audio_address=literal(0x23d86)
deltas=(1,2,3,4,8,16,32,60);timers=(-32768,-1,0,5,6,59,60,32767)
cases=[(0,d,t,c,target) for d,t,c,target in itertools.product(deltas,timers,(0,4,255),(-1,0,1,5))]
cases += [(phase,d,t,0,0) for phase,d,t in itertools.product((2,4),deltas,timers)]
cp=subprocess.run([str(root/'bin/results-timer-source-probe.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(cases)
def s16(v):v&=65535;return v if v<32768 else v-65536
steps=0
for case,native in zip(cases,rows):
 phase,d,t,current,target=case;p=InitProbe((0,0,0,0,0,1,0,0,0,0));sp=0x10000400;p.reg[13]=sp;p.reg[9]=phase;p.reg[10]=t&65535
 p.scalars={0x2003b00:d,sp+0x80:current,customer_address:target&255,audio_address:0};p.write(customer_address,target&255,1);p.write(audio_address,0,1)
 p.run(entry={0:0x8023d5e,2:0x8023ebe,4:0x8023f86}[phase],stop=0x802403e)
 event=int(phase==0 and s16(p.reg[10])==0 and s16(t+d)>6 and target!=0)
 expected=[s16(p.reg[10]),p.read(sp+0x80),p.reg[9],event]
 assert native==expected,(case,native,expected);steps+=p.steps
report={'status':'PASS_RESULTS_CUSTOMER_AND_DELAY_BRANCHES','cases':len(cases),'original_thumb_instructions':steps,'scope':'Original phase0 customer timer/increment and phase2/4 delay transitions vs native fields/logical tick event. Explicit frame/stack/signed customer contracts, audio disabled fixture; no actual audio event comparison. Entire dispatch, cash phase, rendering and palette excluded.'}
(root/'reports/results-timer-source-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
