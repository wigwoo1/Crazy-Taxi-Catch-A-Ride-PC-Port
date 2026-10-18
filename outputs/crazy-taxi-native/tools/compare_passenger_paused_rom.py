"""Actual paused22F34..231AC actor control and frame pointer, no RNG call."""
exec(open('outputs/crazy-taxi-native/tools/compare_passenger_idle_live_rom.py').read().split('\nrows=')[0])
for i in md.disasm(r[0x2318e:0x231ac],0x802318e):allowed_thumb[i.address]=bytes(i.bytes)
cases=[(flags,timer,kind) for flags in [1,65] for timer in [0,1,89,90,127,128,255] for kind in [0,7]]
cp=subprocess.run([str(root/'bin/passenger-paused-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (flags,timer,kind),want in zip(cases,rows):
 p=IdleProbe((0,0,0,0,0,1,0,0,0,0));p.calls=0;actor=0x2005f2c;taxi=0x10003000
 for j in range(44):p.write(actor+j,0,1)
 for off,value in [(0,flags),(1,kind),(11,timer)]:p.write(actor+off,value,1)
 for off in [0,4,8]:p.write(taxi+off,0)
 p.write(0x2003b00,1);p.write(0x2005f20,1,1);p.write(0x2003b04,123);p.write(0x2007530,0)
 sl=int.from_bytes(r[0x22fcc:0x22fd0],'little');p.write(sl,0)
 p.reg[7]=actor;p.reg[8]=taxi;p.reg[9]=actor;p.reg[10]=sl;p.write(p.reg[13]+0x38,0)
 p.run(entry=0x8022f34,stop=0x802318e)
 p.reg[5]=actor;p.run(entry=0x802318e,stop=0x80231ac)
 actual=[p.read(actor+j,1) for j in range(44)]+[p.read(0x2003b04)]
 assert actual==want,((flags,timer,kind),actual,want);assert p.calls==0;steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Actual paused freeze1 original22F34 actor branch to2318E then full2318E..231AA frame pointer. Flag64 timer byte11 advances before freeze gate with signed saturation; animation timers/movement/RNG remain unchanged; frame pointer refreshed. Explicit visible/caller-register fixtures, not whole game pause/scene render-cull equivalence.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/passenger_idle.c','tests/passenger_paused_rom_compare.c','tools/compare_passenger_paused_rom.py']})
(root/'reports/passenger-paused-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
