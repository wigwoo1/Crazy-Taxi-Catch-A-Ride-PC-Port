"""Retained original scenery caches vs native world_frame at recorded camera poses."""
from pathlib import Path
import json,sys,struct,subprocess,hashlib
sys.path.insert(0,'work/python-packages');root=Path('outputs/crazy-taxi-native')
ns={};exec((root/'tools/verify_source240_continuous_scene_producer.py').read_text().split('steps=0;j=json')[0],ns)
s=ns['s'].replace('def original(case):','def original(case,state=None,generation=1):')
anchor=' write(0x11000018,len(refs))'
s=s.replace(anchor,""" write(0x03006810+52,generation)
 if state is not None:
  regions[0x0201b6ec][:]=bytes.fromhex(state['normal_cache'])
  regions[0x020276ec][:]=bytes.fromhex(state['special_cache'])
"""+anchor)
s=s.replace("return {'cursor':", "return {'generation':read(0x03006810+52),'normal_cache':bytes(regions[0x0201b6ec]).hex(),'special_cache':bytes(regions[0x020276ec]).hex(),'cursor':")
exec(s,ns);ns['steps']=0
ref={};exec((root/'tools/probe_world_producer_multi.py').read_text().split('steps=0;cases=[]')[0],ref);ref['steps']=0
trace=root/'reports/camera-heading-chain-120-camera.txt';rows=[list(map(int,l.split())) for l in trace.read_text().splitlines()]
schedule=[(i,g) for g,i in enumerate([0,0,29,29,59,59,89,119])]
cases=[[0,i,*rows[i][25:28],rows[i][29],rows[i][28],g] for i,g in schedule]
exe=root/'bin/source240-retained-scene-probe.exe'
cp=subprocess.run([str(exe),*[str(root/'assets'/a) for a in ['city1.ctw','city2.ctw','driving.ctr','indexed-scenes/city1.cti','indexed-scenes/city2.cti','city1-special.cts','city2-special.cts']]],input=''.join(' '.join(map(str,c))+'\n' for c in cases),text=True,capture_output=True,check=True)
captures=[json.loads(l) for l in cp.stdout.splitlines()];assert len(captures)==len(cases)
np=struct.unpack_from('<I',(root/'assets/city1.ctw').read_bytes(),8)[0];sn=struct.unpack_from('<I',(root/'assets/city1-special.cts').read_bytes(),4)[0]
state=None;results=[];mb,tb=struct.unpack_from('<II',ns['rom'],0x7556cc+28)
for case,capture in zip(cases,captures):
 if state is not None:assert state['generation']==case[7], 'generation history discontinuity'
 state=ns['original']([0,[0]*425,*case[2:7]],state,case[7]);assert state['generation']==case[7]+1;packets=capture['packets'];count=len(packets)
 assert state['cursor']==count*144,(case,'submission count',state['cursor'],count)
 for key,total in [('normal',np),('special',sn)]:
  data=bytes.fromhex(state[key+'_cache']);values=struct.unpack('<'+str(len(data)//4)+'I',data);assert not any(values[total:]),(case,key,'out-of-asset source stamps')
  sparse=[[i,v] for i,v in enumerate(values[:total]) if v];assert sparse==capture[key+'_stamps'],(case,key,'cache contents mismatch')
 if count:
  refs=[p['source_index']|(0x8000 if p['kind']==2 else 0) for p in packets]
  accepted=ref['original']([0,refs,*case[2:7]]);assert accepted['cursor']==state['cursor']
  a=bytearray(bytes.fromhex(state['packet'])[:count*144]);b=bytearray(bytes.fromhex(accepted['packet'])[:count*144])
  for i,p in enumerate(packets):
   flags=struct.unpack_from('<H',a,i*144+4)[0];mp,tp=struct.unpack_from('<II',a,i*144+8)
   assert [flags,(mp-mb)//28,255 if not tp else (tp-tb)//12]==[p['flags'],p['material'],p['texture']],(case,i,p)
   if p['kind']==0 and flags&16:a[i*144+112:i*144+144]=bytes(32);b[i*144+112:i*144+144]=bytes(32)
  assert a==b,(case,'defined packet bytes/order mismatch')
  assert state['heads']==accepted['heads'],(case,'queue heads mismatch')
 results.append({'trace_index':case[1],'generation_before':case[7],'generation_after':state['generation'],'submitted_packets':count,'normal_stamped_records':len(capture['normal_stamps']),'special_stamped_records':len(capture['special_stamps'])});print('PASS',results[-1],flush=True)
report={'status':'PASS','calls':results,'normal_cache_records_compared_per_call':np,'special_cache_records_compared_per_call':sn,'continuous_original_instructions':ns['steps'],'accepted_original_instructions':ref['steps'],'native_probe_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'trace_sha256':hashlib.sha256(trace.read_bytes()).hexdigest(),'scope':'Retained original2D18/31EC normal/special cache arrays vs native productionworld_frame at recorded poses, original entry generation increments0to8 with samepose repeats and camera movement. Every asset cache record exact after each call; original extra slots zero. Submitted metadata and defined original packet bytes/order/head links match original31EC on native acceptedrefs; unused fourth triangle vertex excluded. Phase0, generation owner increment agrees with original2D18 and existing live helper; not original bootstrap, phase lifecycle, preview snapshot, fullframe/pixels/cycles/cadence proof.'}
(root/'reports/source240-retained-scene-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
