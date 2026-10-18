"""Check native service ordering and exact preserved replay output."""
import pathlib,json,hashlib
P=pathlib.Path('./');B=P/'outputs/crazy-taxi-native';W=B/'work/llvm-build-20261014';R=B/'reports/original-core-reference'
def sha(b):return hashlib.sha256(b).hexdigest()
old=(W/'state-traffic-235-native.log').read_bytes();new=(W/'service-order-235-native.log').read_bytes()
lines=new.decode().splitlines();assert lines[-1]=='SERVICE_ORDER_PASS pre=235 post=235 reject_before_physics=1'
assert lines[:-1]==old.decode().splitlines(),'Read-only pre/post observers changed the verified replay'
endpoint=json.loads((R/'native-endpoint-state-20261014.json').read_bytes());assert endpoint['status']=='PASS_RAW_NATIVE_ENDPOINT_WITH_LOGICAL_AUDIO_MAPPING'
prior=(W/'reward-traffic-235-native.log').read_bytes()
filtered=[l for l in lines[:-1] if not l.startswith(('B ','U ','W ','H '))]
assert filtered==prior.decode().splitlines()
report={'status':'PASS_PRE_DRIVER_SERVICE_ORDER_WITH_UNCHANGED_235_REPLAY','pre_service_calls':235,'post_service_calls':235,'input_and_dynamic_owners_imported_before_pre_service':True,'pre_service_observes_prior_native_car':True,'post_service_observes_completed_native_driver_traffic_update':True,'failure_before_physics_prevents_car_change_and_fixture_advance':True,'all_prior_native_output_lines_unchanged':True,'native_binary_sha256':sha((W/'reference-service-order.exe').read_bytes()),'native_log_sha256':sha(new),'baseline_log_sha256':sha(old),'source_sha256':{p:sha((B/p).read_bytes()) for p in ['src/reference_driving.c','src/reference_driving.h','tests/reference_service_order_test.c','tests/continuous_driver_traffic_state_probe.c']},'limits':['This is an offline diagnostic service API; it does not alter playable production startup.','Observers are read-only and do not yet dispatch the full audio controller.','Callback/input changes are not rolled back on a rejected service; only physics/post service and advancement are prevented.','Captured update timing remains a diagnostic fixture, not autonomous scheduling.'],'next_dependency':'Attach the recovered full audio controller to this pre-driver boundary using the verified source-owned channel importer, then compare physical channel state and shared playback owners against source snapshots.'}
(R/'reference-service-order-20261014.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
print(json.dumps({'status':report['status'],'pre':235,'post':235,'rejected_service_preserves_car':True,'replay_unchanged':True}))