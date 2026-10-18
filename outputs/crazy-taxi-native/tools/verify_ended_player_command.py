"""Original player command updates across ended flags and every held mask."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_player_command_rom.py').read_text().split('\ncases=')[0],ns)
Probe=ns['CommandProbe'];count=steps=0;changed=0
for mask in range(1024):
 results=[]
 for ended in [0,1,255]:
  p=Probe((0,0,0,0,0,1),0);p.initialize((mask,0,0,0,0,0,0,0));p.arm.mem[0x02001694]=ended<<24
  for i in range(16):p.write(0x030053d0+i,(255+i*17)&255,1)
  p.reg[0]=0;p.run(entry=0x08011734,stop=0x10020000)
  results.append([p.read(0x030053d0+i,1) for i in range(16)]);steps+=p.steps;count+=1
 assert results[0]==results[1]==results[2],mask
 if results[1]!=[(255+i*17)&255 for i in range(16)]:changed+=1
assert count==3072 and changed==1024
report=dict(status='PASS_ORIGINAL_COMMAND_ENDED_INDEPENDENCE',cases=count,held_masks=1024,ended_flags=[0,1,255],masks_command_changed=changed,thumb_instructions=steps,scope='Actual original11734 complete command packer updates all16bytes from held fixtures identically for ended0/1/255. Retaining last command after end is not equivalent to refreshing held input. Original input sampler/action edges, outerengine call timing, native integration and Xbox hardware are not proven.')
(root/'reports/ended-player-command-verification.json').write_text(json.dumps(report,indent=2));print('PASS',report)
