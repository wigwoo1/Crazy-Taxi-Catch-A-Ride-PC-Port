"""Report full-controller regression evidence without hiding divergence."""
import pathlib,json,hashlib,sys
P=pathlib.Path('./');B=P/'outputs/crazy-taxi-native';W=B/'work/llvm-build-20261014';R=B/'reports/original-core-reference'
variant=sys.argv[1] if len(sys.argv)>1 else 'initial'
assert variant in ('initial','cooldown','bank_clock')
prefix={'initial':'full-audio','cooldown':'full-audio-cooldown','bank_clock':'audio-clock'}[variant]
binary={'initial':'continuous-driver-traffic-full-audio.exe','cooldown':'continuous-driver-traffic-full-audio-cooldown.exe','bank_clock':'continuous-driver-traffic-audio-clock.exe'}[variant]
test='tests/continuous_driver_traffic_audio_clock_probe.c' if variant=='bank_clock' else 'tests/continuous_driver_traffic_full_audio_probe.c'
old=(W/'state-traffic-235-native.log').read_text().splitlines()
new=(W/(prefix+'-235-native.log')).read_text().splitlines()
def is_service(l):return l.startswith('A ') and len(l.split())==11
services=[l for l in new if is_service(l)]
def relevant(l):return not l.startswith(('V ','H ')) and not is_service(l)
a=[l for l in old if relevant(l)];b=[l for l in new if relevant(l)]
diffs=[{'line':j,'baseline':x,'full_controller':y} for j,(x,y) in enumerate(zip(a,b)) if x!=y]
report={'status':'PASS_FULL_CONTROLLER_EXECUTION_AND_GAME_STATE_REGRESSION' if len(services)==235 and len(a)==len(b) and not diffs else 'DIVERGENCE_FULL_CONTROLLER_REPLAY','native_service_count':len(services),'baseline_lines':len(a),'full_controller_lines':len(b),'mismatched_lines':len(diffs),'first_differences':diffs[:12],'limits':['Bank-clock variant uses source observation ticks plus one completed frame; other variants use logical driver stamps. Exact IRQ instruction timing and autonomous scheduling remain unproven.','V/H physical audio fields are deliberately excluded from game-state regression and require independent source snapshot comparison.','Offline test only; production startup and autonomous scheduler unchanged.'],'sha256':{p:hashlib.sha256((B/p).read_bytes()).hexdigest() for p in [test,'work/llvm-build-20261014/'+binary,'work/llvm-build-20261014/'+prefix+'-235-native.log']}}
(R/('full-audio-replay-'+variant+'-20261014.json')).write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps({k:report[k] for k in ['status','native_service_count','baseline_lines','full_controller_lines','mismatched_lines','first_differences']}))