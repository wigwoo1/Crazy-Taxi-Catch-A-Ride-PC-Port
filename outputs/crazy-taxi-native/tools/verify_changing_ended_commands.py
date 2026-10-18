from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native');ns={};exec((root/'tools/compare_player_command_rom.py').read_text().split('\ncases=')[0],ns)
rows=[list(map(int,l.split())) for l in (root/'reports/changing-ended-buttons.txt').read_text().splitlines()];assert len(rows)==124
steps=0
for row in rows:
 tick,keys=row[:2];buttons=row[2:32];native=row[32:];assert len(native)==16
 streams=[sum(buttons[i*3+j]<<i for i in range(10)) for j in range(3)]
 assert streams[0]==(keys^1023)
 p=ns['CommandProbe']((0,0,0,0,0,1),0);p.initialize(tuple(streams)+(0,0,0,0,0));p.arm.mem[0x02001694]=1<<24
 for i in range(16):p.write(0x030053d0+i,0,1)
 p.reg[0]=0;p.run(entry=0x08011734,stop=0x10020000)
 actual=[p.read(0x030053d0+i,1) for i in range(16)];assert actual==native,(tick,actual,native);steps+=p.steps
report=dict(status='PASS_CHANGING_ENDED_COMMANDS',cases=124,distinct_masks=len({r[1] for r in rows}),thumb_instructions=steps,scope='Native actual post-ended sampler/command capture versus original11734 all16bytes on124changingheld/pressed/repeat fixtures with1697=1; held matches active-low input. Original sampler edge/timer trajectory, nested actionquery/physics, keyboard, hardware and menu timing remain unqualified.')
(root/'reports/changing-ended-command-verification.json').write_text(json.dumps(report,indent=2));print('PASS',report)
