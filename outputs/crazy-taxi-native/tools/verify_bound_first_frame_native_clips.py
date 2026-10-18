from pathlib import Path
import json,struct,subprocess
root=Path('outputs/crazy-taxi-native')
cases=json.loads((root/'reports/bound-first-frame-native-clip-inputs.json').read_text())
draws=json.loads((root/'reports/bound-first-frame-native-draw-inputs.json').read_text())
assert len(cases)==12
blob=bytearray(struct.pack('<I',len(cases)))
for c in cases:
 blob+=struct.pack('<10I',len(c['input_vertices']),*c['projection'],*c['bounds'])
 for v in c['input_vertices']:blob+=struct.pack('<8I',*v)
fixture=root/'reports/bound-first-frame-native-clips.fixture';fixture.write_bytes(blob)
cp=subprocess.run([str(root/'bin/bound-first-frame-clip-native-compare.exe'),str(fixture)],capture_output=True)
rows=[];offset=0
if cp.returncode==0:
 for c in cases:
  n,accepted=struct.unpack_from('<2I',cp.stdout,offset);offset+=8
  actual=[list(struct.unpack_from('<8I',cp.stdout,offset+i*32)) for i in range(n)];offset+=n*32
  expected=c['projected_vertices'] or []
  original_accepted=any(d['packet_index']==c['packet_index'] for d in draws)
  rows.append(dict(packet_index=c['packet_index'],native_vertices=n,original_vertices=len(expected),native_accepted=accepted,original_accepted=original_accepted,vertices_match=actual==expected,acceptance_match=bool(accepted)==original_accepted,actual=actual,expected=expected))
 assert offset==len(cp.stdout)
passed=cp.returncode==0 and len(rows)==12 and all(r['vertices_match'] and r['acceptance_match'] for r in rows)
report=dict(status='NATIVE_ORIGINAL_FARCLIP_PASS' if passed else 'NATIVE_FARCLIP_DEPENDENCY',native_exit=cp.returncode,cases=rows,scope='12 actual original farclip caller inputs/projection/bounds and exact projected32bytevertex outputs/culling vs direct native C; observed retained first frame only, not whole gameplay or general clipping proof.')
(root/'reports/bound-first-frame-native-clips-comparison.json').write_text(json.dumps(report,indent=2)+'\n')
print(report['status'],cp.returncode,[(r['packet_index'],r['vertices_match'],r['acceptance_match']) for r in rows])
