from pathlib import Path
import json,struct,subprocess,hashlib
import sys
sys.path.insert(0,'work/python-packages')
exec(Path('outputs/crazy-taxi-native/tools/original_marker_raster_probe.py').read_text())
root=Path('outputs/crazy-taxi-native');inputs=json.loads((root/'reports/source240-preparation.json.source240.json').read_text())['packets'];textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures'];cases=[];skipped=[]
for a in inputs:
 if a['kind']!=2:continue
 t=textures[a['texture']];assert (t['width'],t['height'])==(64,64);ptr=int(t['pixels'],16);cases.append((a,ptr))
assert len(cases)==4
class Probe(MarkerRasterProbe):
 def __init__(self,*args):self.track=False;super().__init__(*args);self.reads=self.writes=0;self.track=True
 def read(self,a,n=4):
  if self.track and 0x08000000<=a<0x08800000 and n==1:self.reads+=1
  return super().read(a,n)
 def write(self,a,v,n=4):
  if self.track and FB<=a<FB+38400:self.writes+=1;assert n==2
  return super().write(a,v,n)
exe=root/'bin/special-raster-work-probe.exe';rom='Crazy Taxi - Catch a Ride (USA).gba'
cp=subprocess.run([str(exe),rom],input=''.join(' '.join(map(str,[ptr-0x08000000]+a['rect']+[int(a['callback']=='030069a4')]))+'\n' for a,ptr in cases).encode(),capture_output=True,check=True);assert len(cp.stdout)==len(cases)*38408
reads=writes=steps=0
for i,(a,ptr) in enumerate(cases):
 p=Probe(ptr,tuple(a['rect']),a['callback']=='030069a4');fb=p.run();assert cp.stdout[i*38408:(i+1)*38408]==fb+struct.pack('<2I',p.reads,p.writes)
 reads+=p.reads;writes+=p.writes;steps+=p.steps
report={'status':'PASS','actual_native_packets':len(cases),'source_wide_unqualified_packets':len(skipped),'framebuffer_bytes':len(cases)*38400,'texture_byte_reads':reads,'framebuffer_halfword_writes':writes,'original_arm_instructions':steps,'scope':'Four isolated native source240 scenery sprites including partialrightedge238..248 vs original49F8/4A94 fullcallbacks and sharednativework API. RealROMtexture116, synthetic173initialfb/patchedboundarycontracts; not fullscene/cycles/cadenceproof.'};(root/'reports/source240-special-raster-work-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
