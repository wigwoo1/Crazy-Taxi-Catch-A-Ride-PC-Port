"""Probe multiple explicit height assumptions; never silently choose one."""
import json
import subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
report={'status':'Diagnostic supplied-Y queries, not original initializer equivalence.', 'scenes':{}}
for scene in ['city1','city2']:
    source=json.loads((root/'reports/passengers'/f'{scene}-passengers.json').read_text())
    routes={r['index']:r for r in source['routes']}
    requests=[];lines=[]
    for p in source['pedestrians']:
        if not p['customer_candidate']:continue
        slots=source['destinations'][p['destination_index']]['accepted_material_slots']
        for route_index in p['matching_route_indices']:
            point=routes[route_index]['points_raw'][-1]
            for mode,y in [('first_hit',-2147483648),('zero',0),('pedestrian_y',p['position_xyz'][1])]:
                lines.append(' '.join(map(str,[point[1],y,point[2],*slots])))
                requests.append({'pedestrian':p['index'],'route':route_index,'height_assumption':mode})
    process=subprocess.run([str(root/'bin/passenger-endpoint-probe.exe'),str(root/'assets'/f'{scene}.ctw')],
                           input='\n'.join(lines)+'\n',text=True,capture_output=True,check=True)
    values=process.stdout.splitlines()
    assert len(values)==len(requests)
    for row,line in zip(requests,values):
        hit,material,height,accepted=map(int,line.split())
        row.update(hit=bool(hit),material=material,height=height,accepted=bool(accepted))
    summary={mode:sum(r['accepted'] for r in requests if r['height_assumption']==mode)
             for mode in ['first_hit','zero','pedestrian_y']}
    report['scenes'][scene]={'accepted_route_pairs_by_assumption':summary,'queries':requests}
    print(scene,summary)
(root/'reports/passengers/endpoint-probe.json').write_text(json.dumps(report,indent=2)+'\n')

