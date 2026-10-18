import pathlib,json
root=pathlib.Path('outputs/crazy-taxi-native');ns={};s=(root/'tools/compare_live_ordinary_world_rom.py').read_text();exec(s.split('import sys')[0],ns);exec('fields={'+s.split('fields={')[1].split('steps=armsteps=0')[0],ns)
a=bytes.fromhex((root/'reports/startup-chain-owner.txt').read_text().split()[10]);schema=json.loads((root/'reports/ordinary-full-world-state-schema.json').read_text())['fields'];lines=(root/'reports/startup-chain-world.txt').read_text().splitlines();d=dict(zip(schema,map(int,lines[0].split()[1:])));differences=[];checks=0
for o,k in {**ns['fields'],**ns['extra']}.items():
 n=ns['width'](o);key='input_'+k if o in ns['extra'] or o in [0,4,8] else k;v=int.from_bytes(a[o:o+n],'little');want=d[key]&((1<<(8*n))-1);checks+=1
 if v!=want:differences.append(dict(offset=hex(o),field=k,initializer=v,first_world=want))
report=dict(world_calls=len(lines),driver_calls=len((root/'reports/startup-chain-driver.txt').read_text().splitlines()),camera_calls=len((root/'reports/startup-chain-camera.txt').read_text().splitlines()),checks=checks,differences=differences,scope='Actual initialized packed owner to first native world input; exact offset audit only, source initializer separately qualified; differing fields require caller ownership evidence.')
(root/'reports/startup-initializer-world-boundary.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
