"""Whole14-record attract palette trajectory versus original source helpers."""
exec(open('outputs/crazy-taxi-native/tools/compare_attract_assets_rom.py').read().split('\nfor i in md.disasm')[0])
p.reg[:3]=[0x084a5a34,32,1];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800e154,stop=0x10020000)
records=bytes(p.regions[0x2000000][0x1790:0x2190]);banks=bytes(p.regions[0x5000000]);globals_before=bytes(p.regions[0x2000000][0x1770:0x1790]);asset_steps=p.steps
exec(open('outputs/crazy-taxi-native/tools/compare_palette_fade_service_rom.py').read().split('\ncases=')[0])
todo=[0x0800eac2];seen=set()
while todo:
 a=todo.pop()
 if a==0x0800edde or a in seen:continue
 assert 0x0800eac2<=a<0x0800edde;seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
saved_allowed=allowed_thumb.copy();exec(source.replace('self.steps<50000','self.steps<2000000').replace("'bhi':c and not z","'bhs':c,'blo':not c,'bhi':c and not z"));allowed_thumb.update(saved_allowed);BankProbe.run=MotionProbe.run
p=BankProbe((0,0,0,0,0,1,0,0,0,0));p.records[:]=records;p.palette[:]=r[0x4a5a34:0x4a5c34];p.banks[:]=banks;p.globals=bytearray(globals_before);p.motion=bytearray(24);p.dimensions={0x2007542:240,0x2007544:160};p.hardware[0x400004c]=0x3456
p.reg[:4]=[1,32,1,1];p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800e5a8,stop=0x800edde);assert not any(p.banks);assert p.read(0x2001780,2)==14
native=(root/'reports/attract-native-fade-timeline.bin').read_bytes();assert len(native)==33*1024
for frame in range(33):
 p.write(0x2003b00,1);p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x800ee10,stop=0x800f5c6)
 assert bytes(p.banks)==native[frame*1024:(frame+1)*1024],frame
 assert p.read(0x200177b,1)==(frame<32) and p.read(0x200178a,2)==min(frame+1,31)
 assert p.hardware[0x400004c]==0
report={'status':'PASS','frames':33,'records':14,'palette_bytes_compared':len(native),'asset_Thumb_instructions':asset_steps,'fade_Thumb_instructions':p.steps,'scope':'Actual27994/E154/E388/7190/73C0 creates13OBJrecords thenactualE154cityrecord, actualE5A8initialpalette and33wholeEE10 services. Exactall1024banks/frame,progress/active/mosaic; nativecapturefromcomposedsetup. DMA/rawcopylogicalcontracts,source28BD0returncontract/divisioncontract. Hostframe scheduling/audio/input/renderer/gameplay excluded.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/city_palette_owned.c',root/'src/palette_fade_registered.c',root/'bin/attract-setup-owned-check.exe',root/'reports/attract-native-fade-timeline.bin']}}
(root/'reports/attract-loading-fade-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS original33frame14palette trajectory',len(native),'bytes',p.steps,'Thumb')
