"""Native frame selection vs bounded actual original Thumb instructions."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
import subprocess,itertools
for i in md.disasm(r[0x2318e:0x231ac],0x802318e):allowed_thumb[i.address]=bytes(i.bytes)
cases=list(itertools.product(range(8),range(4),range(256)))
cp=subprocess.run([str(root/'bin/passenger-frame-select-rom-compare.exe')],input=''.join(f'{k} {s} {f}\n' for k,s,f in cases),text=True,capture_output=True,check=True)
outputs=list(map(int,cp.stdout.split()));assert len(outputs)==len(cases);steps=0
for (k,s,f),expected in zip(cases,outputs):
 p=IdleProbe((0,0,0,0,0,1,0,0,0,0));actor=0x2005f2c
 for n in range(44):p.write(actor+n,0,1)
 p.write(actor+1,k,1);p.write(actor+10,s,1);p.write(actor+8,f,1);p.reg[5]=actor
 p.run(entry=0x802318e,stop=0x80231ac)
 assert p.read(actor+36)==expected,(k,s,f,p.read(actor+36),expected);steps+=p.steps
report={'cases':len(cases),'thumb_instructions':steps,'match':True,'rom_sha256':hashlib.sha256(r).hexdigest(),'scope':'Original2318E..231AA actual instructions and ROM087EC51C pointer table vs native customer word36; all8 legal kinds,4 stages,256 signed frame bytes. No caller cull, bitmap pixels, full22DF4 or fullframe proof.'}
(root/'reports/passenger-frame-select-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
