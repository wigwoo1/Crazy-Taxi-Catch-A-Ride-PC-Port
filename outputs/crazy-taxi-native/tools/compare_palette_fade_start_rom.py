"""Original full E5A8..EAC0 normal mode initialization, before palette body."""
exec(open('outputs/crazy-taxi-native/tools/compare_palette_fade_setup_rom.py').read().split('\nvalues=')[0])
todo=[0x0800e8ee];seen2=set()
while todo:
 a=todo.pop()
 if a==0x0800eac2 or a in seen2:continue
 assert 0x0800e8ee<=a<0x0800eac2,hex(a);seen2.add(a)
 i=next(md.disasm(r[a-0x08000000:a-0x08000000+4],a,count=1));allowed_thumb[a]=bytes(i.bytes)
 if i.mnemonic=='b':todo.append(i.operands[0].imm);continue
 todo.append(a+i.size)
 if i.group(CS_GRP_JUMP):todo.append(i.operands[0].imm)
class StartProbe(SetupProbe):
 def __init__(self,*args):self.motion=bytearray([0xa5]*24);self.hardware={0x04000050:0x1234,0x04000054:0x2345};self.dimensions={};super().__init__(*args)
 def read(self,a,size=4):
  if a in self.hardware:return self.hardware[a]
  if a in self.dimensions:return self.dimensions[a]
  if 0x020094f0<=a<0x02009508:return int.from_bytes(self.motion[a-0x020094f0:a-0x020094f0+size],'little')
  return super().read(a,size)
 def write(self,a,v,size=4):
  if a in self.hardware:self.hardware[a]=v&((1<<(size*8))-1)
  elif 0x020094f0<=a<0x02009508:self.motion[a-0x020094f0:a-0x020094f0+size]=(v&((1<<(size*8))-1)).to_bytes(size,'little')
  else:super().write(a,v,size)
values=[0,1,3,4,8,16,32,64,128,256,0x1000,0x2000,0xffff]
cases=[(m,cap,bg,obj,240,160) for m,cap in [(0,0),(1,32),(256,33),(257,255)] for bg,obj in zip(values,reversed(values))]
rng=random.Random(0xe8ee)
cases += [((rng.getrandbits(32)&~255)|rng.randrange(2),rng.getrandbits(32),rng.getrandbits(32),rng.getrandbits(32),rng.getrandbits(32),rng.getrandbits(32)) for _ in range(128)]
run=subprocess.run([str(root/'bin/palette-fade-start-rom-compare.exe')],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True);rows=[list(map(int,l.split())) for l in run.stdout.splitlines()];assert len(rows)==len(cases);steps=0
for case,want in zip(cases,rows):
 p=StartProbe((0,0,0,0,0,1,0,0,0,0));p.globals=bytearray([0xa5]*32);before=bytes(p.globals)
 p.dimensions={0x02007542:case[4]&65535,0x02007544:case[5]&65535};p.reg[:4]=case[:4];p.reg[13]=0x10000400;p.reg[14]=0x10020001
 p.run(entry=0x0800e5a8,stop=0x0800eac2)
 read=lambda off,size=1:p.read(0x02001770+off,size)
 got=[read(3),read(28,2),read(4),read(22,2),read(24,2)]+[read(i) for i in range(5,11)]
 progress=read(26,2);got += [progress if progress<32768 else progress-65536,read(3),read(11),read(4),read(1),read(2),read(12,2),read(14,2),p.hardware[0x04000050],p.hardware[0x04000054]]
 got += [signed(p.read(0x020094f0+i*4)) for i in range(6)]
 assert got==want,(case,got,want)
 for i in [0,1,2,16,17,18,19,20,21,30,31]:assert p.globals[i]==before[i],(case,i)
 steps+=p.steps
report={'status':'PASS','cases':len(cases),'actual_thumb_instructions':steps,'scope':'Complete E5A8..EAC0 normal modes0/1 initialization including mask/channel prefix, stored cap/masks/mode/active/progress, all motion fields, mode1 scale writes/mode0 retention, optional BLDCNT/BLDY writes/otherwise retention, timing/count/cursors/1770 retained. Registered palette processing EAC2 onward/GPU presentation excluded; other modes excluded.','rom_sha256':hashlib.sha256(r).hexdigest()}
(root/'reports/palette-fade-start-rom-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
