"""Complete original tip text dispatcher and glyph OAM builder."""
exec(open('outputs/crazy-taxi-native/tools/compare_tip_text_glyph_rom.py').read().split('\nrows=')[0])
jump=[int.from_bytes(r[a:a+4],'little') for a in range(0x28344,0x28358,4)]
pending=[0x8028288];seen={}
while pending:
 a=pending.pop()
 if a in seen:continue
 assert 0x8028288<=a<0x8028656,hex(a);i=next(md.disasm(r[a-0x8000000:a-0x8000000+4],a,count=1));seen[a]=dict(address=hex(a),bytes=i.bytes.hex(),mnemonic=i.mnemonic,operands=i.op_str);allowed_thumb[a]=i.bytes
 if i.mnemonic=='mov' and i.op_str=='pc, r0':pending+=jump
 elif i.mnemonic=='bx' or (i.mnemonic=='pop' and 'pc' in i.op_str):continue
 elif i.mnemonic.startswith('b') and i.mnemonic!='bics':
  pending.append(int(i.op_str.lstrip('#'),16))
  if i.mnemonic!='b':pending.append(a+i.size)
 else:pending.append(a+i.size)
saved=allowed_thumb.copy();source=source.replace('a=val(ops[1]);n=val(ops[2]);','a=val(ops[1]) if len(ops)==3 else self.reg[rn(ops[0])];n=val(ops[2]) if len(ops)==3 else val(ops[1]);');source=source.replace("if m=='movs':self.nz(value)","if m=='movs':self.nz(value)\n    if rn(ops[0])==15:nextpc=value&~1");source=source.replace("   elif m=='cmp':", "   elif m=='cmn':self.arithmetic(val(ops[0]),val(ops[1]),False)\n   elif m=='cmp':");exec(source);allowed_thumb.update(saved)
class OamProbe(GlyphProbe):
 def external(self,target):
  if target==0x8028288:self.run(entry=target,stop=self.reg[14]&~1)
  else:super().external(target)
OamProbe.run=MotionProbe.run
(root/'reports/tip-font-oam-cfg.json').write_text(json.dumps(dict(instructions=[seen[a] for a in sorted(seen)],jump_table=[hex(a) for a in jump],scope='Staticcomplete28288 includingoriginal5-waymovPC jump table.',rom_sha256=hashlib.sha256(r).hexdigest()),indent=2))
font=json.loads((root/'assets/hud/font.json').read_text());rgba=(root/'assets/hud/font.rgba').read_bytes();steps=cases=glyphs=0;examples=[]
for text in ['$0.50','$0.75','$1.25','$4.00']:
 for color in [1,2,3]:
  for x,y in [(120,80),(0,0),(239,159)]:
   p=OamProbe((0,0,0,0,0,1,0,0,0,0))
   for a,v,n in [(0x2001788,0,2),(0x2007e38,0x8070e50,4),(0x2001e18,0,4),(0x2001e1c,32,4),(0x2001e20,0x8415bbc,4),(0x2001e24,1,4),(0x2007542,240,2),(0x2007544,160,2),(0x2007f54,0,4),(0x2007e3c,0x6010000,4),(0x2007e2e,0,1),(0x2007e2f,0,1)]:p.write(a,v,n)
   for i in range(3):p.write(0x2007e32+i,0,1)
   for i,v in enumerate(text.encode()+b'\0'):p.write(0x2008000+i,v,1)
   p.reg[:4]=[0,x,y,0x2008000]
   for i,v in enumerate([0,0,36,color,256]):p.write(p.reg[13]+i*4,v)
   p.run(entry=0x800f7dc,stop=0x10020000);count=p.read(0x2007e32,1);requests=[[p.read(0x20081ec+i*8+j*2,2) for j in range(4)] for i in range(count)]
   # Native original-metrics dispatcher projected glyph requests; clipping uses actual source tile dimensions.
   result=subprocess.run([str(root/'bin/tip-text-glyph-compare.exe')],input='%d %d %s\n'%(x,y,text),text=True,capture_output=True,check=True);values=list(map(int,result.stdout.split()));native=[]
   for i in range(values[0]):
    g,gx,gy=values[1+i*3:4+i*3];m=font['glyphs'][g];w,h=m['tiles'];w*=8;h*=8
    if gx>=240 or gy>=160 or gx<=-w or gy<=-h:continue
    assert w==h==8,(g,w,h)
    native.append([gy&255,gx&511,(color<<12)|m['tile'],0])
   assert requests==native,dict(text=text,color=color,xy=(x,y),original=requests,native=native)
   steps+=p.steps;cases+=1;glyphs+=count
   if (x,y)==(120,80):examples.append(dict(text=text,color=color,requests=requests))
report=dict(cases=cases,oam_requests=glyphs,thumb_instructions=steps,match=True,dollar_tile=font['glyphs'][4]['tile'],dollar_opaque_pixels=[sum(rgba[(b*224+4)*1024+i]>0 for i in range(3,1024,4)) for b in range(4)],examples=examples,scope='CompleteoriginalF7DC+FBAC+28E3C+28288 throughreturn vsnativeglyphplacement/clipping/tile/palette OAMfields, font0flags36scale256. Explicitgroup0registration/VRAMbase/viewport/queue/palette fixtures. ActualOAMraster pixels/sourceownership/fullHUDorder unverified.',rom_sha256=hashlib.sha256(r).hexdigest())
(root/'reports/tip-font-oam-rom-comparison.json').write_text(json.dumps(report,indent=2));print('PASS',{k:v for k,v in report.items() if k!='examples'})
