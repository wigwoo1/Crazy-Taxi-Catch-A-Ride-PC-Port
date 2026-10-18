"""Original complete pause controller, ordinary menu states; helpers opaque."""
exec(open('outputs/crazy-taxi-native/tools/compare_customer_dispatch_rom.py').read().split('\ncases=')[0])
import itertools
cfg=json.loads((root/'reports/pause-palette-request-owners-audit.json').read_text())['functions'][0]['instructions']
allowed_thumb.update({int(i['address'],16):bytes.fromhex(i['bytes']) for i in cfg})
class PauseProbe(DispatchProbe):
 def read(self,a,n=4):
  if 0x40000d4<=a<0x40000e0:
   assert a&~3 in self.arm.mem;return (self.arm.mem[a&~3]>>((a&3)*8))&((1<<(8*n))-1)
  return super().read(a,n)
 def write(self,a,v,n=4):
  if 0x40000d4<=a<0x40000e0:
   assert n==4;self.arm.mem[a]=v
   if a==0x40000dc:assert v==0x80000010;self.arm.mem[a]=v&0x7fffffff
  else:super().write(a,v,n)
 def external(self,target):
  assert target in [0x802773c,0x8027704,0x8027754,0x802771c,0x8027fb8,0x802c790,0x802c784,0x802b330,0x800e5a8],hex(target)
  self.requests.append([target,*self.reg[:4]])
cases=[(f,sel,d,no,outer,fade,ended,bits) for f,sel,d,no,outer,fade,ended,bits in itertools.product([0,1],[0,1,2],[0,1,2],[0,1],[0,2],[0,1],[0,1],[0,1,2,4,8,16,3,6,24]) if d==0 or sel==d]
cp=subprocess.run([str(root/'bin/pause-menu-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,s.split())) for s in cp.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,native in zip(cases,rows):
 f,sel,d,no,outer,fade,ended,bits=case;p=PauseProbe((0,0,0,0,0,1,0,0,0,0));p.requests=[]
 for i in cfg:
  if 'literal' in i:
   a=int(i['literal'],16)
   if 0x2000000<=a<0x2040000:
    for off in range(0,32,4):p.write((a&~3)+off,0)
 for off,v in [(0,f),(1,sel),(2,d),(3,no)]:p.write(0x2005f20+off,v,1)
 p.write(0x2001ea8,outer,1);p.write(0x200177b,fade,1);p.write(0x2001696,ended,1)
 for a,mask in [(0x2003503,1),(0x2003500,2),(0x2003501,4),(0x20034fc,8),(0x20034fd,16)]:p.write(a,int(bool(bits&mask)),1)
 p.reg[14]=0x10020001;p.run(entry=0x8021c5c,stop=0x10020000)
 actual=[p.read(a,n) for a,n in [(0x2005f20,1),(0x2005f21,1),(0x2005f22,1),(0x2005f23,1),(0x2001ea8,1),(0x2005f1a,1),(0x2005f24,1),(0x2005f19,1),(0x2005f1c,2),(0x2005f1e,2)]]
 pause_audio=int(any(q[0]==0x802773c for q in p.requests));resume_audio=int(any(q[0]==0x8027754 for q in p.requests))
 art=0
 for q in p.requests:
  if q[0]==0x8027fb8:
   source_pointer=q[1];bank=next(j+1 for j,off in enumerate([0x7ec36c,0x7ec380,0x7ec394]) if struct.unpack_from('<I',r,off)[0]==source_pointer)
   assert art in (0,bank);art=bank
 fade_out=int(any(q[0]==0x800e5a8 for q in p.requests));camera_refresh=int(any(q[0]==0x802c790 for q in p.requests))
 actual += [pause_audio,resume_audio,art,fade_out,camera_refresh]
 assert actual==native,(case,actual,native);steps+=p.steps
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':len(cases),'instructions':steps,'equal':True,'source_sha256':{name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ['src/pause_menu.c','src/pause_menu.h','tests/pause_menu_rom_compare.c','tools/compare_pause_menu_rom.py']},'scope':'Whole original21C5C throughreturn ordinary pause/Resume/Retry/Quit logical state plus audio/art/fade/camera request events versusnative pause_menu.c. Audio/image/camera/fade callees explicit captured request boundaries; DMA request observed without hardware copy. Debug camera menu3, original device input and host integration excluded.'}
(root/'reports/pause-menu-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
