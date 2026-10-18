from pathlib import Path
import sys,json,struct
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0])
producer=original
src=Path('outputs/crazy-taxi-native/tools/original_marker_raster_probe.py').read_text()
src=src.replace('pc!=0xffffffff','pc!=0x080046d8').replace('assert 0x49f8<=off<0x4bac or off in (0x49e4,0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)','assert 0x4424<=off<0x4704 or off in (0x4418,0x2ce8) or 0x29fc<=off<0x2ce8,hex(pc)')
src=src.replace("   elif m in ('ldr','ldrb','str','strh'):","""   elif m in ('ldm','stm'):
    base=i.op_str.split(', ',1)[0].rstrip('!');idx=self.ri(base);a=self.reg[idx]
    names=i.op_str[i.op_str.index('{')+1:i.op_str.index('}')].split(', ')
    for j,name in enumerate(names):
     if m=='ldm':self.reg[self.ri(name)]=self.read(a+j*4)
     else:self.write(a+j*4,self.val(name))
    if i.writeback:self.reg[idx]=a+4*len(names)
   elif m in ('ldr','ldrb','str','strh'):""")
src=src.replace("'ands'):","'ands','mla'):").replace("    elif m=='mul':x=a*b","    elif m=='mul':x=a*b\n    elif m=='mla':x=a*b+self.val(p[3])")
src=src.replace("b=self.operand(p[2:]) if len(p)>2 else 0", "b=self.val(p[2]) if m=='mla' else self.operand(p[2:]) if len(p)>2 else 0")
src=src.replace("14:True,0:self.z", "14:True,10:self.n==self.v,0:self.z")
exec(src)
arm.detail=True
BASE=0x08000700
class NearClip(MarkerRasterProbe):
 def __init__(self,packet):
  self.blocks=[(0x03006810,bytearray(248)),(0x12000000,bytearray(packet[16:])),(0x13000000,bytearray(4096)),(0x03006920,bytearray(32)),(0x14000000,bytearray(16))]
  self.reg=[0]*16;self.reg[0]=3 if struct.unpack_from('<H',packet,4)[0]&16 else 4;self.reg[1]=0x12000000;self.reg[13]=0x13001000;self.reg[14]=0xffffffff
  self.steps=0;self.z=self.n=self.v=self.c=False;self.entry=0x08004424
  self.write(0x03006920,0x03006810)
  for off,v in [(24,120),(28,80),(40,192),(44,256)]:self.write(0x03006810+off,v)
  for i,v in enumerate([0,0,240,160]):self.write(0x0300692c+i*4,v)
 def read(self,a,n=4):
  if 0x08000000<=a<a+n<=0x08800000:return int.from_bytes(r[a-0x08000000:a-0x08000000+n],'little')
  return super().read(a,n)
steps=0;j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());results=[]
for p in j['packets']:
 if not p['raster_flags']&3:continue
 o=producer([0,[p['source_index']],*j['camera'],j['yaw'],j['pitch']]);q=NearClip(bytes.fromhex(o['packet']));q.run();n=q.reg[8];ptr=q.reg[13]+0x94;vertices=[]
 for i in range(n):
  sx,u,v,sy,x,y,z,attr=[signed(q.read(ptr+i*32+k*4)) for k in range(8)]
  vertices.append([sx,sy,x,y,z,u,v,attr])
 fields=5 if p['texture']==255 else 7
 assert n==len(p['vertices'])
 if p['texture']==255:
  assert [v[7] for v in vertices]==[v[7] for v in p['vertices']]
  assert all(v[5:7]==[-1515870811,-1515870811] for v in vertices)
 else:assert all(v[7]==-1515870811 for v in vertices)

 assert [v[:fields] for v in vertices]==[v[:fields] for v in p['vertices']],(p['source_index'],vertices,p['vertices'])
 results.append({'source_index':p['source_index'],'vertices':vertices,'original_arm_instructions':q.steps})
report={'status':'PASS','packets':results,'scope':'Original nearconsumer4424 through46d8 including realoriginalsigneddivision and screenprojection against actualsource240prepared nativepolygons. Flatattribute compared; flatunusedUV and texturedunusedattribute asserted unchanged sourceA5sentinel then excluded; stopsbefore materialcallback, no originalraster/fullframe/cyclesproof.'}
(root/'reports/source240-near-consumer-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
