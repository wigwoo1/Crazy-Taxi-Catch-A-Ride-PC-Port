"""Original whole pause overlay caller versus native layout requests."""
exec(open('outputs/crazy-taxi-native/tools/compare_customer_dispatch_rom.py').read().split('\ncases=')[0])
import itertools
cfg=json.loads((root/'reports/pause-palette-request-owners-audit.json').read_text())['functions'][1]['instructions']
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg})
class RenderProbe(DispatchProbe):
 def external(self,target):
  if target==0x8028fe4:
   assert self.reg[0] in (451,452,453,457,458);self.text_id=self.reg[0]
   self.reg[0]=self.text_id # Explicit localized string token contract; text renderer opaque.
  elif target==0x8028288:
   pos=self.reg[2];assert self.reg[3]==0 and self.read(pos+8)==0
   assert [self.read(self.reg[13]+k) for k in [0,4,8]]==[0,0,0]
   self.requests.append([0,self.reg[0],signed(self.read(pos)),signed(self.read(pos+4)),self.reg[1]])
  elif target==0x800f7dc:
   assert self.reg[0]==0 and self.reg[3]==self.text_id
   assert [self.read(self.reg[13]+k) for k in [0,4,8,16]]==[0,0,4,256]
   self.requests.append([1,self.text_id,self.reg[1],self.reg[2],self.read(self.reg[13]+12)])
  else:raise AssertionError(('unqualified overlay helper',hex(target)))
cases=list(itertools.product([0,1],[0,1,2],[0,1,2],[0,1],[0,2],[0,1],[240,320]))
cp=subprocess.run([str(root/'bin/pause-render-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,s.split())) for s in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=requests=0
for case,native in zip(cases,rows):
 frozen,selection,dialog,no,outer,fade,width=case;p=RenderProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[]
 for i in cfg:
  if 'literal' in i:
   a=int(i['literal'],16)
   if 0x2000000<=a<0x2040000:
    for off in range(0,32,4):p.write((a&~3)+off,0)
 for off,v in [(0,frozen),(1,selection),(2,dialog),(3,no)]:p.write(0x2005f20+off,v,1)
 p.write(0x2001ea8,outer,1);p.write(0x200177b,fade,1);p.write(0x2007542,width,2)
 p.reg[14]=0x10020001;p.run(entry=0x80221f8,stop=0x10020000)
 actual=[p.read(0x2005f24,1),len(p.requests)]+[v for q in p.requests for v in q]
 assert actual==native,(case,actual,native);steps+=p.steps;requests+=len(p.requests)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':len(cases),'requests':requests,'instructions':steps,'equal':True,'source_sha256':{name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['src/pause_render.c','src/pause_render.h','tests/pause_render_rom_compare.c','tools/compare_pause_render_rom.py','tools/extract_pause_art.py','assets/pause/pause.json']},'scope':'Whole original221F8 throughreturn ordinarydialog0/1/2, gate/state/width240or320fixtures, sprite/text order positions/highlight/caller flags and palette request exact. Localizationlookup explicit idtoken contract;28288/F7DC renderer boundaries. No original pixels/OAM blend or debugdialog3;320 sourcecontext extension fixture not originalGBA widescreen proof.'}
(root/'reports/pause-render-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
