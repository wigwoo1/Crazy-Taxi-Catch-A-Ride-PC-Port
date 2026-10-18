import pathlib,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');scope={};exec((root/'tools/compare_frame_time_rom.py').read_text().split('cases=[]')[0],scope)
rows=[list(map(int,l.split())) for l in (root/'reports/live-due-clock.txt').read_text().splitlines()];assert len(rows)==360 and all(len(r)==8 for r in rows)
phase=counter=previous=elapsed=animation=page=0;delta=1;updates=0;ready=1;commits=skips=0
for ns,ticks,prior,now,after,freeze,committed,count in rows:
 updates+=ready;assert count==updates and prior==delta
 total=phase+ns*262144;expected_ticks,phase=divmod(total,4389000000000);assert ticks==expected_ticks;counter=(counter+ticks)&0xffffffff;assert now==counter
 due=int(counter!=previous);assert committed==due
 if due:
  p=scope['TimingProbe']((0,0,0,0,0,1,0,0,0,0))
  for a,v in {0x2003520:counter,0x2003524:previous,0x2003b00:delta,0x2005f20:freeze,0x200755c:elapsed,0x2001e10:animation,0x2007540:page}.items():p.arm.mem[a]=v
  p.run(entry=0x80263d2,stop=0x802641e);previous=p.read(0x2003524);delta=p.read(0x2003b00);elapsed=p.read(0x200755c);animation=p.read(0x2001e10);page=p.read(0x2007540,1);commits+=1
 else:skips+=1
 assert after==delta;ready=due
assert skips>0 and updates>300
report={'status':'PASS','rendered_frames':len(rows),'gameplay_updates':updates,'presentation_commits':commits,'render_only_events':skips,'elapsed_counter_ticks':counter,'scope':'Actual360-frame live trace: every gameplay update uses preceding committed frame time; render-only events do not commit or duplicate gameplay. Committed timing outputs compared with original tail. Source routine physics individually qualified earlier, continuous original trajectory/cadence and4K display60fps remain unproven.','binary_sha256':hashlib.sha256((root/'bin/scene-viewer-live-due-clock.exe').read_bytes()).hexdigest()}
(root/'reports/live-due-clock-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
