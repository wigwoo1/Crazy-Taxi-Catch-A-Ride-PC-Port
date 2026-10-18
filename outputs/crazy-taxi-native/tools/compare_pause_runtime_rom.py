"""Persistent ordinary pause service composition; physical input supplied at boundary."""
exec(open('outputs/crazy-taxi-native/tools/compare_pause_menu_rom.py').read().split('\ncases=')[0])
menu_cfg=cfg;MenuProbe=PauseProbe
exec(open('outputs/crazy-taxi-native/tools/compare_pause_render_rom.py').read().split('\ncases=')[0])
render_cfg=cfg
for i in menu_cfg:allowed_thumb[int(i['address'],16)]=bytes.fromhex(i['bytes'])
for i in md.disasm(r[0x26320:0x26348],0x8026320):allowed_thumb[i.address]=bytes(i.bytes)
class CombinedProbe(MenuProbe):
 def external(self,target):
  if target in (0x8028fe4,0x8028288,0x800f7dc):RenderProbe.external(self,target)
  elif target==0x8022508:self.transfers.append(self.reg[0])
  else:super().external(target)
# Every press separated; held directions exercise native original input repeat.
cases=[(0,0,0,0),(8,1,0,0),(8,0,0,0),(0,0,0,0),(8,0,0,0)]+[(8,0,0,0)]*90
for mask in [8,8,128,1,2,1,8,8,64,1,1,128,1,64,1]:cases += [(0,0,0,0),(mask,0,0,0)]
cases += [(0,0,0,0),(128,0,0,0)]+[(128,0,0,0)]*100
cp=subprocess.run([str(root/'bin/pause-runtime-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(cases)
p=CombinedProbe((0,0,0,0,0,1,0,0,0,0))
for i in menu_cfg+render_cfg:
 if 'literal' in i:
  a=int(i['literal'],16)
  if 0x2000000<=a<0x2040000:
   for off in range(0,32,4):p.write((a&~3)+off,0)
p.write(0x2007542,240,2);steps=requests=0;alternate=0
for index,(case,row) in enumerate(zip(cases,rows)):
 before_steps=p.steps
 mask,fade,intro,ended=case;pressed=row[17:27];repeat=row[27:37]
 for j in range(10):p.write(0x2003500+j,pressed[j],1);p.write(0x20034f6+j,repeat[j],1)
 p.write(0x200177b,fade,1);p.write(0x2001696,intro,1);p.write(0x2001697,ended,1)
 p.requests=[];p.reg[14]=0x10020001;p.run(entry=0x8021c5c,stop=0x10020000)
 calls=p.requests;art=0
 for q in calls:
  if q[0]==0x8027fb8:art=next(j+1 for j,a in enumerate([0x7ec36c,0x7ec380,0x7ec394]) if struct.unpack_from('<I',r,a)[0]==q[1])
 events=[int(any(q[0]==t for q in calls)) for t in [0x802773c,0x8027754]]+[art]+[int(any(q[0]==t for q in calls)) for t in [0x800e5a8,0x802c790]]
 p.requests=[];p.reg[14]=0x10020001;p.run(entry=0x80221f8,stop=0x10020000)
 draws=p.requests;requests+=len(draws)
 p.transfers=[];p.run(entry=0x8026320,stop=0x8026348)
 steps+=p.steps-before_steps
 mode=p.transfers[0] if p.transfers else -1
 if mode!=-1:alternate=mode
 state=[p.read(a,n) for a,n in [(0x2005f20,1),(0x2005f21,1),(0x2005f22,1),(0x2005f23,1),(0x2001ea8,1),(0x2005f1a,1),(0x2005f24,1),(0x2005f19,1),(0x2005f1c,2),(0x2005f1e,2)]]
 actual=state+events+[mode,alternate]+pressed+repeat+[len(draws)]+[v for q in draws for v in q]
 assert actual==row,(index,case,actual,row)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'frames':len(rows),'thumb_instructions':steps,'draw_requests':requests,'equal':True,'source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ['src/pause_menu.c','src/pause_render.c','src/menu_input.c','src/palette_fade.c','src/pause_runtime.c','src/pause_runtime.h','tests/pause_runtime_test.c','tests/pause_runtime_rom_compare.c','tools/compare_pause_runtime_rom.py']},'scope':'Persistent original21C5C then221F8 then26320..26346 service composition versus native runtime. Input pressed/repeat explicit boundary from native menu_input, not original15C3C/device execution. Audio/art/camera/fade/draw/palette transfer captured boundaries; no full normal frame, host freezing, pixels or playable integration proof.'}
(root/'reports/pause-runtime-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
