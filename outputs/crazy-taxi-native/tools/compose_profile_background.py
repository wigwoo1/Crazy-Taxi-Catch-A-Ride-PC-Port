"""Original profile BG-only composition, source5778 flags3/1. No OBJ claim."""
import pathlib,hashlib,struct,json
from extract_guidance import EXPECTED
from png_writer import write_png
r=pathlib.Path('Crazy Taxi - Catch a Ride (USA).gba').read_bytes()
assert hashlib.sha256(r).hexdigest()==EXPECTED
root=pathlib.Path('outputs/crazy-taxi-native/assets')
front=(root/'profile/background.indices').read_bytes()
back=(root/'profile-layer1/background.indices').read_bytes()
assert len(front)==len(back)==512*256
pal=struct.unpack_from('<256H',r,0xb458c)
other=struct.unpack_from('<256H',r,0xa2f54)
assert [i for i in range(256) if pal[i]!=other[i]]==[0]
assert r[0x340d8+120:0x340d8+122]==bytes([3,1])
rgba=bytearray();full=bytearray();blended=0
for y in range(256):
 for x in range(512):
  i=y*512+x;a,b=front[i],back[i]
  color=pal[a] if a else pal[b] if b else pal[0]
  # BG0priority0 first target, BG1priority1 second target. BLDCNT1f4f
  # excludes backdrop as a second target; coefficients8/8 at04000052.
  if a and b:
   color=sum(min(31,(((pal[a]>>s)&31)+((pal[b]>>s)&31))//2)<<s for s in (0,5,10));blended+=1
  pixel=bytes([((color>>s)&31)*255//31 for s in (0,5,10)]+[255])
  full.extend(pixel)
  if x<240 and y<160:rgba.extend(pixel)
assert len(rgba)==240*160*4
out=root/'profile'
(out/'background-composite.rgba').write_bytes(rgba)
write_png(out/'background-composite.png',240,160,rgba)
write_png(out/'background-composite-full.png',512,256,full)
(out/'background-composite.json').write_text(json.dumps(dict(rom_sha256=EXPECTED,descriptor='080340d8',palette_owner='080b458c',front_priority=0,back_priority=1,blend_control='1f4f',coefficients=[8,8],viewport=[240,160],scroll=[0,0],blended_pixels=blended,front_nonzero_in_view=sum(front[y*512+x]!=0 for y in range(160) for x in range(240)),qualification='Static BG-only source composition; source OBJ/button prompts and palette-frame scheduling still require native renderer integration',rgba_sha256=hashlib.sha256(rgba).hexdigest()),indent=2)+'\n')
print('Recovered source profile BG composition:',len(rgba),'bytes, blended full-map pixels',blended)
