import pathlib,json,subprocess
root=pathlib.Path('outputs/crazy-taxi-native');scope={};exec((root/'tools/compare_frame_time_rom.py').read_text().split('cases=[]')[0],scope)
cases=[(1000000,(i//25)%2) for i in range(350)]+[(0,0),(16742706,0),(16742707,1),(100000000,0),(0,1)]
out=subprocess.run([str(root/'bin/host-time-due-probe.exe')],input=''.join(f'{ns} {freeze}\n' for ns,freeze in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in out.stdout.splitlines()];assert len(rows)==len(cases)
phase=counter=previous=elapsed=animation=page=delta=0;commits=skips=0
for (duration,freeze),row in zip(cases,rows):
 total=phase+duration*262144;ticks,phase=divmod(total,4389000000000);counter=(counter+ticks)&0xffffffff;due=int(counter!=previous)
 if due:
  p=scope['TimingProbe']((0,0,0,0,0,1,0,0,0,0))
  for a,v in {0x2003520:counter,0x2003524:previous,0x2003b00:delta,0x2005f20:freeze,0x200755c:elapsed,0x2001e10:animation,0x2007540:page}.items():p.arm.mem[a]=v
  p.run(entry=0x80263d2,stop=0x802641e);previous=p.read(0x2003524);delta=p.read(0x2003b00);elapsed=p.read(0x200755c);animation=p.read(0x2001e10);page=p.read(0x2007540,1);commits+=1
 else:skips+=1
 assert row==[ticks,counter,due,delta,elapsed,animation,page],(duration,row)
assert skips>300 and commits>20
report={'status':'PASS','events':len(rows),'commits_source_compared':commits,'zero_tick_events_no_mutation':skips,'scope':'Host adaptation gates recovered frame tail after actual tick advancement. Original routine compared on committed events; skipped rendered frames preserve all timing state. No original gameplay cadence or display scanout claim.'}
(root/'reports/host-time-due-verification.json').write_text(json.dumps(report,indent=2));print(report)
