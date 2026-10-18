from pathlib import Path
import json,struct,subprocess,hashlib
exec(Path('outputs/crazy-taxi-native/tools/original_marker_raster_probe.py').read_text())
root=Path('outputs/crazy-taxi-native');inputs=json.loads((root/'reports/live-special-inputs.json').read_text())['special_inputs'];textures=json.loads((root/'assets/indexed-scenes/city1.json').read_text())['textures'];cases=[];skipped=[]
for a in inputs:
 if a['native_source_result']!=1:skipped.append(a);continue
 t=textures[a['texture']];assert (a['width'],a['height'],a['log_width'])==(64,64,6);ptr=int(t['pixels'],16);cases.append((a,ptr))
assert len(cases)==3
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
 assert (p.reads,p.writes)==(a['texture_byte_reads'],a['framebuffer_halfword_writes']);reads+=p.reads;writes+=p.writes;steps+=p.steps
report={'status':'PASS','actual_native_packets':len(cases),'source_wide_unqualified_packets':len(skipped),'framebuffer_bytes':len(cases)*38400,'texture_byte_reads':reads,'framebuffer_halfword_writes':writes,'original_arm_instructions':steps,'scope':'Actual native prepared source-inbounds scenery rectangles/realROMtexture116 vsoriginal49F8/4A94 fullcallback and shared native work API; synthetic173initialfb/patched boundary contracts. Other3nativewide rectangles not source240qualified; not original full scene/cycles/cadence proof.'};(root/'reports/live-special-source-work-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
