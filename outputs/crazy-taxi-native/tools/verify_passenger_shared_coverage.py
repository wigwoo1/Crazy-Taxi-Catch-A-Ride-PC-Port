import pathlib,re,json,hashlib
r=pathlib.Path('outputs/crazy-taxi-native');log=(r/'reports/passenger-shared-audit.log').read_text();assert 'PASS paced native input-only boarding and original passenger voice playback;' in log
match=re.search(r'PASSENGER CULL: records(\d+) eligibility_skips(\d+) attempts(\d+) culled(\d+) visible(\d+)',log);assert match
records,skips,attempts,culled,visible=map(int,match.groups());assert records==skips+attempts and attempts==culled+visible and visible>0
rows=[line.split() for line in (r/'reports/passenger-shared-records.txt').read_text().splitlines()];assert len(rows)==attempts
for row in rows:assert len(row)==14 and len(bytes.fromhex(row[0]))==44 and int(row[-1]) in (0,1)
assert sum(int(row[-1]) for row in rows)==visible
traffic=int(re.search(r'TIMESTAMPED TRAFFIC VIEWS: (\d+)',log)[1]);assert traffic>0
report={'status':'PASS_HISTORICAL_SHARED_PASSENGER_RENDER_COVERAGE','records':records,'eligibility_skips':skips,'projection_attempts':attempts,'projection_culled':culled,'queued_passenger_views':visible,'prepared_traffic_views':traffic,'binary_sha256':hashlib.sha256((r/'bin/scene-viewer-passenger-cull-audit.exe').read_bytes()).hexdigest(),'scope':'Scripted native pickup withtrafficenabledsharedqueue; exact traced projection input/count accounting, car/context/traffic/passengerscene immutability assertions. Queue count is not GPU visiblepixel/original sprite fidelity proof. Prior zero was omitted sharedqueue, legacyfallbackstillneeds timestamp synchronization or explicitguard.'};(r/'reports/passenger-shared-coverage-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(report)
