import pathlib,struct,json,hashlib
root=pathlib.Path('outputs/crazy-taxi-native'); ref=root/'reports/original-core-reference'; rom=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
manifest=json.loads((root/'assets/taxis/taxis.json').read_text()); raw=(root/'assets/taxis'/manifest['characters'][0]['pixels']).read_bytes()[:32768]
records=[]
for label in ('boot3300','boot4200'):
 oam=(ref/(label+'.07000000.bin')).read_bytes();vram=(ref/(label+'.06000000.bin')).read_bytes();pal=(ref/(label+'.05000000.bin')).read_bytes();rgba=bytearray(32768); halves=[]
 for i in range(128):
  a,b,c=struct.unpack_from('<HHH',oam,i*8)
  if (a,b,c) not in ((0x341,0xc017,0x426d),(0x341,0xc057,0x42ad)):continue
  half=0 if b==0xc017 else 1;tile=c&1023;bank=c>>12;data=vram[0x10000+tile*32:0x10000+tile*32+2048];ptr=struct.unpack_from('<I',rom,0x4a5514+half*4)[0]-0x8000000;assert data==rom[ptr:ptr+2048]
  for y in range(64):
   for x in range(64):
    q=data[((y//8)*8+x//8)*32+(y%8)*4+(x%8)//2];idx=(q>>(4*(x%2)))&15;v=struct.unpack_from('<H',pal,0x200+(bank*16+idx)*2)[0];out=((y*128+half*64+x)*4);rgba[out:out+4]=bytes(((v&31)*255//31,((v>>5)&31)*255//31,((v>>10)&31)*255//31,255 if idx else 0))
  halves.append({'half':half,'oam_index':i,'attributes':[a,b,c],'rom_offset':ptr,'center':[ (b&511)+64,(a&255)+64]})
 assert len(halves)==2
 matrix=list(struct.unpack_from('<h',oam,j)[0] for j in (6,14,22,30))
 visible=sum(rgba[i:i+4]!=raw[i:i+4] for i in range(0,len(raw),4) if rgba[i+3] or raw[i+3]);assert visible==0,visible
 records.append({'label':label,'matrix0':matrix,'halves':halves,'visible_rgba_mismatches':visible,'original_rgba_sha256':hashlib.sha256(rgba).hexdigest()})
report={'status':'PASS','records':records,'native_capture':'native-driver0-reference.json','finding':'Original driver0 frame0 VRAM and visible palette-decoded pixels match extracted native driver0 frame0. Prior native default startup check selected driver1; original reference driver0. Frame0 is rear sprite for these drivers, so prior front-facing interpretation is unsupported. Original and native camera poses differ; this is not full framebuffer equivalence.','scope':'Independent original OAM/VRAM/palette compared to native extracted assets; no runtime assets or camera altered.'}
(root/'reports/original-taxi-pixel-identity.json').write_text(json.dumps(report,indent=2));print(json.dumps(report,indent=2))
