"""Complete B6EC including original1E7F0 settings copy; all owned bytes."""
exec(open('outputs/crazy-taxi-native/tools/compare_effect_pool_reset_rom.py').read().split('\nseeds=')[0])
for entry,end in [(0x0800b6ec,0x0800b9f6),(0x0801e7f0,0x0801e85e)]:
 todo=[entry];seen=set()
 while todo:
  a=todo.pop()
  if a in seen:continue
  assert entry<=a<=end,hex(a);seen.add(a)
  i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
  if i.mnemonic=='bx':continue
  if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
  todo.append(a+i.size)
  if i.mnemonic.startswith('b') and i.mnemonic!='bl':todo.append(i.operands[0].imm)
regions=[(0x02001534,8),(0x0200165e,174),(0x02002618,24),(0x02005f20,12),(0x02005310,6),(0x02007530,16),(0x02001eb0,512),(0x020020b0,512),(0x02005ee8,48)]
class InitProbe(ExitProbe):
 run=MotionProbe.run
 def __init__(self,*args):self.regions={a:bytearray(n) for a,n in regions};self.scalars={};super().__init__(*args)
 def read(self,a,size=4):
  for start,data in self.regions.items():
   if start<=a and a+size<=start+len(data):return int.from_bytes(data[a-start:a-start+size],'little')
  if a in self.scalars:return self.scalars[a]
  return super().read(a,size)
 def write(self,a,v,size=4):
  for start,data in self.regions.items():
   if start<=a and a+size<=start+len(data):data[a-start:a-start+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little');return
  if a in self.scalars:self.scalars[a]=v&((1<<(size*8))-1);return
  super().write(a,v,size)
 def external(self,t):
  assert t==0x0801e7f0,hex(t);self.run(entry=t,stop=self.reg[14]&~1)
cases=[(mode|low,seconds,alternate,challenge,width,seed) for mode in range(0,256,32) for low,seconds,alternate,challenge,width,seed in [(0,0,0,0,240,0),(31,255,1,0,65535,165),(1,90,0,1,0,255),(15,3,255,255,257,12345)]]
rows=subprocess.run([str(root/'bin/session-init-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True).stdout.splitlines();assert len(rows)==len(cases);steps=0
for case,row in zip(cases,rows):
 mode,seconds,alternate,challenge,width,seed=case;p=InitProbe((0,0,0,0,0,1,0,0,0,0))
 for a,n in regions:p.regions[a]=bytearray((seed+i*37)&255 for i in range(n))
 p.scalars={0x02001ea8:seed&255,0x0200430c:seed&65535,0x030067d0:seed,0x02007542:width&65535}
 p.write(0x02001eb0,mode,1);p.write(0x02001eb5,seconds,1);p.write(0x02001708,alternate,1);p.write(0x02001537,challenge,1)
 p.reg[13]=0x10000400;p.reg[14]=0x10020001;p.run(entry=0x0800b6ec,stop=0x10020000)
 parts=row.split();data=b''.join(bytes(p.regions[a]) for a,n in regions[:-1]);assert data==bytes.fromhex(parts[0]),(case,next((i for i,(a,b) in enumerate(zip(data,bytes.fromhex(parts[0]))) if a!=b),None))
 got=[p.scalars[a] for a in [0x02001ea8,0x0200430c,0x030067d0]]
 for i in range(12):assert p.read(0x02005ee8+i*4)==0,case;got.append(-1)
 assert got==list(map(int,parts[1:])),(case,got,parts);steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'CompleteB6EC includingwholeoriginal1E7F0 settings-copy, all1264ownedpackedbytes and3scalarowners/12null-to-native-1slots, all8modefamilies/retainedcountdown/alternate/challenge/widthsettings. No fullhost/gameplayframe equivalence claim.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/session-init-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
