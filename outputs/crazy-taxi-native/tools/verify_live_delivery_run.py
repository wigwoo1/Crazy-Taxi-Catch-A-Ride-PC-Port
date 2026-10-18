from pathlib import Path
import re,json,hashlib,sys
r=Path('outputs/crazy-taxi-native');stem=sys.argv[1];binary=sys.argv[2];log=(r/f'reports/{stem}.log').read_text();err=(r/f'reports/{stem}.err').read_text();route=re.search(r'LIVE DELIVERY ROUTE: source(\d+) points(\d+) destination(-?\d+)',log);end=re.search(r'LIVE DELIVERY TERMINAL: presentations(\d+) sourcecounter(\d+) updates(\d+) completed(\d+) cash(\d+)\.(\d+) active(\d+) pending(-?\d+) waypoint(\d+) clock(-?\d+) rejected(\d+) unavailable(\d+)',log);assert route and end
terminal=dict(zip(['presentations','source_counter','updates','completed','cash_whole','cash_fraction','active','pending','waypoint','clock','rejected','unavailable'],map(int,end.groups())))
success='PASS LIVE DELIVERY:' in log
if success:assert not err and terminal['completed']>0 and terminal['cash_whole']>0 and terminal['active']==terminal['pending']==terminal['rejected']==terminal['unavailable']==0
else:assert 'Live input-only delivery/fare/audio did not complete.' in err
progress=[dict(zip(['counter','waypoint','points','clock','trip_clock','x','z','speed','heading_difference','brake'],map(int,x))) for x in re.findall(r'LIVE DELIVERY PROGRESS: sourcecounter(\d+) waypoint(\d+)/(\d+) remaining(-?\d+) trip(-?\d+) position(-?\d+),(-?\d+) speed(-?\d+) heading_difference(-?\d+) brake(\d+)',log)]
report={'status':'PASS_LIVE_INPUT_ONLY_DELIVERY_FARE' if success else 'FAILED_LIVE_DELIVERY_RECORDED','route':{'source':int(route[1]),'points':int(route[2]),'destination':int(route[3])},'terminal':terminal,'progress':progress,'binary_sha256':hashlib.sha256((r/f'bin/{binary}').read_bytes()).hexdigest(),'scope':'Native elapsed gameplay with input-only test driver and original recovered route. No pose/clock/cash/completion edits; internal4K and timestamped owners assertions. Success proves this one route/native integration only, not original whole-session differential equivalence, all fares, traffic/deviceXbox/audio listening or sustained60.'};
target=re.search(r'NATIVE4K TARGET: rendered(\d+) window blits(\d+)',log)
if target:
 report['native_4k_draws']=int(target[1]);report['native_window_blits']=int(target[2])
 assert report['native_4k_draws']==report['native_window_blits']
 report['scope']+=' Terminal presentations counter is not the actual4K draw count; do not derive renderer FPS from that field.'
experimental=re.search(r'EXPERIMENTAL UPDATE GROUP: minimum(\d+) source ticks',log)
if experimental:
 report['experimental_minimum_update_ticks']=int(experimental[1])
 report['cadence_matches_original_workload']=False
 report['scope']='Explicit experimental minimum-tick policy; actual original variable workload cadence NOT reproduced or verified. '+report['scope']
(r/f'reports/{stem}-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps({k:v for k,v in report.items() if k!='progress'},indent=2))
