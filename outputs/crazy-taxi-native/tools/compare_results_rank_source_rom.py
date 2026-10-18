"""Execute original rank scale and final timer branches offline."""
exec(open('outputs/crazy-taxi-native/tools/compare_session_init_rom.py').read().split('\ncases=')[0])
import itertools,subprocess,json
for a,b in [(0x23fb2,0x23fd8),(0x23ffc,0x2402e)]:
 for i in md.disasm(r[a:b],0x8000000+a):allowed_thumb[i.address]=bytes(i.bytes)
h=struct.unpack_from('<H',r,0x23ffc)[0];literal=((0x23ffc+4)&~3)+(h&255)*4;target_address=struct.unpack_from('<I',r,literal)[0]
cases=[(5,d,s,99,0) for d,s in itertools.product((1,2,3,4,8,16,32,60),(-32768,-1,0,255,256,257,512,32767))]
cases += [(6,d,t,k,target) for d,t,k,target in itertools.product((1,2,3,4,8,16,32,60),(-32768,-1,0,299,300,301,32767),(0,1,99,255),(-1,0,1,99))]
cp=subprocess.run([str(root/'bin/results-rank-source-probe.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,l.split())) for l in cp.stdout.splitlines()];assert len(rows)==len(cases)
def s16(v):v&=65535;return v if v<32768 else v-65536
steps=0
for case,native in zip(cases,rows):
 phase,d,value,rank,target=case;p=InitProbe((0,0,0,0,0,1,0,0,0,0));sp=0x10000400;p.reg[13]=sp;p.reg[9]=phase;p.reg[10]=value&65535
 p.scalars={0x2003b00:d,sp+0x90:value&65535,sp+0x84:rank,target_address:target&255}
 p.write(target_address,target&255,1)
 if phase==5:
  p.run(entry=0x8023fb2,stop={0x802403e,0x8023fd8})
  transitioned=p.stopped_pc==0x8023fd8
  expected=[s16(p.read(sp+0x90)),rank,6 if transitioned else 5,4 if transitioned else 0,0]
 else:
  p.run(entry=0x8023ffc,stop={0x802403e,0x802402e})
  exited=p.stopped_pc==0x802402e
  expected=[s16(p.reg[10]),p.read(sp+0x84),6,8 if exited else 0,int(exited)]
 assert native==expected,(case,native,expected,hex(p.stopped_pc));steps+=p.steps
report={'status':'PASS_RESULTS_RANK_BRANCHES','cases':len(cases),'original_thumb_instructions':steps,'target_owner':f'{target_address:08X}','scope':'Actual23FB2..23FD8 scale transition and23FFC..2402E final rank decrement/timer branch vs native phase5/6. Explicit stack/frame/target contracts; signed16 wrapping and signed target/unsigned rank edge cases. Audio/art/fade requests at boundaries and whole results dispatch excluded.'}
(root/'reports/results-rank-source-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
