"""Original attract graphics caller to AD66; three resource contracts explicit."""
exec(open('outputs/crazy-taxi-native/tools/compare_title_parent_rom.py').read().split('class ParentProbe')[0])
for start,end in [(0xacc6,0xad02)]:
 for i in md.disasm(r[start:end],0x8000000+start):allowed_thumb[i.address]=bytes(i.bytes)
class GraphicsProbe(InitProbe):
 run=MotionProbe.run
 def external(self,target):
  assert target in [0x8027994,0x800fec8]
  self.calls.append([target,*self.reg[:2],0,0,0,0] if target==0x8027994 else [target,*self.reg[:4],self.read(self.reg[13]),self.read(self.reg[13]+4)])
  self.reg[0]=0
binary=root/'bin/attract-graphics-rom-compare.exe';rows=[list(map(int,l.split())) for l in subprocess.run([str(binary)],capture_output=True,text=True,check=True).stdout.splitlines()]
p=GraphicsProbe((0,0,0,0,0,1,0,0,0,0));p.calls=[];p.write(0x2001708,1,1);p.reg[10]=1;p.reg[13]=0x10000400;p.write(p.reg[13]+28,0);saved=p.reg[4:12].copy();p.run(entry=0x800acc6,stop=0x800ad66)
assert p.calls==rows and len(rows)==3;assert p.reg[13]==0x10000400 and p.reg[10]==1
report={'status':'PASS','Thumb_instructions':p.steps,'calls':rows,'scope':'Actualoverride-enabledACC6..AD66 caller branch; retainedsl1/stacklocal0 from earlierentry. Exactthree helper targets and usedarguments inclstack. All27994/FEC8 effects returningcontracts; noassetpixels/nativehelper/frameproof.','hashes':{str(q):hashlib.sha256(q.read_bytes()).hexdigest() for q in [Path(__file__),root/'src/attract_graphics.c',binary]}}
(root/'reports/attract-graphics-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS original attract graphics branch',p.steps,'Thumb')
