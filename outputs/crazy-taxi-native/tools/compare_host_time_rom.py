import pathlib,json,subprocess,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');ns={}
exec((root/'tools/compare_frame_time_rom.py').read_text().split('cases=[]')[0],ns)
TimingProbe=ns['TimingProbe'];cases=[];expected=[]
for previous in [0,0xfffffffe,0x7ffffffe]:
 for duration in [0,1,16742706,16742707,50000000,67000000,84000000,100000000,1100000000]:
  for freeze in [0,1,255]:
   cases.append((previous,0xfffffffe,0xfffffffe,255,freeze,duration))
   counter=(previous+(duration*262144)//4389000000000)&0xffffffff
   p=TimingProbe((0,0,0,0,0,1,0,0,0,0))
   for a,v in {0x2003520:counter,0x2003524:previous,0x2003b00:0,0x2005f20:freeze,0x200755c:0xfffffffe,0x2001e10:0xfffffffe,0x2007540:255}.items():p.arm.mem[a]=v
   p.run(entry=0x80263d2,stop=0x802641e)
   expected.append([p.read(0x2003524),ns['ns']['signed'](p.read(0x2003b00)),p.read(0x200755c),p.read(0x2001e10),p.read(0x2007540,1)])
binary=root/'bin/host-time-probe.exe';out=subprocess.run([str(binary)],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
actual=[list(map(int,l.split())) for l in out.stdout.splitlines()];assert actual==expected
report={'status':'PASS','cases':len(cases),'scope':'Native nanosecond accumulation split into two helper-work intervals, followed by presentation commit, compared with original263D2..2641C instructions on exact rational counters. Covers subframe boundaries, long-frame clamp, wrap, freeze and presentation counters. Host module standalone; live gameplay scheduling, original workload cadence, input sampling and60fps presentation remain unverified.','records':[dict(input=c,output=o) for c,o in zip(cases,actual)],'hashes':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [root/'src/host_time.c',root/'src/host_time.h',binary]}}
(root/'reports/host-time-original-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',len(cases),'native elapsed-time/source-tail cases')
