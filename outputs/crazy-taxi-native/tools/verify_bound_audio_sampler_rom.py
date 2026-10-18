"""Stateful original15C3C versus all captured native Bound replay samples."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native')
ns={};exec((root/'tools/compare_input_sampler_rom.py').read_text().split('\ncases=')[0],ns)
p=ns['InputProbe']((0,0,0,0,0,1),0)
rows=[list(map(int,l.split())) for l in (root/'reports/bound-audio-presentation-capture-samplers.txt').read_text().splitlines()]
assert rows and all(len(row)==109 for row in rows)
assert rows[0][3:56]==[0]*53, 'Initial sampler state must be explicit zero reset.'
for a in range(0x20034dc,0x2003520,4):p.arm.mem[a]=0

def read_state():
 values=[p.read(0x20034de,2),p.read(0x20034dc,2),p.read(0x20034e0,2)]
 for i in range(10):
  values.extend([p.read(a+i,n) for a,n in [(0x20034e2,1),(0x20034ec,1),(0x20034f6,1),(0x2003500,1)]]+[p.read(0x200350a+i*2,2)])
 return values
ended=0;carry_checks=0
for index,row in enumerate(rows):
 raw,delta,flag=row[:3];before=row[3:56];after=row[56:]
 # No reseeding between callbacks: original retained state must match actual native prestate.
 assert read_state()==before,dict(sample=index,phase='before',original=read_state(),native=before)
 if index:carry_checks+=53
 p.arm.mem[0x4000130]=raw;p.arm.mem[0x2001ea8]=0;p.arm.mem[0x2001708]=0;p.arm.mem[0x200170c]=0;p.arm.mem[0x2003b00]=delta;p.reg[14]=0x10020001
 p.run(entry=0x8015c3c,stop=0x10020000)
 assert read_state()==after,dict(sample=index,phase='after',original=read_state(),native=after)
 ended+=bool(flag)
assert ended==203
report=dict(status='PASS_STATEFUL_BOUND_AUDIO_SAMPLER',samples=len(rows),ended_samples=ended,state_fields_per_sample=53,retained_field_checks=carry_checks,thumb_instructions=p.steps,rom_sha256=hashlib.sha256(ns['r']).hexdigest(),scope='Original15C3C continuous retained input state across every captured native sampler call from explicit zero reset. All raw/auxiliary/pressed masks, held/previous/repeat/pressed/countdown fields exact. Hardware key register and elapsed delta explicit recorded fixtures, physical/override suppression zero. Native service scheduling, full original game cadence and actual Xbox hardware not proved.')
report['artifacts']={str(path):hashlib.sha256(path.read_bytes()).hexdigest() for path in [root/'reports/bound-audio-presentation-capture-samplers.txt',root/'tests/scene_viewer_bound_audio_probe.c',root/'bin/scene-viewer-bound-audio-state-probe.exe']}
(root/'reports/bound-audio-presentation-capture-sampler-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
