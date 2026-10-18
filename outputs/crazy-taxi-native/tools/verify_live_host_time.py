import pathlib,json,hashlib,collections
root=pathlib.Path('outputs/crazy-taxi-native');scope={}
exec((root/'tools/compare_frame_time_rom.py').read_text().split('cases=[]')[0],scope)
rows=[list(map(int,l.split())) for l in (root/'reports/live-host-time-trace.txt').read_text().splitlines()];assert len(rows)==90
phase=counter=elapsed=animation=page=0;previous_delta=1
for ns,ticks,prior,now,delta,freeze in rows:
 total=phase+ns*262144;expected_ticks,phase=divmod(total,4389000000000);assert ticks==expected_ticks
 old_counter=counter;counter=(counter+ticks)&0xffffffff;assert now==counter and prior==previous_delta
 p=scope['TimingProbe']((0,0,0,0,0,1,0,0,0,0))
 for a,v in {0x2003520:counter,0x2003524:old_counter,0x2003b00:previous_delta,0x2005f20:freeze,0x200755c:elapsed,0x2001e10:animation,0x2007540:page}.items():p.arm.mem[a]=v
 p.run(entry=0x80263d2,stop=0x802641e)
 assert p.read(0x2003b00)==delta;previous_delta=delta;elapsed=p.read(0x200755c);animation=p.read(0x2001e10);page=p.read(0x2007540,1)
report={'status':'PASS','live_commits':len(rows),'counter_ticks':counter,'frame_delta_histogram':dict(collections.Counter(r[4] for r in rows)),'source_tail_calls':90,'scope':'Actual bounded native live-clock nanosecond trace checked against exact counter ratio and original timing tail at each commit. Retained prior delta verified in trace. Gameplay trajectory/original cadence and display60fps not proven; observed90swaps mean22.003ms at644x461,31missed60Hz deadlines, no4K test. Audio output disabled and signal silent; logical audio120frames independent of90presentation commits. Installed playable unchanged.','binary_sha256':hashlib.sha256((root/'bin/scene-viewer-live-host-time.exe').read_bytes()).hexdigest()}
(root/'reports/live-host-time-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
