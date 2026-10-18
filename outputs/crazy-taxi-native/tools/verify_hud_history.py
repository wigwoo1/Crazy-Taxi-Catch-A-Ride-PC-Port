import pathlib,json,re,hashlib
r=pathlib.Path('outputs/crazy-taxi-native');data={}
for label,binary in [('tip','scene-viewer-hud-history.exe'),('expiry-diagnostic','scene-viewer-hud-history-diagnostic.exe')]:
 log=(r/f'reports/hud-history-{label}.log').read_text();assert not (r/f'reports/hud-history-{label}.err').read_text()
 values=tuple(map(int,re.search(r'TIMESTAMPED HUD: clock(\d+) cash(\d+) tip_requests(\d+) expiry(\d+)',log).groups()));assert values[0]>0 and values[1]>0
 if label=='tip':assert values[2]>0 and 'PASS paced native input-only boarding and original passenger voice playback;' in log and 'LIVE TIP COVERAGE:' in log
 else:assert values[3]>0 and 'PASS normal startup gameplay music:' in log and 'forced initial clock1' in log
 data[label]={'clock_draws':values[0],'cash_draws':values[1],'tip_requests':values[2],'expiry_draws':values[3],'binary_sha256':hashlib.sha256((r/'bin'/binary).read_bytes()).hexdigest()}
assert 'Original gameplay expiry audio fade failed.' in (r/'reports/hud-history-expiry.err').read_text()
report={'status':'PASS_HUD_HISTORY_BRANCH_COVERAGE_WITH_UNRESOLVED_EARLIER_EXPIRY_FAILURE','runs':data,'scope':'Copied session owns clock/fare/trip/expiry/tip eligibility; perpose cash glyph rows and tip frame retained. Native tip run earns near-miss via input-only route; expiry run forces initial clock1 explicitly. Perdraw immutable assertions preserve session/RNG/current cash rows/tip frame/panel/effects. Subsequent expiry diagnostic and previous binary passed, but initial new-binary expiry failed with no retained stdout. Added stdout flush before native failure for future evidence; failure cause unresolved, not asserting fixed or ready to release. No independent full GPU/source pixel, natural full-mission timeout, sustained4K60 or Xbox device claim.'}
(r/'reports/hud-history-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS scoped HUD branches; earlier expiry failure remains unresolved')
