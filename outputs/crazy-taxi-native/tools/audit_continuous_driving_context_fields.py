import pathlib,json,struct,subprocess,collections
r=pathlib.Path('outputs/crazy-taxi-native');fields=json.loads((r/'reports/continuous-driving-context-field-map.json').read_text());results=[]
for side in ('left','right'):
 records=json.loads((r/f'reports/original-core-reference/fullshared{side}.packed-driving-trace.json').read_text())['records'];steps=[(a,b) for a,b in zip(records,records[1:]) if a['previous']!=b['previous']]
 payload=''.join(f"{struct.unpack_from('<H',bytes.fromhex(b['input_history']),2)[0]} {a['frame_delta']} {struct.unpack_from('<i',bytes.fromhex(b['time_commands']))[0]} {b['frame_delta']}\n" for a,b in steps)
 cp=subprocess.run([str(r/'bin/original-continuous-context-probe.exe')],input=payload,text=True,capture_output=True,check=True);rows=[list(map(int,x.split())) for x in cp.stdout.splitlines()];assert len(rows)==len(steps)
 failures=collections.defaultdict(list)
 for (_,record),row in zip(steps,rows):
  assert row[0]==0 and len(row)==len(fields)+1;actor=bytes.fromhex(record['actor'])
  for field,value in zip(fields,row[1:]):
   offset=field['offset'];data=actor
   if field.get('region')=='shared':
    data=bytearray(0x8000)
    for address,raw in record['shared_owners'].items():
     start=int(address,16)-0x2000000;block=bytes.fromhex(raw);data[start:start+len(block)]=block
   expected=int.from_bytes(data[offset:offset+field['bytes']],'little')
   if value!=expected:failures[field['field']].append({'tick':record['tick'],'native':value,'source':expected})
 results.append({'side':side,'updates':len(steps),'fields':len(fields),'always_matching_fields':len(fields)-len(failures),'mismatching_fields':{key:{'count':len(values),'first':values[0]} for key,values in failures.items()}})

assert all(not x['mismatching_fields'] for x in results)
report={'status':'PASS_CONTINUOUS_NATIVE_186_FIELDS','scope':'Same baseline-seeded continuous native journey. Field bit patterns compared through declared original widths; duplicate native copies included. Mismatches require owner/caller-order audit, not automatically original physics defects. No full actor or complete port equivalence claim.','results':results};(r/'reports/continuous-driving-context-field-audit.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
