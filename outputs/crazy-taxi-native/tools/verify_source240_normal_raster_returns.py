"""Original normal polygon bodies through actual stack/register epilogues."""
from pathlib import Path
import sys,json,struct,re,hashlib
sys.path.insert(0,'work/python-packages');root=Path('outputs/crazy-taxi-native')
j=json.loads((root/'reports/source240-geometry-work.json.source240.json').read_text());textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures']
data=(root/'assets/city1.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',data,4);mb=20+nv*16+np*24
callbacks=['03006950','03006948','03006914','03006934','030069ac'];names=['flat','affine','clipped','perspective','column'];ends=[0x1f94,0x198c,0x1730,0x1044,0x1440]
results=[];total=0
for callback,name,end in zip(callbacks,names,ends):
 scope={};exec((root/'tools'/('verify_source240_full_'+name+'_polygon.py')).read_text().split('\nj=json')[0],scope)
 cls=scope['Probe'];oldrun=cls.run;s=scope['s']
 s,n=re.subn(r'while pc!=BASE\+0x[0-9a-f]+-0x700:', 'while pc!=0xffffffff:',s);assert n==1
 oldend={'flat':0x1f88,'affine':0x1980,'clipped':0x1724,'perspective':0x1038,'column':0x1434}[name]
 assert '<'+hex(oldend) in s;s=s.replace('<'+hex(oldend),'<'+hex(end))
 exec(s,scope);scope['arm'].detail=True;newrun=scope['MarkerRasterProbe'].run
 for packet in j['packets']:
  if packet['callback']!=callback:continue
  arg=struct.unpack_from('<H',data,mb+packet['material']*28+14)[0] if name=='flat' else textures[packet['texture']]
  before=cls(packet,arg);expected=oldrun(before)
  after=cls(packet,arg);saved=[0xa4100000+k*0x10203 for k in range(9)];after.reg[4:13]=saved;initial_sp=after.reg[13];initial_lr=after.reg[14]
  actual=newrun(after)
  assert actual==expected and after.reg[4:13]==saved and after.reg[13]==initial_sp and after.reg[14]==initial_lr,(name,packet['source_index'])
  assert after.steps==before.steps+3
  if name=='flat':assert (after.half,after.word)==(before.half,before.word)
  else:assert (after.reads,after.writes)==(before.reads,before.writes)
  total+=after.steps;results.append({'source_index':packet['source_index'],'family':name,'original_arm_instructions':after.steps,'stack_restored':True,'r4_through_r12_restored':True,'lr_restored':True,'framebuffer_sha256':hashlib.sha256(actual).hexdigest()})
assert len(results)==33
report={'status':'PASS','normal_packets':33,'framebuffer_bytes_compared':33*38400,'original_arm_instructions':total,'packets':results,'scope':'All33 actualsource240 normal polygons execute actual original body plus addSP/popR4-R12LR/BXLR epilogue, preserving seeded caller registers/stack/LR. Pixels and read/store work unchanged vs previously native-qualified body. Relocated fixture bounds/frame/material/texture/stack contexts remain; near wrapper callchain and special billboard returns not included. No full frame/game/cycles/cadence proof.'}
(root/'reports/source240-normal-raster-returns-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',len(results),'original normal raster returns',total,'ARM instructions',flush=True)
