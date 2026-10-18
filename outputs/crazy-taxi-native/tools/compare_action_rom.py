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
cases=[(mask,0,0,profile,0,0,0,2) for mask in range(1024) for profile in range(3)]
for profile in range(3):
 for action in range(7):
  for flags in [0,32]:
   for stream in range(3):
    for mask in [0,1023]+[1<<i for i in range(10)]:
     m=[0,0,0];m[stream]=mask;cases.append(tuple(m)+(profile,flags,0,0,action))
  for cmd in range(16):cases.append((1023,1023,1023,profile,0,1,cmd,action))
res=subprocess.run([str(root/'bin/action-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=list(map(int,res.stdout.split()));assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 probe=ActionProbe((0,0,0,0,0,1),0);probe.initialize(case);probe.run(entry=0x8015d98,stop=0x10020000);assert probe.reg[0]==row,dict(case=case,original=probe.reg[0],native=row);steps+=probe.steps
report=dict(cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original15D98 query vsnative all1024heldmasks foraction2/all3profiles, all7actions/singlebuttons/held-pressed-repeat/gates plusoverride16masks. Input/globalfixtures not originalinputsampler/physicalXbox orwholeframe.')
report['sha256']={str(x):hashlib.sha256(x.read_bytes()).hexdigest() for x in [root/'tools/compare_action_rom.py',root/'tests/action_rom_compare.c',root/'src/actions.c',root/'bin/action-rom-compare.exe',root/'reports/challenge-bound-acceleration-action-query-cfg.json']}
(root/'reports/action-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)

