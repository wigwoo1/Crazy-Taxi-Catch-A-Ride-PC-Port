from pathlib import Path
import subprocess,json,hashlib
root=Path('outputs/crazy-taxi-native');scope={};exec((root/'tools/compare_frame_time_rom.py').read_text().split('cases=[]')[0],scope)
cases=[(p,c,m,f) for p,c in [(0,0),(0,1),(0,4),(0,5),(0,6),(0,7),(0,60),(0,61),(0,120),(12,11),(0xfffffffe,2),(0,0x80000000)] for m in [0,1,4,5,6,7,60,61] for f in [0,1]]
binary=root/'bin/host-time-group-probe.exe';out=subprocess.run([str(binary)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),capture_output=True,text=True,check=True);rows=[list(map(int,l.split())) for l in out.stdout.splitlines()];assert len(rows)==len(cases);commits=deferred=rejected=0
for case,row in zip(cases,rows):
 previous,counter,minimum,freeze=case
 if not 1<=minimum<=60:expected=[-1,previous,7,123,44,1];rejected+=1
 elif ((counter-previous)&0xffffffff)<minimum:expected=[0,previous,7,123,44,1];deferred+=1
 else:
  p=scope['TimingProbe']((0,0,0,0,0,1,0,0,0,0))
  for a,v in {0x2003520:counter,0x2003524:previous,0x2003b00:7,0x2005f20:freeze,0x200755c:123,0x2001e10:44,0x2007540:1}.items():p.arm.mem[a]=v
  p.run(entry=0x80263d2,stop=0x802641e);expected=[1,p.read(0x2003524),scope['ns']['signed'](p.read(0x2003b00)),p.read(0x200755c),p.read(0x2001e10),p.read(0x2007540,1)];commits+=1
 assert row==expected,(case,row,expected)
report=dict(status='PASS_NATIVE_GROUPED_COMMIT_BOUNDARY',cases=len(cases),original_tail_compared_commits=commits,deferred_without_mutation=deferred,rejected_without_mutation=rejected,scope='Caller-controlled native timing boundary with exact original263D2 tail at commits, byte-immutable skipped/invalid events, modulo32 counter wrap, signed-clamp/overdue/freeze cases. Defaultpresent_due is minimum1 and policy unchanged. This does not establish an original group policy or integrate general live gameplay.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),root/'src/host_time.c',root/'src/host_time.h',root/'tests/host_time_group_probe.c',binary]})
(root/'reports/host-time-group-verification.json').write_text(json.dumps(report,indent=2)+'\n');print({k:v for k,v in report.items() if k not in ['scope','hashes']})
