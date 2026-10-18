from pathlib import Path
import json,struct,subprocess
root=Path('outputs/crazy-taxi-native');j=json.loads((root/'reports/source240-bounds-and-cells-observation.json').read_text());source=json.loads((root/'reports/source240-original-cell-loop-verification.json').read_text());data=(root/'assets/city1.ctw').read_bytes();nv,np,nm=struct.unpack_from('<3I',data,4);off=20+nv*16+np*24+nm*28;nx,nz=struct.unpack_from('<2I',data,off+4);sn=struct.unpack_from('<I',(root/'assets/city1-special.cts').read_bytes(),4)[0]
lines=[' '.join(map(str,[nx,nz,np,sn,*j['bounds'],len(source['cells'])]))];expected=[];normal=set();special=set()
for c in source['cells']:
 lines.append(' '.join(map(str,[c['x'],c['z'],len(c['references']),*c['references']])))
 for r in c['references']:
  if r&0x8000:
   if r&0x4000:continue
   key=r&0x3fff;cache=special
  else:key=r;cache=normal
  if key not in cache:expected.append(r);cache.add(key)
cp=subprocess.run([str(root/'bin/source240-cached-visitor-probe.exe')],input='\n'.join(lines)+'\n',capture_output=True,text=True,check=True);rows=[list(map(int,line.split())) for line in cp.stdout.splitlines()];assert rows==[[1,*expected],[1],[2,*expected]]
report={'status':'PASS','source_visited_cells':len(source['cells']),'source_raw_references':425,'first_generation_dispatched_references':len(expected),'same_generation_replay_dispatched':0,'next_generation_dispatched_references':len(expected),'normal_unique':len(normal),'special_unique':len(special),'dispatch_order':expected,'scope':'Productionnativeworld_render_visit_cached on exactoriginalcellpointerloop observedrefs: firstseen normal/special gate order and specialbit14 skip, repeatedgeneration suppression and nextgeneration reopen. Prior source stamp routines separatelyqualified; replay here native invariant, not continuous originalframe/cachetrajectory/cycleproof.'};(root/'reports/source240-native-cached-visitor-verification.json').write_text(json.dumps(report,indent=2)+'\n');print({k:v for k,v in report.items() if k!='dispatch_order'})
