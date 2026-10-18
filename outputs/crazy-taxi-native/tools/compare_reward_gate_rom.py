"""OriginalAFD4 reward-update dispatch prefix: gate proof only."""
exec(open('outputs/crazy-taxi-native/tools/compare_reward_update_rom.py').read().split('\nimport itertools')[0])
import itertools
for i in md.disasm(r[0xafd4:0xb00a],0x800afd4):allowed_thumb[i.address]=i.bytes
class GateProbe(UpdateProbe):
 def external(self,target):
  assert target==0x8029264,hex(target);self.dispatch+=1
cases=list(itertools.product([0,1,128],[0,1,128],[0,1,128],[0,1]))
res=subprocess.run([str(root/'bin/reward-gate-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=list(map(int,res.stdout.split()));steps=0
for case,want in zip(cases,rows):
 p=GateProbe((0,0,0,0,0,1,0,0,0,0));p.dispatch=0;p.reg[9]=0x2001708
 for a,v in zip([0x2005f20,0x2001696,0x2001697,0x2001708],case):p.write(a,v,1)
 p.run(entry=0x800afd4,stop={0x800b27c,0x800b284,0x800b00a})
 assert p.dispatch==want,(case,p.dispatch,want);steps+=p.steps
report=dict(cases=len(cases),thumb_instructions=steps,match=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='OriginalAFD4..B006 dispatch prefix: native pause/intro/ended/mode gate exact;29264call counted atboundary only, gameframe/body/bootstrap notexecuted. Original order29264 beforeB022driver/B026world from reachablecaller instructions, frame timing outsideprefix unverified.');(root/'reports/reward-gate-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
