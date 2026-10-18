from pathlib import Path
import sys,json,struct,subprocess
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/compare_captured_flat_row_batch_rom.py').read_text().split('scope={}')[0])
s=s.replace('assert 0x1f14<=off<0x1f88,hex(pc)','assert 0x1dd4<=off<0x1f88 or 0x29f8<=off<0x2ce8,hex(pc)')
s=s.replace("('ldr','ldrb','str','strh')", "('ldr','ldrb','ldrh','str','strh')").replace("2 if m=='strh' else 4", "2 if m in ('ldrh','strh') else 4")
s=s.replace("'ands'):","'ands','orr'):").replace("elif m=='orrs':", "elif m=='orr':x=a|b\n    elif m=='orrs':")
s=s.replace("elif m in ('b','bl'):","elif m=='mla':\n    self.reg[self.ri(p[0])]=(self.val(p[1])*self.val(p[2])+self.val(p[3]))&MASK\n   elif m in ('b','bl'):")
scope={};exec(s,scope);Base=scope['MarkerRasterProbe'];BASE=scope['BASE'];FB=scope['FB'];PACK=scope['PACK'];MASK=scope['MASK']
class Probe(Base):
 def __init__(self,p,pattern):
  self.track=False;super().__init__(0x08000000,(0,0,2,2),False);self.half=self.word=0
  self.blocks.append((0x020344f0,bytearray(160*32)))
  self.entry=BASE+0x1dd4-0x700;self.reg[0]=len(p['vertices']);self.reg[1]=PACK;self.reg[3]=PACK+300
  self.write(PACK+300,pattern,2)
  for i,v in enumerate(p['vertices']):
   sx,sy,x,y,z,u,vv,attr=v
   for k,value in enumerate([sx,u,vv,sy,x,y,z,attr]):self.write(PACK+i*32+k*4,value)
  for off,value in [(0x1db0,0),(0x1db4,0),(0x1db8,240),(0x1dbc,160),(0x1dc4,FB)]:self.write(BASE+off-0x700,value)
  self.track=True
 def write(self,a,v,n=4):
  if self.track and FB<=a<FB+38400:
   assert n in (2,4)
   if n==2:self.half+=1
   else:self.word+=1
  return super().write(a,v,n)
j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());data=(root/'assets/city1.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',data,4);mb=20+nv*16+np*24;cases=[]
for p in j['packets']:
 if p['callback']=='03006950':cases.append((p,struct.unpack_from('<H',data,mb+p['material']*28+14)[0]))
assert len(cases)==6
cp=subprocess.run([str(root/'bin/source240-flat-polygon-probe.exe')],input=''.join(' '.join(map(str,[len(p['vertices']),color,*[value for v in p['vertices'] for value in v[:2]]]))+'\n' for p,color in cases).encode(),capture_output=True,check=True);assert len(cp.stdout)==len(cases)*38408
steps=half=word=0
for i,(packet,color) in enumerate(cases):
 p=Probe(packet,color);pixels=p.run()
 assert pixels+struct.pack('<2I',p.half,p.word)==cp.stdout[i*38408:(i+1)*38408],packet['source_index']
 assert (p.half,p.word)==(packet['flat_halfword_writes'],packet['flat_word_writes'])
 steps+=p.steps;half+=p.half;word+=p.word
report={'status':'PASS','actual_flat_packets':len(cases),'framebuffer_bytes':len(cases)*38400,'flat_halfword_writes':half,'flat_word_writes':word,'original_arm_instructions':steps,'scope':'Original flat1DD4 entry through1F88 beforeepilogue including edgepreparation and actualoriginaldivision, realmaterialpatterns/actualsource240preparedgeometry vs sharednativefullflatpolygon CPU. Fresh173fb/zeroedgeworkspace/patchedsourcebounds. Includes clippedprepared7181; wrappergeometry separatelyverified. Notwholeframe/cycles/cadenceproof.'};(root/'reports/source240-full-flat-polygon-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
