"""Whole original16100 reset versus native, retaining raw/pressed state."""
exec(open('outputs/crazy-taxi-native/tools/compare_save_defaults_rom.py').read().split('\nbinary=')[0])
for i in md.disasm(r[0x16100:0x1612e],0x8016100):allowed_thumb[i.address]=bytes(i.bytes)
class ResetProbe(DefaultsProbe):
 def write(self,a,v,size=4):InitProbe.write(self,a,v,size)
results=[];binary=root/'bin/input-startup-reset-rom-compare.exe';capture=root/'reports/input-startup-reset-native.bin'
for seed in [0,165,255]:
 p=ResetProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x2000000:bytearray([165])*262144};p.scalars={}
 for i in range(66):p.write(0x20034dc+i,(seed+i*13)&255,1)
 before=bytes(p.regions[0x2000000]);p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x8016100,stop=0x10020000)
 subprocess.run([str(binary),str(seed),str(capture)],check=True);native=capture.read_bytes();assert len(native)==66
 expected=bytearray(before);expected[0x34dc:0x351e]=native
 assert p.regions[0x2000000]==expected and p.reg[13]==0x10000400 and p.reg[4:12]==saved
 results.append({'seed':seed,'Thumb_instructions':p.steps})
report={'status':'PASS','cases':results,'compared_external_bytes_per_case':262144,'scope':'Wholeactual16100 andactual1E784 fills, nohelpercontracts. Allnative66packed inputbytes andsurroundingEWRAM compare. Held/previous/repeat 10bytes each and20countdownbytes cleared; raw/auxiliary/pressedmask and10pressedbytes retained. SP/callee-saved preserved. NativeAPI notyetbound tooriginalstartupparent; physicalXbox/fullframe excluded.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/input.c',root/'src/input.h',root/'tests/input-startup-reset-rom-compare.c',binary]},'rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/input-startup-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS wholeoriginal startup input reset',len(results),'cases',sum(q['Thumb_instructions'] for q in results),'Thumb')
