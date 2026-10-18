import csv,json,struct,hashlib
from pathlib import Path
r=Path('outputs/crazy-taxi-native');ram=(r/'reports/original-core-reference/boot4200.ewram').read_bytes();initial=struct.unpack_from('<i',ram,0x2634+0x544)[0]
rows=[]
for mode in [0,1]:rows.append([{k:int(v) for k,v in x.items()} for x in csv.DictReader((r/f'reports/cadence-stage-mode{mode}.csv').open()) if x['group']=='0'])
coarse=rows[0][0];fine=rows[1];assert initial==0 and len(fine)==coarse['delta']==5
assert {v['acceleration6a0'] for v in [coarse]+fine}=={13016}
assert {v['rpm538'] for v in [coarse]+fine}=={500}
assert all(v['collided']==0 and v['scale7c8']==v['scalee84']==4096 for v in [coarse]+fine)
force=coarse['acceleration6a0'];coarse_increment=force//(60//coarse['delta']);unit_increment=force//60
assert coarse_increment==coarse['motion_before_speed']==1084
assert [v['motion_before_speed'] for v in fine]==[unit_increment*i for i in range(1,6)]
assert len({(v['world_x'],v['world_z']) for v in fine})==1
report=dict(status='FIRST_GROUP_INTEGER_INTEGRATION_EXPLAINED',initial_speed=initial,unchanged_acceleration_force=force,unchanged_rpm=500,original_five_tick_increment=coarse_increment,five_unit_tick_increment=5*unit_increment,speed_difference=coarse_increment-5*unit_increment,original_displacement=[coarse['dx'],coarse['dz']],unit_displacements=[[v['dx'],v['dz']] for v in fine],scope='Actual native stage records verify speed increment formula force / (60 / delta) from productionacceleration.c with unchanged force/RPM/scales and no collisions. Allfive unitmotion outputs zero; source5tick motion(3,4). Smallerstep integration also evaluates changing speed at eachstep, so this is not solely a discarded fractional-position issue. This explains firstlocal mismatch only, not fulljourney/failurecause or a proven fix.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [r/'src/acceleration.c',r/'src/motion.c',r/'reports/cadence-stage-mode0.csv',r/'reports/cadence-stage-mode1.csv']})
(r/'reports/first-cadence-integer-integration.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='hashes'},indent=2))
