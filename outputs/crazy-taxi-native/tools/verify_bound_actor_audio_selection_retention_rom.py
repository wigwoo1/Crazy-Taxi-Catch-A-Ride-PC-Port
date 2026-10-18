"""Bound original227AC selector ownership diagnostic."""
from pathlib import Path
import json
rroot=Path('outputs/crazy-taxi-native');source=(rroot/'tools/probe_bound_actor_init_dependencies.py').read_text()
prefix,body=source.split("p=ns['Probe']",1);ctx={};exec(prefix,ctx)
body="p=ns['Probe']"+body;body=body.split("report={'status'")[0]
body=body.replace('before=bytes(', 'p.write(0x20025d6,first,1);p.write(0x20025d7,second,1)\nbefore=bytes(',1)
cases=0
for first in [0,1,127,255]:
 for second in [0,1,127,255]:
  ctx['first']=first;ctx['second']=second;exec(body,ctx);p=ctx['p'];values=[p.read(0x20025d6,1),p.read(0x20025d7,1)]
  assert values==[first,second] and ctx['dependency'] is None
  cases+=1
report=dict(status='PASS_ORIGINAL_ACTOR_RETAINS_SELECTIONS',cases=cases,scope='Bound original227AC bounded diagnostic retains both audio selection bytes over sixteen independently varied prior values; ground/division/display dependencies retain previous explicit contracts. Bounds original actor ownership only; whole bootstrap excluded.')
(rroot/'reports/bound-original-actor-audio-selection-owner.json').write_text(json.dumps(report,indent=2));print(report)
