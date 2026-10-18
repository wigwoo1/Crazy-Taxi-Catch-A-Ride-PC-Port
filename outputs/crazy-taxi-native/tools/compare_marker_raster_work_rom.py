"""Patched original ARM49F8/4A94 versus native indexed markers; palette excluded."""
exec(open('outputs/crazy-taxi-native/tools/original_marker_raster_probe.py').read())
import subprocess,json
root=pathlib.Path('outputs/crazy-taxi-native')
rects=[(30,20,94,84,0),(30,20,94,21,0),(31,21,95,85,0),(0,0,240,160,0),(-17,20,47,84,1),(200,20,264,84,1),(30,-19,94,45,1),(30,125,94,189,1),(-17,-19,264,189,1)]
fixtures=[(k,*q) for k in range(10) for q in rects]
exe=root/'bin/marker-raster-work-probe.exe'
rom='Crazy Taxi - Catch a Ride (USA).gba'
p=subprocess.run([str(exe),rom],input=(''.join(' '.join(map(str,q))+'\n' for q in fixtures)).encode(),stdout=subprocess.PIPE,stderr=subprocess.PIPE)
assert p.returncode==0,(p.returncode,p.stderr)
assert len(p.stdout)==len(fixtures)*38408
class WorkProbe(MarkerRasterProbe):
 def __init__(self,*args):self.track=False;super().__init__(*args);self.reads=self.writes=0;self.track=True
 def read(self,a,n=4):
  if self.track and 0x08000000<=a<0x08800000 and n==1:self.reads+=1
  return super().read(a,n)
 def write(self,a,v,n=4):
  if self.track and FB<=a<FB+38400:self.writes+=1;assert n==2
  return super().write(a,v,n)
steps=0;changed=0;reads=writes=0
for j,(k,l,t,rr,b,c) in enumerate(fixtures):
 ptr=struct.unpack_from('<I',r,0x7ec59c+k*12+4)[0]
 probe=WorkProbe(ptr,(l,t,rr,b),c);original=probe.run();native=p.stdout[j*38408:j*38408+38400]
 if original!=native:
  difference=next((i,a,b) for i,(a,b) in enumerate(zip(original,native)) if a!=b)
  report={'rom_sha256':hashlib.sha256(r).hexdigest(),'equal':False,'first_failed_fixture':fixtures[j],'first_difference':difference,'passed_cases_before_failure':j,'original_arm_instructions':probe.steps,'scope':'Actual relocated49F8/4A94 including original49E4->4418->2CE8->29FC signed divider; no quotient-only helper shortcut. Isolated indexed framebuffer; palette and complete game excluded.'}
  (root/'reports/marker-raster-work-rom-comparison.json').write_text(json.dumps(report,indent=2))
  raise AssertionError((j,fixtures[j],difference))
 assert struct.unpack_from("<2I",p.stdout,j*38408+38400)==(probe.reads,probe.writes),(j,probe.reads,probe.writes)
 reads+=probe.reads;writes+=probe.writes
 steps+=probe.steps;changed+=sum(x!=173 for x in original)
report={'rom_sha256':hashlib.sha256(r).hexdigest(),'cases':len(fixtures),'compared_framebuffer_bytes':len(fixtures)*38400,'original_arm_instructions':steps,'changed_pixels':changed,'equal':True,'texture_byte_reads':reads,'framebuffer_halfword_writes':writes,'gpu_row_planner_equal':False,'actual_gpu_pixels_verified':False,'fixtures':fixtures,'source_entries':['080049F8','08004A94'],'relocation':'ROM700..4F5C copied to explicit diagnostic RAM;49E8 patched to1DB0 clip block,49EC to1DC4 framebuffer word; original instructions decoded at relocated PC.','helper_boundary':'Original49E4/4418/2CE8/29FC divider executed with volatile register outputs; no division helper shortcut.','scope':'All10 recovered64x64 markers, native240x160 indexed framebuffer versus original callbacks; zero key and duplicated pairs, odd bounds and each screen edge included. No registered palette/colors/full game/GPU widescreen pixels verified.'}
(root/'reports/marker-raster-work-rom-comparison.json').write_text(json.dumps(report,indent=2))
print('PASS',len(fixtures),'cases',len(fixtures)*38400,'indexed framebuffer bytes',steps,'original ARM instructions')
