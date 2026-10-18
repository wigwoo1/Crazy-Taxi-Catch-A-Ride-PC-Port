"""Original10AAC complete caller, opaque27610 stop requests."""
exec(open('outputs/crazy-taxi-native/tools/compare_audio_stop_one_rom.py').read().split('\ncases=')[0])
for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[0x10aac:0x10b2a],0x8010aac):allowed_thumb[i.address]=bytes(i.bytes)
class CleanupProbe(ExitProbe):
 def external(self,target):
  assert target==0x8027610,hex(target)
  self.calls.append(self.reg[0])
cases=list(range(1024));cp=subprocess.run([str(root/'bin/game-audio-cleanup-rom-compare.exe')],input=''.join(str(c)+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];steps=0
for mask,want in zip(cases,rows):
 p=CleanupProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[]
 for i in range(10):p.write(0x20025e8+i*4,0x2009000+(i%4)*32 if mask&(1<<i) else 0)
 p.reg[14]=0x10020001;p.run(entry=0x8010aac,stop=0x10020000)
 assert [len(p.calls),*p.calls]==want,(mask,p.calls,want)
 for i in range(10):assert p.read(0x20025e8+i*4)==(0x2009000+(i%4)*32 if mask&(1<<i) else 0)
 steps+=p.steps
assert len(rows)==1024
report=dict(cases=1024,thumb_instructions=steps,equal=True,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete10AAC ten nonzero sound-handle stop requests ordered, aliases retained, owner slots unchanged;27610 opaque boundary independently qualified; actual gameplay handle assignment excluded')
(root/'reports/game-audio-cleanup-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
