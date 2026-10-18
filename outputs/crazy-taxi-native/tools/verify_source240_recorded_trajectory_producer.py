"""Offline continuous ROM scenery vs production native scene across recorded poses."""
from pathlib import Path
import sys,json,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages')
root=Path('outputs/crazy-taxi-native')
scopes=[]
for path,marker in [('probe_world_producer_multi.py','steps=0;cases=[]'),('verify_source240_continuous_scene_producer.py','steps=0;j=json')]:
 ns={};exec((root/'tools'/path).read_text().split(marker)[0],ns);ns['steps']=0;scopes.append(ns)
reference,continuous=scopes
trace=root/'reports/camera-heading-chain-120-camera.txt'
rows=[list(map(int,l.split())) for l in trace.read_text().splitlines()];assert len(rows)==120 and all(len(r)==31 for r in rows)
indices=[0,29,59,89,119];cases=[[0,i,*rows[i][25:28],rows[i][29],rows[i][28]] for i in indices]
exe=root/'bin/source240-trajectory-frame-probe.exe'
cp=subprocess.run([str(exe),*[str(root/'assets'/a) for a in ['city1.ctw','city2.ctw','driving.ctr','indexed-scenes/city1.cti','indexed-scenes/city2.cti','city1-special.cts','city2-special.cts']]],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
lines=cp.stdout.splitlines();assert len(lines)==len(cases)
results=[];mb,tb=struct.unpack_from('<II',continuous['rom'],0x7556cc+28)
for case,line in zip(cases,lines):
 v=list(map(int,line.split()));assert v[:2]==case[:2];count=v[2];assert len(v)==3+count*5
 packets=[v[3+i*5:8+i*5] for i in range(count)];refs=[p[1]|(0x8000 if p[0]==2 else 0) for p in packets]
 full=continuous['original']([0,[0]*425,*case[2:]]);accepted=reference['original']([0,refs,*case[2:]])
 assert full['cursor']==accepted['cursor']==count*144,(case,count,full['cursor'],accepted['cursor'])
 a=bytes.fromhex(full['packet'])[:full['cursor']];b=bytes.fromhex(accepted['packet'])[:accepted['cursor']]
 defined_a=bytearray(a);defined_b=bytearray(b);unused_triangle_bytes=0
 for i,p in enumerate(packets):
  if p[0]==0 and p[4]&16:
   # A triangle never consumes fourth vertex; rejected candidates can leave its XYZ behind.
   defined_a[i*144+112:i*144+144]=bytes(32);defined_b[i*144+112:i*144+144]=bytes(32);unused_triangle_bytes+=32
 assert defined_a==defined_b,('defined producer submission/order mismatch',case)
 assert full['heads']==accepted['heads'],('bucket links mismatch',case)
 for i,p in enumerate(packets):
  flags=struct.unpack_from('<H',a,144*i+4)[0];mp,tp=struct.unpack_from('<II',a,144*i+8)
  assert [flags,(mp-mb)//28,255 if not tp else (tp-tb)//12]==[p[4],p[2],p[3]],(case,i,p)
 results.append({'trace_index':case[1],'camera':case[2:5],'yaw':case[5],'pitch':case[6],'packets':count,'normal_packets':sum(p[0]==0 for p in packets),'special_packets':sum(p[0]==2 for p in packets),'raw_packet_bytes':len(a),'unused_triangle_bytes_excluded':unused_triangle_bytes,'raw_packet_sha256':hashlib.sha256(a).hexdigest()})
 print('PASS',results[-1],flush=True)
report={'status':'PASS','poses':results,'original_continuous_arm_instructions':continuous['steps'],'original_accepted_producer_arm_instructions':reference['steps'],'trace_sha256':hashlib.sha256(trace.read_bytes()).hexdigest(),'native_probe_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'scope':'Five actual recorded native camera poses. Native production world_frame real city grid, fresh cache0/source240/focal192/corners. Full original2D18 bounds/cellloop/31EC producer defined submission bytes and bucket links exactly match (unused fourth triangle vertex excluded) independently running original31EC on native accepted references in native submission order; all native flags/material/texture match. Not native vertex/pixel equality at these poses, retained generation/cache lifecycle, original bootstrap, full frame, cycles or cadence proof.'}
(root/'reports/source240-recorded-trajectory-producer-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
