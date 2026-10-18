import pathlib,json,itertools,subprocess
root=pathlib.Path('outputs/crazy-taxi-native');intro={};normal={}
exec((root/'tools/compare_countdown_source_initialized_rom.py').read_text().split('schema=')[0],intro)
exec((root/'tools/probe_ordinary_first_movement_rom.py').read_text().split('rows=[]')[0],normal)
cases=[];expected=[]
for phase,active,fade,exiting,freeze,ended,demo in itertools.product((-1,0,2),(0,1),(0,1),(0,1),(0,1),(0,1),(0,1)):
 frame=4;prior=17;c=(prior,frame,phase,active,fade,exiting,freeze,ended,demo);cases.append(c)
 p=intro['StartProbe']((0,0,0,0,0,1,0,0,0,0));p.scalars.update({0x200177b:fade,0x2001ea8:exiting,0x2003b00:frame,0x30053c0:prior});p.write(0x2001696,active,1);p.write(0x20016b4,phase,2)
 try:p.run(entry=0x800ba04,stop={0x800ba4c,0x800bb24,0x800bd5c})
 except intro['ReturnBoundary']:pass
 first=p.read(0x30053c0)
 p=normal['CallerProbe']((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.scalars.update({0x2005f20:freeze,0x2001708:demo,0x2001696:active,0x2001697:ended,0x2007700:0,0x2001700:0,0x2001704:0,0x2003b00:frame,0x30053c0:prior,0x3004ca0:0x2002634});
 for a,v in [(0x2005f20,freeze),(0x2001696,active),(0x2001697,ended),(0x2001708,demo)]:p.write(a,v,1)
 p.reg[9]=0x2001708;p.reg[7]=0x2002634;p.run(entry=0x800afd4,stop={0x800b27c,0x800b284,0x800b08c,0x800b022});expected.append([first,p.read(0x30053c0)])
binary=root/'bin/frame-time-handoff-probe.exe';result=subprocess.run([str(binary)],input=''.join(' '.join(map(str,x))+'\n' for x in cases),text=True,capture_output=True,check=True);actual=[list(map(int,x.split())) for x in result.stdout.splitlines()];assert actual==expected,next((c,a,e) for c,a,e in zip(cases,actual,expected) if a!=e)
report={'status':'PASS','cases':len(cases),'source_ranges':['BA04 throughBA4C/BB24/BD5C/C524','AFD4 throughB022/B08C/B284/B27C'],'finding':'Intro physics time copy is blocked by fade/exiting and requires active phase-1. Ordinary B00A copy is blocked by freeze/intro/ended/demo; boarding/delivery gates occur AFTER copy. Prior value retained otherwise.','scope':'Exact local ownership gates across all combinations. Demo B2E0 later copy and entire source frame continuity not included.'};(root/'reports/frame-time-handoff-original-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',len(cases),'original/native physics-time gate cases')
