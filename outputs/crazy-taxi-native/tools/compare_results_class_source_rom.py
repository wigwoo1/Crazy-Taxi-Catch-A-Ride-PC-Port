"""Original phase3 scale/hold branch, explicit stack boundary; offline only."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
import itertools,subprocess,json
for i in md.disasm(r[0x23eee:0x23f28],0x8023eee):allowed_thumb[i.address]=bytes(i.bytes)
cases=list(itertools.product((1,2,3,4,8,16,32,60),(-32768,-1,0,255,256,257,512,32767),(-32768,-1,0,18,19,20,32767)))
output=subprocess.run([str(root/'bin/results-class-source-probe.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,l.split())) for l in output.stdout.splitlines()];assert len(rows)==len(cases)
def s16(x):x=x&65535;return x if x<32768 else x-65536
steps=0
for case,native in zip(cases,rows):
 delta,scale,hold=case;p=InitProbe((0,0,0,0,0,1,0,0,0,0));sp=0x10000400
 p.scalars={0x2003b00:delta,sp+0x8c:scale&65535,sp+0x94:hold&65535};p.reg[13]=sp
 p.run(entry=0x8023eee,stop={0x8023f0e,0x8023f26,0x8023f28})
 transitioned=p.stopped_pc==0x8023f28
 expected=[s16(p.read(sp+0x8c)),0 if transitioned else s16(p.read(sp+0x94)),4 if transitioned else 3,2 if transitioned else 0]
 assert native==expected,(case,native,expected,hex(p.stopped_pc));steps+=p.steps
report={'status':'PASS_RESULTS_CLASS_BRANCH','cases':len(cases),'original_thumb_instructions':steps,'scope':'Actual original23EEE..23F28 class scale/hold branch vs native phase3 fields/events. Explicit frame3B00/stack8C/94 input contracts; thresholds and signed16 wrapping across1..60selectedsteps. Artwork/audio request after transition, full result dispatch, other phases and presentation excluded.'}
(root/'reports/results-class-source-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
