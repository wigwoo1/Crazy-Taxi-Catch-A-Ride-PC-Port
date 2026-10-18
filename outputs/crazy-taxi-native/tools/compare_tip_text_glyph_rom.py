"""Original F7DC/FBAC glyph requests vs native tip placement; pixels excluded."""
exec(open('outputs/crazy-taxi-native/tools/compare_tip_text_measure_rom.py').read().split('\nrows=')[0])
pending=[0x800f7dc];seen=set()
while pending:
 a=pending.pop()
 if a in seen:continue
 assert 0x800f7dc<=a<0x800fbac,hex(a);seen.add(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));allowed_thumb[a]=i.bytes
 if i.mnemonic=='bl':pending.append(a+i.size)
 elif i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 elif i.mnemonic.startswith('b') and i.mnemonic!='bics':
  pending.append(int(i.op_str.lstrip('#'),16))
  if i.mnemonic!='b':pending.append(a+i.size)
 else:pending.append(a+i.size)
class GlyphProbe(MeasureProbe):
 def external(self,target):
  if target in [0x800fbac,0x8028e3c]:self.run(entry=target,stop=self.reg[14]&~1)
  elif target==0x8028288:
   g,group,xy=self.reg[:3];assert group==0
   container=0x8070e50;desc=container+self.read(container+4);glyph=container+self.read(desc+20+g*4)
   ox=self.read(glyph+4,2);oy=self.read(glyph+6,2);ox=ox if ox<32768 else ox-65536;oy=oy if oy<32768 else oy-65536
   self.glyphs.append([g,signed(self.read(xy))-ox,signed(self.read(xy+4))-oy])
   self.write(0x2007e2c,self.read(glyph,2)*8,1)
  else:super().external(target)
rows=[list(map(int,l.split())) for l in (root/'reports/current-reward-updates.txt').read_text().splitlines()];cases=set()
for row in rows:
 tips=row[16:576]
 for n in range(10):
  t=tips[n*56:(n+1)*56]
  if not t[2]:continue
  whole=int.from_bytes(bytes(t[4:8]),'little',signed=True);frac=t[1] if t[1]<128 else t[1]-256;tens=trunc(frac,10)
  cases.add((int.from_bytes(bytes(t[32:36]),'little',signed=True)>>8,int.from_bytes(bytes(t[36:40]),'little',signed=True)>>8,'$%d.%d%d'%(whole,tens,frac-tens*10)))
# Controlled negative/edge anchors exercise source truncation and screen clipping inputs.
texts={c[2] for c in cases}
for text in texts:
 for x,y in [(-1,-1),(0,0),(1,1),(239,159),(-240,-160)]:cases.add((x,y,text))
cases=sorted(cases);native=subprocess.run([str(root/'bin/tip-text-glyph-compare.exe')],input=''.join('%d %d %s\n'%c for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(native)==len(cases);steps=glyphs=0
for case,line in zip(cases,native):
 x,y,text=case;p=GlyphProbe((0,0,0,0,0,1,0,0,0,0));p.glyphs=[]
 for a,v in [(0x2007e38,0x8070e50),(0x2001e18,0),(0x2001e1c,32),(0x2001e20,0x8415bbc),(0x2001e24,1)]:p.write(a,v,4)
 for j,v in enumerate(text.encode()+b'\0'):p.write(0x2008000+j,v,1)
 p.reg[:4]=[0,x&MASK,y&MASK,0x2008000]
 for j,v in enumerate([0,0,36,2,256]):p.write(p.reg[13]+j*4,v,4)
 p.run(entry=0x800f7dc,stop=0x10020000);want=list(map(int,line.split()));actual=[len(p.glyphs)]+[v for g in p.glyphs for v in g];assert actual==want,(case,actual,want);steps+=p.steps;glyphs+=len(p.glyphs)
report=dict(cases=len(cases),glyphs=glyphs,thumb_instructions=steps,match=True,scope='CompleteoriginalF7DC + actualFBAC/28E3C, font0 flags36 scale256; capturedactive tip positions plus20 controlled edgeanchors. 28288 glyph dispatch boundary applies original metric offsets and tilewidth sideeffect; pixels/palette/clipping/OAM not proved.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/tip-text-glyph-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',report)
