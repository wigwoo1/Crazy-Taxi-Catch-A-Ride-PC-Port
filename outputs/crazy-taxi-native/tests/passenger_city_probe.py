"""Run native passenger orchestration on exported city source data."""
import json
import subprocess
from pathlib import Path
root=Path(__file__).resolve().parents[1]
report={'status':'Native city-data diagnostic, not playable integration or GBA execution equivalence.',
        'assumptions':['Endpoint query Y explicitly 0; original stack Y unresolved.',
                       'Spawn Y supplied from source record, not original height helper.',
                       'Taxi fixed 700 units from each trial spawn; RNG and external gates supplied as zero.',
                       'Every trial resets customers; no trip timer, fare or delivery.'],'scenes':{}}
for scene,expected in [('city1',68),('city2',76)]:
    source=json.loads((root/'reports/passengers'/f'{scene}-passengers.json').read_text())
    passengers=source['pedestrians'];routes=source['routes'];destinations=source['destinations']
    lines=[f'{len(passengers)} {len(routes)} {len(destinations)} 0']
    for d in destinations:lines.append(' '.join(map(str,[*d['target_xz'],*d['accepted_material_slots']])))
    for p in passengers:lines.append(' '.join(map(str,[p['kind'],p['destination_index'],*p['position_xyz']])))
    for r in routes:
        points=r['points_raw'];lines.append(str(len(points)))
        lines.extend(f'{p[1]} {p[2]}' for p in points)
    result=subprocess.run([str(root/'bin/passenger-city-probe.exe'),str(root/'assets'/f'{scene}.ctw')],
                          input='\n'.join(lines)+'\n',text=True,capture_output=True,check=True)
    rows=result.stdout.splitlines();summary=list(map(int,rows[-1].split()[1:]));assert summary==[expected,expected,expected],summary
    report['scenes'][scene]={'eligible':summary[0],'trials':summary[1],'completed':summary[2],
                           'trips':[dict(zip(['near_spawn','selected_customer','updates','destination'],map(int,r.split()[1:]))) for r in rows[:-1]]}
    print(scene,summary)
(root/'reports/passengers/city-integration-check.json').write_text(json.dumps(report,indent=2)+'\n')
