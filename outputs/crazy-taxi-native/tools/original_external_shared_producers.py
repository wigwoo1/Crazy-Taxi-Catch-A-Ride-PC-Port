"""Execute qualified original consumers with caller-owned RNG; capture fixtures remain explicit."""
from pathlib import Path
import json
root=Path('outputs/crazy-taxi-native')

def load_consumer(name,start,end,remove):
 text=(root/'tools'/name).read_text();prefix=text.split('\nrows=')[0];body=text.split(start,1)[1].split(end,1)[0]
 for line in remove:body=body.replace(line,'')
 body=body.replace(' p=',' assert before==seed, (before,seed)\n p=',1)
 body=body.replace(' p.run(entry=', ' p.write(0x20025d6,shared_voice,1)\n p.run(entry=',1)
 ns={};exec(prefix,ns)
 exec('def consume(row,seed,shared_voice):\n index=0\n'+body+'\n return p.read(0x2003b04),p.calls,p.read(0x20025d6,1)\n',ns)
 return ns['consume']
idle=load_consumer('compare_passenger_idle_live_rom.py','for index,row in enumerate(rows):\n','report=dict(',[' stages[str(pre[10])]=stages.get(str(pre[10]),0)+1;steps+=p.steps\n'])
pickup=load_consumer('compare_passenger_pickup_start_live_rom.py','for row in rows:\n','assert (root/', [' if not row[0]:continue\n',' cases+=1;steps+=p.steps\n'])
trip=load_consumer('compare_trip_start_live_rom.py','for row in rows:\n','assert rows', [' steps+=p.steps\n'])
# Trip mapping is declared after the report rows; supply it explicitly from its source definition.
trip.__globals__['mapping']=[(0x2001688,4),(0x200753c,4),(0x20016ac,4),(0x20016b0,4),(0x20016e0,4),(0x2005310,2),(0x2005312,2),(0x2005314,2),(0x2001694,1),(0x2001695,1),(0x20025d6,1)]
voice=load_consumer('compare_impact_voice_live_rom.py','for index,row in enumerate(rows):\n','assert rows', [' steps+=p.steps;requests+=requested\n'])
voice.__globals__.update(active_addr=0x2001694,voice_addr=0x20025d6)
fixtures={}
for path,before_index,after_index,labels in [
 ('current-passenger-idle-states.txt',6,7,['passenger-idle-choice','passenger-idle-delay']),
 ('current-shared-voice-pickup.txt',10,11,['pickup-first']),
 ('current-shared-voice-fare.txt',6,7,['fare-pickup']),
 ('current-shared-voice-impact.txt',1,3,['world-impact-voice'])]:
 for row in [list(map(int,l.split())) for l in (root/'reports'/path).read_text().splitlines()]:
  for label in labels:
   if label=='pickup-first' and not row[0]:continue
   key=(label,row[before_index]);assert key not in fixtures;fixtures[key]=row

executed=[]
def advance_transition(transition,seed,shared_voice):
 calls=transition['calls'];i=0
 while i<len(calls):
  call=calls[i];label=call['label'];assert seed==call['before']
  consumer=idle if label.startswith('passenger-idle-') else pickup if label=='pickup-first' else trip if label=='fare-pickup' else voice if label=='world-impact-voice' else None
  assert consumer is not None,label
  row=fixtures[(label,seed)]
  prior_voice=row[20] if label=='pickup-first' else row[19] if label=='fare-pickup' else row[2] if label=='world-impact-voice' else shared_voice
  assert prior_voice==shared_voice,(label,prior_voice,shared_voice)
  seed,count,shared_voice=consumer(row,seed,shared_voice)
  assert count in [1,2] and i+count<=len(calls)
  assert seed==calls[i+count-1]['after']
  executed.append(dict(frame=transition['frame'],label=label,original_calls=count,computed_rng=seed,computed_voice=shared_voice))
  i+=count
 return seed,shared_voice
