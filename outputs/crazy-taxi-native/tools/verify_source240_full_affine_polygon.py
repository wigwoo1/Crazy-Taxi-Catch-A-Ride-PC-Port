from pathlib import Path
import sys,json,struct,subprocess
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/verify_source240_full_flat_polygon.py').read_text().split('\nscope={}')[0])
s=s.replace('while pc!=BASE+0x1f88-0x700:', 'while pc!=BASE+0x1980-0x700:').replace('assert 0x1dd4<=off<0x1f88 or 0x29f8<=off<0x2ce8,hex(pc)','assert 0x1730<=off<0x1980 or 0x29f8<=off<0x2ce8,hex(pc)')
s=s.replace('self.val(terms[1]) if len(terms)>1 else 0','self.operand(terms[1:]) if len(terms)>1 else 0')
s=s.replace("   elif m in ('ldr','ldrb','ldrh','str','strh'):","""   elif m in ('ldm','stm'):
    base=i.op_str.split(', ',1)[0].rstrip('!');idx=self.ri(base);a=self.reg[idx]
    names=i.op_str[i.op_str.index('{')+1:i.op_str.index('}')].split(', ')
    values=[self.read(a+j*4) for j in range(len(names))] if m=='ldm' else [self.val(name) for name in names]
    for j,name in enumerate(names):
     if m=='ldm':self.reg[self.ri(name)]=values[j]
     else:self.write(a+j*4,values[j])
    if i.writeback:self.reg[idx]=a+4*len(names)
   elif m in ('ldr','ldrb','ldrh','str','strh'):""")
scope={};exec(s,scope);scope['arm'].detail=True;Base=scope['MarkerRasterProbe'];BASE=scope['BASE'];FB=scope['FB'];PACK=scope['PACK']
class Probe(Base):
 def __init__(self,p,t):
  self.track=False;super().__init__(0x08000000,(0,0,2,2),False);self.reads=self.writes=0
  self.blocks.append((0x020344f0,bytearray(160*32)))
  self.entry=BASE+0x1730-0x700;self.reg[0]=len(p['vertices']);self.reg[1]=PACK;self.reg[2]=PACK+300;self.reg[3]=0
  self.write(PACK+300,t['width']|(t['height']<<8)|((t['width'].bit_length()-1)<<16));self.write(PACK+304,int(t['pixels'],16))
  for i,v in enumerate(p['vertices']):
   sx,sy,x,y,z,u,vv,attr=v
   for k,value in enumerate([sx,u,vv,sy,x,y,z,attr]):self.write(PACK+i*32+k*4,value)
  for off,value in [(0x1db4,0),(0x1dbc,160),(0x1dc4,FB)]:self.write(BASE+off-0x700,value)
  self.track=True
 def read(self,a,n=4):
  if self.track and 0x08000000<=a<0x08800000 and n==1:self.reads+=1
  return super().read(a,n)
 def write(self,a,v,n=4):
  if self.track and FB<=a<FB+38400:self.writes+=1;assert n==2
  return super().write(a,v,n)
j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures'];cases=[p for p in j['packets'] if p['callback']=='03006948'];assert len(cases)==15
cp=subprocess.run([str(root/'bin/source240-affine-polygon-probe.exe'),str(root/'assets/indexed-scenes/city1.cti')],input=''.join(' '.join(map(str,[len(p['vertices']),p['texture'],*[value&0xffffffff if i>=2 else value for v in p['vertices'] for i,value in enumerate([v[0],v[1],v[5],v[6],v[4]])]]))+'\n' for p in cases).encode(),capture_output=True,check=True);assert len(cp.stdout)==len(cases)*38408
reads=writes=steps=0
for i,packet in enumerate(cases):
 p=Probe(packet,textures[packet['texture']]);pixels=p.run()
 assert pixels+struct.pack('<2I',p.reads,p.writes)==cp.stdout[i*38408:(i+1)*38408],packet['source_index']
 assert (p.reads,p.writes)==(packet['texture_byte_reads'],packet['textured_halfword_writes'])
 reads+=p.reads;writes+=p.writes;steps+=p.steps
report={'status':'PASS','actual_affine_packets':len(cases),'framebuffer_bytes':len(cases)*38400,'texture_byte_reads':reads,'framebuffer_halfword_writes':writes,'original_arm_instructions':steps,'scope':'Original1730entrythrough1980preepilogue including originaledgeproducer/division and fullrasterbody vs nativefullpolygon, actualsource240prepared geometry/realROMtextures,miplevel0,fresh173fb/zeroedges/patchedbounds. Notwholeframe/cycles/cadenceproof.'};(root/'reports/source240-full-affine-polygon-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
