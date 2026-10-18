"""Original shadow-frame raster vs native indexed billboard pixels and work."""
from pathlib import Path
import json,struct,subprocess,hashlib,sys
sys.path.insert(0,'work/python-packages')
root=Path('outputs/crazy-taxi-native')
exec((root/'tools/original_marker_raster_probe.py').read_text())
class ShadowRaster(MarkerRasterProbe):
 def __init__(self,pixels,rect):
  self.track=False;super().__init__(pixels,rect,False);self.write(DESC,0x05062040,4);self.reads=0;self.writes=0;self.track=True
 def read(self,a,n=4):
  if self.track and 0x08000000<=a<0x08800000 and n==1:self.reads+=1
  return super().read(a,n)
 def write(self,a,v,n=4):
  if self.track and FB<=a<FB+38400:self.writes+=1;assert n==2
  return super().write(a,v,n)
joined=json.loads((root/'reports/taxi-shadow-joined-renderer-verification.json').read_text());assert joined['status']=='PASS'
cases=joined['cases'];input_text=''.join(' '.join(map(str,[int(c['shadow_call']['pixels'],16)-0x08000000]+c['shadow_rectangle']+[0]))+'\n' for c in cases)
cp=subprocess.run([str(root/'bin/taxi-shadow-raster-probe.exe'),'Crazy Taxi - Catch a Ride (USA).gba'],input=input_text.encode(),capture_output=True,check=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
assert len(cp.stdout)==len(cases)*38408;results=[]
for i,c in enumerate(cases):
 p=ShadowRaster(int(c['shadow_call']['pixels'],16),tuple(c['shadow_rectangle']));pixels=p.run();native=cp.stdout[i*38408:(i+1)*38408]
 assert pixels==native[:38400] and struct.unpack('<2I',native[38400:])==(p.reads,p.writes),(i,p.reads,p.writes)
 (root/'reports'/('shadow-source240-'+str(c['ticks'])+'.bin')).write_bytes(pixels)
 results.append(dict(ticks=c['ticks'],rect=c['shadow_rectangle'],texture_byte_reads=p.reads,halfword_writes=p.writes,changed_pixels=sum(x!=173 for x in pixels),original_arm_instructions=p.steps,pixel_sha256=hashlib.sha256(pixels).hexdigest()))
report=dict(status='PASS',cases=results,exact_indexed_pixels=len(cases)*38400,scope='Original ARM unclipped64x32 shadow billboard pixel body atoriginal1556C-selected ROMframes vs productionnative keyed indexed raster. All38400pixels including retained173background andworkexact each of5source240rectangles. Originalpacket producer separatelyqualified; relocation literal fixtures, originalgroundquery/livepalette/sharedcompleteframe/shadowGPU/continuous4K/cadence unverified. Offlineonly.')
(root/'reports/taxi-shadow-raster-verification.json').write_text(json.dumps(report,indent=2));print(json.dumps(report),flush=True)
