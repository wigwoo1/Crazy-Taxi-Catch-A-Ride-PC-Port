"""Original actionquery15D98 vsnative; diagnosticfixtureinputglobals."""
exec(open('outputs/crazy-taxi-native/tools/compare_vehicle_rom.py').read().split('\ncases=[')[0])
root=pathlib.Path('outputs/crazy-taxi-native');cfg=json.loads((root/'reports/challenge-bound-acceleration-action-query-cfg.json').read_text());allowed_thumb={int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg['instructions']}
class ActionProbe(MotionProbe):
 def initialize(self,case):
  h,p,r,profile,flags,over,cmd,action=case
  for start,end in [(0x20034e0,0x200350c),(0x2001708,0x200170c),(0x2001eb0,0x2001eb4),(0x30053d0,0x30053d8)]:
   for a in range(start,end,4):self.arm.mem[a]=0
  def put(a,v):self.arm.mem[a&~3]=(self.arm.read(a&~3)&~(255<<((a&3)*8)))|((v&255)<<((a&3)*8))
  for mask,start in [(h,0x20034e2),(p,0x2003500),(r,0x20034f6)]:
   for i in range(10):put(start+i,(mask>>i)&1)
  put(0x2001eb0,profile<<1);put(0x2001708,over);put(0x30053d4,cmd);self.arm.mem[0x2002ae0]=flags;self.reg[0]=action;self.reg[14]=0x10020001
rows=[list(map(int,l.split())) for l in (root/'reports/jump-effect-input-continuous-states.txt').read_text().splitlines()];differences=[];steps=0
for row in rows:
 assert len(row)==720
 held=sum(row[685+3*j]<<j for j in range(10));pressed=sum(row[686+3*j]<<j for j in range(10));repeat=sum(row[687+3*j]<<j for j in range(10))
 profile,flags,over,cmd,want=row[715:720];case=(held,pressed,repeat,profile,flags,over,cmd,2)
 probe=ActionProbe((0,0,0,0,0,1),0);probe.initialize(case);probe.run(entry=0x8015d98,stop=0x10020000);assert probe.reg[0]==want,(row[0],want,probe.reg[0]);steps+=probe.steps
 if want!=row[14]:differences.append(row[0])
report=dict(frames=len(rows),source_query_matches=len(rows),scripted_effect_action_differences=len(differences),first_difference=differences[0] if differences else None,thumb_instructions=steps,scope='Original15D98 vs actual captured input-query returns; effective scripted effect gas separately differs, not nested full-effect equivalence')
report['sha256']={str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in [root/'reports/jump-effect-input-continuous-states.txt',root/'tools/compare_continuous_action_inputs_rom.py',root/'src/scene_viewer.c',root/'bin/scene-viewer.exe']}
(root/'reports/continuous-action-inputs-rom-comparison.json').write_text(json.dumps(report,indent=2));print(report)
