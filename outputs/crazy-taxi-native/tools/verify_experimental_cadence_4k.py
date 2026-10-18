import sys
import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');scope={};exec((root/'tools/compare_frame_time_rom.py').read_text().split('cases=[]')[0],scope)
minimum=int(sys.argv[1]);stem=f'experimental-cadence-{minimum}-4k'
rows=[list(map(int,l.split())) for l in (root/f'reports/{stem}.clock.txt').read_text().splitlines()];assert len(rows)==360 and all(len(r)==10 for r in rows)
phase=counter=previous=elapsed=animation=page=0;delta=1;updates=0;ready=1;commits=skips=0
for ns,ticks,prior,now,after,freeze,committed,count,audio_ticks,audio_frames in rows:
 updates+=ready;assert count==updates and prior==delta
 total=phase+ns*262144;expected_ticks,phase=divmod(total,4389000000000);assert ticks==expected_ticks;counter=(counter+ticks)&0xffffffff;assert now==counter
 assert audio_ticks==counter and audio_frames==counter
 due=int(((counter-previous)&0xffffffff)>=minimum);assert committed==due
 if due:
  p=scope['TimingProbe']((0,0,0,0,0,1,0,0,0,0))
  for a,v in {0x2003520:counter,0x2003524:previous,0x2003b00:delta,0x2005f20:freeze,0x200755c:elapsed,0x2001e10:animation,0x2007540:page}.items():p.arm.mem[a]=v
  p.run(entry=0x80263d2,stop=0x802641e);previous=p.read(0x2003524);delta=p.read(0x2003b00);elapsed=p.read(0x200755c);animation=p.read(0x2001e10);page=p.read(0x2007540,1);commits+=1
 else:skips+=1
 assert after==delta;ready=due
assert skips>0 and updates>50
report={'status':'PASS_EXPERIMENTAL_CADENCE_TIMING','minimum_ticks':minimum,'rendered_frames':len(rows),'gameplay_updates':updates,'presentation_commits':commits,'render_only_events':skips,'elapsed_counter_ticks':counter,'audio_ticks_serviced':audio_ticks,'mixed_audio_frames':audio_frames,'scope':'Actual360-frame experimental caller-selected minimum cadence trace, NOT original workload scheduling: every gameplay update uses preceding committed frame time; render-only events do not commit or duplicate gameplay. Committed timing outputs compared with original tail. Every measured elapsed tick also services exactly one logical352-sample audio frame. Audibility/device output disabled; not waveform or input-event ordering proof. Source routine physics individually qualified earlier, continuous original trajectory/cadence and4K display60fps remain unproven.','binary_sha256':hashlib.sha256((root/'bin/scene-viewer-cadence-experiment.exe').read_bytes()).hexdigest()}
(root/f'reports/{stem}-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))


log=(root/f'reports/{stem}.log').read_text();assert not (root/f'reports/{stem}.err').read_text()
assert 'LIVE4K GPU COMPLETED: 360 frames at3840x2160' in log and 'NATIVE4K TARGET: rendered360 window blits360.' in log
assert f'EXPERIMENTAL UPDATE GROUP: minimum{minimum} source ticks' in log
print('PASS360 realGPU4Kframes; experimental source cadence not claimed faithful')
