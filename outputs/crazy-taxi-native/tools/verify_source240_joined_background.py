"""Joined original renderer prefix/background call/Thumb return with shared framebuffer."""
from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages');root=Path('outputs/crazy-taxi-native')
prefix_source=(root/'tools/verify_background_renderer_prefix.py').read_text().split('cases=')[0]
prefix_source=prefix_source.replace('exec(source)',"source=source.replace('   assert 0x08032000<=pc', '   if pc==0x08002fe0:\\n    pc=self.background();continue\\n   assert 0x08032000<=pc');exec(source)")
exec(prefix_source)
arm_scope={};arm_source=(root/'tools/compare_world_background_arm.py').read_text().split('cases=')[0]
a=arm_source.index('def original(case):');start=arm_source.index(' N=Z=V=False;C=False;pc=0x08002fe0',a);end=arm_source.index(' return regions[0x10080000]',start)
body=arm_source[start:end].replace('while pc!=0xffffffff:', 'while pc!=stop:').replace("if mn=='bx':nxt=value(ops[0])", "if mn=='bx':nxt=value(ops[0])&~1")
fn='''def background(self):
 global steps
 regs={f'r{i}':self.reg[i] for i in range(9)}
 regs.update({name:self.reg[index] for name,index in [('sb',9),('sl',10),('fp',11),('ip',12),('sp',13),('lr',14),('pc',15)]})
 stop=regs['lr']&~1;before=steps
 read=self.read;write=self.write
'''+body+'''
 for name,index in [(f'r{i}',i) for i in range(9)]+[('sb',9),('sl',10),('fp',11),('ip',12),('sp',13),('lr',14),('pc',15)]:self.reg[index]=regs[name]
 self.n,self.z,self.c,self.v=N,Z,C,V;self.background_steps+=steps-before
 return pc
'''
exec(arm_source[:a]+fn,arm_scope);arm_scope['steps']=0
class JoinedBackgroundProbe(PrefixProbe):
 def __init__(self,pitch,yaw,scene):
  super().__init__(pitch,0x10080000);self.background_steps=0
  self.write(0x03006810,scene);self.write(0x10001010,yaw&MASK)
  for off,v in [(8,0),(12,0),(16,240),(20,160)]:self.write(0x03006810+off,v)
  descriptor=struct.unpack_from('<H',ROM,scene-0x08000000+20)[0];table=struct.unpack_from('<I',ROM,scene-0x08000000+32)[0];pixels=struct.unpack_from('<I',ROM,table-0x08000000+descriptor*12+4)[0];height=ROM[table-0x08000000+descriptor*12+1]
  pattern=ROM[pixels-0x08000000+(height-1)*240]*0x01010101;self.write(0x03006850,pattern)
  for a in range(0x10080000,0x10089600):self.shared[a]=165
 def read(self,a,size=4):
  if a==0x08001dc4:return super().read(0x02009100,size)
  if 0x10080000<=a<a+size<=0x10089600:return sum(self.shared.get(a+i,0)<<(8*i) for i in range(size))
  return super().read(a,size)
 def write(self,a,v,size=4):
  if 0x10080000<=a<a+size<=0x10089600:
   for i in range(size):self.shared[a+i]=(v>>(i*8))&255
  else:super().write(a,v,size)
 def background(self):
  self.background_input_pitch=self.signed(self.reg[0]);return arm_scope['background'](self)
rows=[list(map(int,l.split())) for l in (root/'reports/camera-heading-chain-120-camera.txt').read_text().splitlines()];results=[]
for city,scene in [('city1',0x087556cc),('city2',0x08755734)]:
 for index in [0,29,59,89,119]:
  pitch,yaw=rows[index][28:30];p=JoinedBackgroundProbe(pitch,yaw,scene);p.run(0x0802b6ac,stop=0x0802b6f8)
  assert p.reg[14]==0x0802b6f9 and p.read(0x02009100)==0x10080000
  cp=subprocess.run([str(root/'bin/background-pitch-probe.exe')],input=str(pitch)+'\n',capture_output=True,text=True,check=True);native_pitch=int(cp.stdout);assert p.background_input_pitch==native_pitch
  cp=subprocess.run([str(root/'bin/source240-real-background-probe.exe'),str(root/'assets/indexed-scenes'/f'{city}.cti'),'0',str(pitch),str(yaw)],capture_output=True,check=True)
  fb=bytes(p.shared[a] for a in range(0x10080000,0x10089600));assert fb==cp.stdout,(city,index,'background mismatch')
  results.append({'city':city,'trace_index':index,'camera_pitch':pitch,'camera_yaw':yaw,'framebuffer_sha256':hashlib.sha256(fb).hexdigest(),'thumb_instructions':p.steps,'angle_arm_instructions':p.angle_steps,'background_arm_instructions':p.background_steps})
  print('PASS',city,index,flush=True)
report={'status':'PASS','cases':results,'compared_framebuffer_bytes':len(results)*38400,'scope':'Actualoriginal2B6AC framebufferhandoff throughnestednumeric/anglehelpers, actual32450 BXr1 tocompleteoriginalARM2FE0 background andThumbreturn2B6F8. Samecamera/engine/frameblock/stack/framebuffer memory bridgedacrossISA; immutable realcitydescriptors/ROMpixels. Fixture relocation08001DC4 mapsselectedrelocatedframeword02009100. Every38400byte framebuffer matches nativeactualCTIbackground at5recordedposes eachcity. No queue/actors/palette/fullframe/cycles/cadence proof.'}
(root/'reports/source240-joined-background-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
