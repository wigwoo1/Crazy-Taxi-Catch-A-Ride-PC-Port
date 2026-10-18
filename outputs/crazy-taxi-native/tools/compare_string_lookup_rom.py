"""Whole28FE4 source lookup with original selected English/null table."""
exec(open('outputs/crazy-taxi-native/tools/compare_normal_exit_rom.py').read().split('\ncases=')[0])
from capstone import Cs,CS_ARCH_ARM,CS_MODE_THUMB
for start,end in [(0x28fe4,0x29008),(0x29010,0x29016)]:
 for i in Cs(CS_ARCH_ARM,CS_MODE_THUMB).disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class LookupProbe(ExitProbe):
 def read(self,a,size=4):
  if a==0x2009a88:assert size==1;return self.selected
  if a==0x2009a8c+self.selected*4:assert size==4;return self.table
  return super().read(a,size)
maximum=int.from_bytes(r[0x49dfe0:0x49dfe4],'little')
cases=[(table,i,selected) for table in [0,0x849dfe0] for i in [0,147,148,149,maximum,maximum+1,0x80000000,0xffffffff] for selected in [0,5,255]]
cp=subprocess.run([str(root/'bin/string-lookup-rom-compare.exe'),'Crazy Taxi - Catch a Ride (USA).gba'],input=''.join(f'{t} {i}\n' for t,i,s in cases),text=True,capture_output=True,check=True)
rows=[int(x) for x in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for (table,index,selected),want in zip(cases,rows):
 p=LookupProbe((0,0,0,0,0,1,0,0,0,0));p.selected=selected;p.table=table;p.reg[0]=index;p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x8028fe4,stop=0x10020000);assert p.reg[0]==want,((table,index,selected),p.reg[0],want);steps+=p.steps
labels=[]
for i in [148,149]:
 off=0x49dfe0+int.from_bytes(r[0x49dfe4+i*4:0x49dfe8+i*4],'little');end=r.index(0,off);labels.append(dict(index=i,identity=hex(off+0x8000000),bytes_hex=r[off:end].hex()))
report=dict(equal=True,cases=len(cases),thumb_instructions=steps,loading_labels=labels,rom_sha256=hashlib.sha256(r).hexdigest(),scope='Whole28FE4 null/actualEnglish table inclusive max and unsigned index boundaries, source selected-byte slot resolution0/5/255 contract. NativeAPI takes resolved table identity; table registration/selection and text rasterization excluded.',source_sha256={n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/string_lookup.c','src/string_lookup.h','tests/string_lookup_rom_compare.c','tools/compare_string_lookup_rom.py']})
(root/'reports/string-lookup-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
