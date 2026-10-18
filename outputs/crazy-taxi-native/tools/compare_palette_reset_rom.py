"""Complete original F5E8, including preservation of record mode/extrema."""
exec(open('outputs/crazy-taxi-native/tools/compare_palette_registration_rom.py').read().split('\ncases=')[0])
for i in md.disasm(r[0xf5e8:0xf62e],0x0800f5e8):allowed_thumb[i.address]=bytes(i.bytes)
saved=allowed_thumb.copy();source=source.replace("'bhi':c and not z", "'bhs':c,'blo':not c,'bhi':c and not z");exec(source);allowed_thumb.update(saved);RegistrationProbe.run=MotionProbe.run
steps=0
for count in [0,1,2,127,128]:
 probe=RegistrationProbe((0,0,0,0,0,1,0,0,0,0));probe.records=bytearray([0xa5]*2560)
 probe.meta={0x02001770:165,0x02001780:count,0x02001782:512,0x02001784:256};probe.reg[13]=0x10000400;probe.reg[14]=0x10020001
 probe.run(entry=0x0800f5e8,stop=0x10020000)
 run=subprocess.run([str(root/'bin/palette-reset-rom-compare.exe'),str(count)],text=True,capture_output=True,check=True).stdout.split()
 actual=[probe.meta[0x02001780],probe.meta[0x02001782],probe.meta[0x02001784],probe.meta[0x02001770]]
 assert int(run[0])==0 and actual==[int(v) for v in run[1:5]] and bytes(probe.records)==bytes(int(v,16) for v in run[5:]),count
 steps+=probe.steps
report={'status':'PASS','cases':5,'actual_thumb_instructions':steps,'scope':'CompleteF5E8 exact all2560records bytes and count/background/object counters/flags; zero/1/2/127/128active records. Active mode byte7 and extrema14/15 retained, inactive records retained. Unused original scratchr0/return-address value and native API status convention excluded; native invalid capacity guard excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/palette-reset-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
