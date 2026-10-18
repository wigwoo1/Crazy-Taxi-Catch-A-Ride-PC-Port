"""Actual DE64 then ABEC attract prefix, shared recorder and RNG."""
exec(open('outputs/crazy-taxi-native/tools/compare_attract_entry_rom.py').read().split('cases=')[0])
for i in md.disasm(r[0xde64:0xde8c],0x800de64):allowed_thumb[i.address]=bytes(i.bytes)
cases=[(s,f) for s in [0,1,0x4652414e,0x40000000,0xffffffff] for f in [0,1,127,255]]
binary=root/'bin/attract-entry-rom-compare.exe';rows=subprocess.run([str(binary),'--prepare'],input=''.join(f'{s} {f}\n' for s,f in cases),capture_output=True,text=True,check=True).stdout.splitlines();assert len(rows)==len(cases)*2;steps=0
for n,(seed,fill) in enumerate(cases):
 p=EntryProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray(262144),0x3000000:bytearray(32768)};p.scalars={};ram=p.regions[0x2000000]
 ram[0x1708:0x171c]=bytes((fill+j*37)&255 for j in range(20));ram[0x1ea8]=91;ram[0x3895]=153;ram[0x1eb0:0x1eb8]=bytes((fill+j*19)&255 for j in range(8));ram[0x153c]=ram[0x1697]=165;p.write(0x2003b04,seed)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800de64,stop=0x10020000);p.run(entry=0x800abec,stop=0x800ac62)
 parts=rows[n*2].split();assert bytes.fromhex(parts[0])==bytes(ram[0x1eb0:0x1eb8]);assert list(map(int,parts[1:]))==[ram[0x153c],ram[0x1697],p.read(p.reg[13]+32),p.read(0x3004ca0),p.read(0x2003b04)]
 assert bytes.fromhex(rows[n*2+1])==bytes(ram[0x1708:0x171c])+bytes([ram[0x1ea8],ram[0x3895]]);steps+=p.steps
report={'status':'PASS','cases':len(cases),'Thumb_instructions':steps,'scope':'ActualDE64 -> actualABEC attract prefix throughAC62 and actual1E72C RNG. Exactrecorder22/settings8/flags/savedcity/sceneidentity/RNG. Signedremainder contract only; noABECcommonsetup/playback/frame lifecycle.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/attract_prepare.c',root/'src/attract_entry.c',root/'tests/attract-entry-rom-compare.c',binary]}}
(root/'reports/attract-prepared-entry-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS prepared attract entry',len(cases),'cases',steps,'Thumb')
