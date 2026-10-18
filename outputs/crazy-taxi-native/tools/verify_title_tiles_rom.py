"""Every originaltitle compressed4bpp tile versusactual1E860."""
exec(open('outputs/crazy-taxi-native/tools/extract_startup_bitmaps.py').read().split('\nrecords=[]')[0])
metadata=json.loads((root/'assets/title/background.json').read_text());start=int(metadata['compressed_tiles'],16);cursor=start;tiles=(root/'assets/title/background.tiles').read_bytes();assert len(tiles)==8032
steps=0
for tile in range(len(tiles)//32):
 p=InitProbe((0,0,0,0,0,1,0,0,0,0));p.regions={0x10001000:bytearray([165]*64)};p.scalars={}
 p.reg[:3]=[cursor,32,0x10001000];p.reg[13]=0x10000400;p.reg[14]=0x10020001;saved=p.reg[4:12].copy();p.run(entry=0x801e860,stop=0x10020000)
 assert p.reg[13]==0x10000400 and p.reg[4:12]==saved
 assert p.regions[0x10001000][:32]==tiles[tile*32:tile*32+32],tile
 assert p.regions[0x10001000][32:]==bytes([165]*32),tile
 assert p.reg[0]>cursor;cursor=p.reg[0];steps+=p.steps
assert cursor==int(metadata['compressed_end'],16)
report={'status':'PASS','tiles':len(tiles)//32,'decoded_bytes':len(tiles),'Thumb_instructions':steps,'source_start':hex(start),'source_end':hex(cursor),'scope':'All251 originaltitle4bpp tiles decodedthroughactualwhole1E860, byte-exact extracted8032tilebytes, returnedfinalcompressedcursor080B60F7, each32byte outputguard andSP/callee-saved. Notwhole7190/GPU/assetallocator/displayproof.','rom_sha256':hashlib.sha256(r).hexdigest(),'hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'assets/title/background.tiles',root/'assets/title/background.json']}}
(root/'reports/title-tiles-rom-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(tiles)//32,steps,hex(cursor))
