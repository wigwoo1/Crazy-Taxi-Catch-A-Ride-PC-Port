"""Original1599E..15A10 immediate palette gate and registration pointer writes."""
exec(open('outputs/crazy-taxi-native/tools/compare_sprite_replacement_rom.py').read().split('\ncases=')[0])
for start,end in [(0x1599e,0x159d2),(0x159f0,0x15a12)]:
 for i in md.disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
cases=[(immediate,driver,override) for immediate in [0,1,255] for driver in range(4) for override in [0,1,255]]
rows=subprocess.run([str(root/'bin/taxi-frame-palette-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases)
steps=0
for case,row in zip(cases,rows):
 immediate,driver,override=case;p=ReplaceProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2001790:bytearray([165]*256),0x4000000:bytearray(256)};p.scalars={0x2001708:override};p.reg[13]=0x10000400
 for address,value in [(0x100004e4,immediate),(0x100004e8,driver),(0x40000d4,0x11111111),(0x40000d8,0x22222222),(0x40000dc,0x33333333)]:p.write(address,value)
 p.run(entry=0x801599e,stop=0x8015a12)
 parts=row.split();expected=[int(bool(immediate)),p.read(0x40000d4),p.read(0x40000d8),p.read(0x40000dc)]
 assert list(map(int,parts[:4]))==expected,(case,parts[:4],expected)
 assert parts[4]==p.regions[0x2001790].hex(),(case,'registry')
 palette=[0xa5a5]*256
 if immediate:
  source=p.read(0x40000d4)-0x8000000;bank=8 if override else 4;palette[bank*16:bank*16+16]=struct.unpack_from('<16H',r,source)
 assert parts[5]==''.join('%04x'%v for v in palette),(case,'palette copy')
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'Actual1599E..15A10 control flow, DMA identities/control and all256 registry bytes vs native; native 16 RGB555 colors and untouched other banks vs ROM bytes. Override maps OBJ8/record8, normal OBJ4/record4. Immediate0 bypass. No DMA timing, palette fade integration, full1556C or facing proof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/taxi_frame_palette_owned.c',root/'src/taxi_frame_palette_owned.h',root/'tests/taxi-frame-palette-rom-compare.c',root/'bin/taxi-frame-palette-rom-compare.exe']}}
(root/'reports/taxi-frame-palette-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(cases),steps)
