"""Recover original start-phase replacement artwork and palette tables."""
import pathlib,struct,hashlib,json,sys
sys.path.insert(0,str(pathlib.Path(__file__).resolve().parent));from png_writer import write_png
root=pathlib.Path(__file__).resolve().parents[1];r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes();sha=hashlib.sha256(r).hexdigest();assert sha=='39996ec9c561f68e16e5435b781bb7c73f4e0123c13002c109f8150ab6e89de5'
out=root/'assets/start-phase';out.mkdir(exist_ok=True);rows=[];blob=bytearray()
for phase in range(3):
 for language in range(5):
  index=phase*5+language
  source=struct.unpack_from('<I',r,0x7ec210+4*index)[0];palette=struct.unpack_from('<I',r,0x7ec24c+4*index)[0]
  a=source-0x8000000;p=palette-0x8000000;assert 0<=a<=len(r)-1536 and 0<=p<=len(r)-32
  values=struct.unpack_from('<16H',r,p);bank=bytearray()
  for frame in range(3):
   raw=r[a+frame*512:a+(frame+1)*512];rgba=bytearray()
   for y in range(32):
    for x in range(32):
     v=raw[((y//8)*4+x//8)*32+(y%8)*4+(x%8)//2];i=(v>>(4*(x&1)))&15;color=values[i]
     rgba.extend([((color>>shift)&31)*255//31 for shift in (0,5,10)]+[255 if i else 0])
   bank.extend(rgba);write_png(out/f'phase{phase}-language{language}-frame{frame}.png',32,32,rgba)
  blob.extend(bank);rows.append({'phase':phase,'language_index':language,'art_pointer':hex(source),'palette_pointer':hex(palette),'rgba_sha256':hashlib.sha256(bank).hexdigest()})
(out/'frames.rgba').write_bytes(blob);assert (out/'frames.rgba').read_bytes()==blob
report={'rom_sha256':sha,'records':rows,'format':'phase-major, language-major, three32x32RGBA frames; source4bpp/index0transparent','source_owner':'BC04..BC64 three27FB8 group7 replacements; BC68..BCA4 OBJpalettebank11 DMA and registeredpointer override','scope':'Exact source pointers/raw pixel decoding. Original language388D range mapping, phase runtime and original OAMscale/positions remain unverified.'}
(out/'frames.json').write_text(json.dumps(report,indent=2));print(f'PASS extracted {len(rows)*3} original startup frames, {len(blob)}RGBA bytes; ROM preserved.')
