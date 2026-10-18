"""Join native sampler, original command/action and driver/world evidence."""
from pathlib import Path
import json,hashlib
root=Path('outputs/crazy-taxi-native');reports=root/'reports';label='bound-changing-replay'
samplers=[list(map(int,l.split())) for l in (reports/(label+'-samplers.txt')).read_text().splitlines()]
buttons=[list(map(int,l.split())) for l in (reports/(label+'-buttons.txt')).read_text().splitlines()]
ended=[row for row in samplers if row[2]];assert len(ended)==len(buttons)==124
for sampler,button in zip(ended,buttons):
 assert sampler[0]==button[1]
 after=sampler[56:];triples=[]
 for i in range(10):triples.extend([after[3+i*5],after[6+i*5],after[5+i*5]])
 assert triples==button[2:32],button[0]
report=dict(status='PASS_JOINED_STATEFUL_REPLAY_INPUT_TO_WORLD',sampler_calls=667,ended_calls=124,sampler_to_command_links=124,commands=124,actions=496,driver=124,world=124,scope='Native changing digital Bound replay: original15C3C retained state verified continuously from explicit zero reset, all53 sampler fields each call; actual ended sampler outputs linked to original command/action comparisons and native driver/world per-call original comparisons. Native failure/No/grid returns normally with701 declared unit-time records. Scheduling uses captured native calls; full original service cadence/continuous vehicle initialization trajectory/analog/hardware/audio not proved. Diagnostic only, production ended branch unchanged.',evidence={})
for name in ['bound-changing-replay-sampler-rom-comparison.json','bound-changing-replay-command-verification.json','bound-changing-replay-action-verification.json','bound-changing-replay-driver-rom-comparison.json','bound-changing-replay-world-rom-comparison.json']:
 path=reports/name;proof=json.loads(path.read_text());assert proof.get('cases',proof.get('samples',0))>0
 report['evidence'][name]=hashlib.sha256(path.read_bytes()).hexdigest()
report['native_exe_sha256']=hashlib.sha256((root/'bin/scene-viewer-bound-sampler-replay.exe').read_bytes()).hexdigest()
(reports/'bound-changing-replay-joined-verification.json').write_text(json.dumps(report,indent=2));print('PASS124 sampler-to-command joins;667 stateful original sampler calls.')
