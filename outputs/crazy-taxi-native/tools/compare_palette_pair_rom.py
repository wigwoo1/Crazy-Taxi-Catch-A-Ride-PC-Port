"""Complete original2C688 nearest-pair helper, no opaque called helpers."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True;todo=[0x802c688];ins={}
while todo:
 a=todo.pop()
 if a in ins:continue
 assert 0x802c688<=a<0x802c784,hex(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));ins[a]=i;allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='bx':continue
 assert i.mnemonic!='bl'
 if i.mnemonic.startswith('b'):
  todo.append(int(i.op_str.lstrip('#'),16))
  if i.mnemonic!='b':todo.append(a+i.size)
 else:todo.append(a+i.size)
context_slot=int.from_bytes(r[0x2c6f4:0x2c6f8],'little');assert context_slot==0x3006810
class PairProbe(ExitProbe):
 def read(self,a,size=4):
  if a==context_slot:assert size==4;return 0x2009000
  return super().read(a,size)
 def external(self,target):raise AssertionError(hex(target))
palettes=[[0]*256,[((i*131)^((i%5)*977))&65535 for i in range(256)],[int.from_bytes(r[0x4a5a34+2*i:0x4a5a36+2*i],'little') for i in range(256)]]
colors=[(a,b,c) for a in [0,31] for b in [0,31] for c in [0,31]]+[(15,15,15)]
cases=[(palette,color) for palette in palettes for color in colors]
cp=subprocess.run([str(root/'bin/palette-pair-rom-compare.exe')],input=''.join(' '.join(map(str,[*color,*palette]))+'\n' for palette,color in cases),text=True,capture_output=True,check=True)
rows=list(map(int,cp.stdout.split()));assert len(rows)==len(cases);steps=0
for (palette,color),want in zip(cases,rows):
 p=PairProbe((0,0,0,0,0,1,0,0,0,0));p.reg[0:3]=color;p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.write(0x2009000+24,0x2009100)
 p.write(0x2009064,0x12345678)
 for j,value in enumerate(palette):p.write(0x2009100+2*j,value,2)
 p.run(entry=0x802c688,stop=0x10020000)
 actual=p.read(0x2009064);assert actual==want,(color,actual,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Complete original2C688 function with no external helper calls. All-equal tie palette, patterned high-bit palette and actual source-city palette; RGB cube corners and mid-gray. Output context word64 exact, excludes index0, two distinct nearest indices and strict first-index ties. Context pointer fixture explicit; full graphics/restart integration excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/shade_table.c','src/shade_table.h','tests/palette_pair_rom_compare.c','tools/compare_palette_pair_rom.py']})
(root/'reports/palette-pair-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)

