"""Bound original227AC selector ownership diagnostic."""
from pathlib import Path
import json
rroot=Path('outputs/crazy-taxi-native');source=(rroot/'tools/probe_bound_actor_init_dependencies.py').read_text()
prefix,body=source.split("p=ns['Probe']",1);ctx={};exec(prefix,ctx)
body="p=ns['Probe']"+body;body=body.split("report={'status'")[0]
body=body.replace('before=bytes(', 'p.write(0x20025d6,255,1);p.write(0x20025d7,255,1)\nbefore=bytes(',1)
exec(body,ctx);p=ctx['p'];values=[p.read(0x20025d6,1),p.read(0x20025d7,1)]
assert values==[255,255] and ctx['dependency'] is None
report=dict(status='PASS_ORIGINAL_ACTOR_RETAINS_SELECTIONS',values=values,scope='Bound original227AC bounded diagnostic retains both FF audio selection bytes; ground/division/display dependencies retain previous explicit contracts. Does not explain native zeroed shared owner lifecycle.')
(rroot/'reports/bound-original-actor-audio-selection-owner.json').write_text(json.dumps(report,indent=2));print(report)
