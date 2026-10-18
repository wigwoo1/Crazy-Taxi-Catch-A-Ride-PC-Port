"""Locate original attract palette submission and audit opaque blue source pixels."""
exec(open('outputs/crazy-taxi-native/tools/probe_effect_state.py').read().split('for off in range(0,0x34000,2):')[0])
import json,collections
lines=['ROM SHA verified; selected contexts, literal pools remain DATA.']
for start,end in [(0xac38,0xacb0),(0xacb0,0xad00),(0x27ae0,0x27bb0),(0x27bb0,0x27cd0)]:
 lines.append(f'RANGE {start:x}..{end:x}')
 for i in md.disasm(r[start:end],0x8000000+start):
  row=f'{i.address:08x}: {i.mnemonic} {i.op_str}'
  h=struct.unpack_from('<H',r,i.address-0x8000000)[0]
  if h&0xf800==0x4800:
   p=((i.address-0x8000000+4)&~3)+(h&255)*4
   row+=f' literal DATA={struct.unpack_from("<I",r,p)[0]:08x}'
  lines.append(row)
pathlib.Path('outputs/crazy-taxi-native/reports/attract-palette-owner-source.txt').write_text('\n'.join(lines))
base=0x5a338;g=base+struct.unpack_from('<I',r,base+8)[0]
count,bits,tiles,start,pals=struct.unpack_from('<5I',r,g);assert count==155 and bits==16 and pals==1
pal=g+20+count*4;pix=pal+32
values=struct.unpack_from('<16H',r,pal)
records=[]
for char in '1F$0':
 glyph=ord(char)-32;a=base+struct.unpack_from('<I',r,g+20+glyph*4)[0];m=struct.unpack_from('<9H',r,a)
 hist=collections.Counter()
 for y in range(m[1]*8):
  for x in range(m[0]*8):
   p=pix+(m[8]+(y//8)*m[0]+x//8)*32+(y&7)*4+(x&7)//2
   hist[(r[p]>>(4*(x&1)))&15]+=1
 records.append({'character':char,'indices':dict(hist)})
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'font1_palette_address':hex(pal+0x8000000),'rgb555_palette':[hex(v) for v in values],'glyph_index_histograms':records,'scope':'Raw ROM nonzero indices are opaque under native extraction; no color-key change is justified by visual appearance alone. Original registration/upload and later palette mutation require source qualification.'}
pathlib.Path('outputs/crazy-taxi-native/reports/attract-font1-palette-audit.json').write_text(json.dumps(report,indent=2))
print(json.dumps(report,indent=2));print('\n'.join(lines))
