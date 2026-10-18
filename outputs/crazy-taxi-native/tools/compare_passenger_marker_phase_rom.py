exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
import itertools,subprocess
for i in md.disasm(r[0x22e38:0x22e82],0x8022e38):allowed_thumb[i.address]=bytes(i.bytes)
for i in md.disasm(r[0x22eb0:0x22ecc],0x8022eb0):allowed_thumb[i.address]=bytes(i.bytes)
cases=list(itertools.product([0,5,6,7,127,128,255],[0,1,3,4,127,128,255],[0,1,255],[-1,0,1,7,128],[0,1,128]))
cp=subprocess.run([str(root/'bin/passenger-marker-phase-rom-compare.exe')],input=''.join(' '.join(map(str,x))+'\n' for x in cases),text=True,capture_output=True,check=True)
outputs=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(outputs)==len(cases);steps=0
for (c,f,d,delta,z),expected in zip(cases,outputs):
 p=IdleProbe((0,0,0,0,0,1,0,0,0,0))
 for a,v in [(0x2005f28,c),(0x2005f29,f),(0x2005f2a,d),(0x2005f20,z)]:p.write(a,v,1)
 p.write(0x2003b00,delta);p.run(entry=0x8022e38,stop=0x8022ecc)
 actual=[p.read(a,1) for a in [0x2005f28,0x2005f29,0x2005f2a]]
 assert actual==expected,((c,f,d,delta,z),actual,expected);steps+=p.steps
report={'cases':len(cases),'thumb_instructions':steps,'match':True,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Original22E38..22ECA marker phase scalar state only; controlled signed byte/wrap/delta/pause cases. Actual marker request projection/pixels and initialization provenance not proved.'}
(root/'reports/passenger-marker-phase-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
