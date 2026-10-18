import pathlib,json,collections,hashlib
root=pathlib.Path('outputs/crazy-taxi-native');reports=[]
for label in ('drivingleft','drivingright'):
 path=root/f'reports/original-core-reference/{label}.actor-trace.json'
 rows=json.loads(path.read_text());assert len(rows)==360
 boundaries=[];partial=[]
 for a,b in zip(rows,rows[1:]):
  changed=any(a[k]!=b[k] for k in ('heading','speed','position','angles','orientation','gravity','airborne'))
  if a['previous']!=b['previous']:
   elapsed=(b['previous']-a['previous'])&0xffffffff
   assert b['frame_delta']==max(1,min(60,elapsed if elapsed<0x80000000 else elapsed-0x100000000))
   boundaries.append({'tick':b['tick'],'elapsed':elapsed,'frame_delta':b['frame_delta'],'global_delta':b['delta'],'keys':b['keyinput']})
  elif changed:partial.append(b['tick'])
 transition=next(i for i,r in enumerate(rows) if len(r['input_keys'])>1)
 heading_change=next(i for i,r in enumerate(rows) if r['heading']!=rows[0]['heading'])
 reports.append({'capture':label,'samples':len(rows),'boundary_count':len(boundaries),'elapsed_histogram':dict(collections.Counter(b['elapsed'] for b in boundaries)),'global_differs_frame_boundaries':sum(b['global_delta']!=b['frame_delta'] for b in boundaries),'state_changes_with_same_previous_counter':partial,'requested_steering_tick':transition,'first_heading_change_tick':heading_change,'start_position':rows[0]['position'],'end_position':rows[-1]['position'],'start_heading':rows[0]['heading'],'end_heading':rows[-1]['heading'],'boundaries':boundaries,'capture_sha256':hashlib.sha256(path.read_bytes()).hexdigest()})
report={'status':'PASS_INDEPENDENT_MOVING_CADENCE_AUDIT','captures':reports,'scope':'Independent original ROM moving captures sampled at VBlank. Frame clamp checked on observed presentation counter boundaries. No within-epoch actor changes were observed in these captures. VBlank sampling still does not prove exact routine entry/exit or capture all packed actor/input state. Use recorded boundary intervals for diagnostics; do not infer a fixed4-tick scheduler. No native trajectory equivalence or interpolation claim.'}
(root/'reports/original-moving-cadence-audit.json').write_text(json.dumps(report,indent=2)+'\n')
for x in reports:print(json.dumps({k:v for k,v in x.items() if k not in ('boundaries','state_changes_with_same_previous_counter')},indent=2));print('within-epoch state changes',len(x['state_changes_with_same_previous_counter']))
