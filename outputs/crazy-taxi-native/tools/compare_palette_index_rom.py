"""Actual2C558 through fill-index owner assignment; excludes buffer clearing."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
md=Cs(CS_ARCH_ARM,CS_MODE_THUMB);md.detail=True;todo=[0x802c558];seen=set()
while todo:
 a=todo.pop()
 if a==0x802c632 or a in seen:continue
 assert 0x802c558<=a<0x802c632,hex(a);seen.add(a)
 i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic.startswith('b'):
  target=int(i.op_str.lstrip('#'),16)
  if target!=0x802c676:todo.append(target)
  if i.mnemonic!='b':todo.append(a+i.size)
 else:todo.append(a+i.size)
mode_slot=int.from_bytes(r[0x2c5b0:0x2c5b4],'little');context_slot=int.from_bytes(r[0x2c5b4:0x2c5b8],'little')
class IndexProbe(ExitProbe):
 def read(self,a,size=4):
  if a==mode_slot:return 4
  if a==context_slot:return 0x2009000
  if a==0x3006810:return 0x2009000
  return super().read(a,size)
 def write(self,a,value,size=4):
  if a==0x3006900:self.selected=value
  else:super().write(a,value,size)
 def external(self,target):raise AssertionError(hex(target))
palettes=[[0]*256,[((i*131)^((i%5)*977))&65535 for i in range(256)],[int.from_bytes(r[0x4a5a34+2*i:0x4a5a36+2*i],'little') for i in range(256)]]
colors=[(a,b,c) for a in [0,31] for b in [0,31] for c in [0,31]]+[(15,15,15)]
cases=[(p,c) for p in palettes for c in colors]
cp=subprocess.run([str(root/'bin/palette-index-rom-compare.exe')],input=''.join(' '.join(map(str,[*c,*p]))+'\n' for p,c in cases),text=True,capture_output=True,check=True)
rows=list(map(int,cp.stdout.split()));assert len(rows)==len(cases);steps=0
for (palette,color),want in zip(cases,rows):
 p=IndexProbe((0,0,0,0,0,1,0,0,0,0));p.selected=None;p.reg[:3]=color;p.reg[13]=0x10000400;p.write(0x2009018,0x2009100)
 for j,v in enumerate(palette):p.write(0x2009100+2*j,v,2)
 p.run(entry=0x802c558,stop=0x802c632)
 assert p.selected==want,(color,p.selected,want);steps+=p.steps
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,rom_sha256=hashlib.sha256(r).hexdigest(),mode_slot=hex(mode_slot),context_slot=hex(context_slot),scope='Actual2C558 entry/mode4/nonnull gate plus complete256-color Manhattan search and03006900 owner store. Includes index0, strict ties, original citypalette; mode/pointer fixtures explicit. Excludes remainder632..684 two-buffer fills and host/GPU bootstrap.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/shade_table.c','src/shade_table.h','tests/palette_index_rom_compare.c','tools/compare_palette_index_rom.py']})
(root/'reports/palette-index-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
