from pathlib import Path
import json,struct,subprocess,csv,hashlib
r=Path('outputs/crazy-taxi-native');binary=r/'bin/connected-cadence-stage-probe.exe';records=json.loads((r/'reports/original-core-reference/longsourceleft.packed-driving-trace.json').read_text())['records'];pairs=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']];lines=[]
for a,b in pairs:lines.append(f"{struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0]} {a['frame_delta']} {struct.unpack_from('<i',bytes.fromhex(b['time_commands']))[0]} {b['frame_delta']}")
results=[]
for mode in range(3):
 path=r/f'reports/cadence-stage-mode{mode}.csv';payload='\n'.join(lines)+'\n';off=subprocess.run([str(binary),str(mode)],input=payload,capture_output=True,text=True,check=True);on=subprocess.run([str(binary),str(mode),str(path)],input=payload,capture_output=True,text=True,check=True);assert on.stdout==off.stdout,'Stage observer changes outputs';assert not on.stderr and not off.stderr
 rows=[{k:int(v) for k,v in x.items()} for x in csv.DictReader(path.open())];first=[v for v in rows if v['group']==0];assert len(first)==(pairs[0][1]['frame_delta'] if mode==1 else 1)
 results.append(dict(mode=mode,capture_records=len(rows),observer_on_off_equal=True,first_group=first))
assert [(v['dx'],v['dz']) for v in results[0]['first_group']]==[(3,4)]
assert all(v['dx']==v['dz']==0 and not v['collided'] for v in results[1]['first_group'])
assert sum(v['delta'] for v in results[0]['first_group'])==sum(v['delta'] for v in results[1]['first_group'])==5
report=dict(status='PASS_FIRST_CADENCE_MISMATCH_STAGE_CAPTURE',results=results,scope='Native production motion observer captures actual before/after motion values; after connected world captures acceleration6A0/RPM/position/speed/collision. All244left-journey endpoint output lines identical observerenabled/disabled for all3modes. Source5tick firstgroup creates(3,4) displacement; five1tickfirstgroup motion outputs all(0,0), no collisions. Demonstrates integer motion generation/substep integration loses distance in this fixture. Acceleration/RPM also differs; this doesnotisolate all later speed differences or prove any proposed fractionalcarry/retiming coefficient faithful.',hashes={str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in [Path(__file__),r/'tests/connected_cadence_stage_probe.c',binary,r/'src/motion.c',r/'src/acceleration.c']})
(r/'reports/connected-cadence-stage-comparison.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='hashes'},indent=2))
