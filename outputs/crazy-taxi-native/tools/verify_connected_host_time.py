import pathlib,json,hashlib,sys
root=pathlib.Path('outputs/crazy-taxi-native')
stem=sys.argv[1] if len(sys.argv)>1 else 'connected-host-time'
binary=sys.argv[2] if len(sys.argv)>2 else 'scene-viewer-connected-host-time-verified.exe'
rows=[list(map(int,s.split())) for s in (root/f'reports/{stem}.txt').read_text().splitlines()]
assert rows and all(len(r)==10 for r in rows)
phase=counter=previous=0;delta=1;updates=rows[0][7];ready=0;commits=skips=0
assert updates in (0,1) # one initial ready update may run in the loading-to-gameplay presentation
base_audio=rows[0][9]-rows[0][8]
for ns,ticks,prior,now,after,freeze,committed,count,audio_ticks,audio_frames in rows:
    updates+=ready
    assert count==updates,(count,updates)
    assert prior==delta
    expected,phase=divmod(phase+ns*262144,4389000000000)
    assert ticks==expected
    counter=(counter+ticks)&0xffffffff
    assert now==counter and audio_ticks==counter
    assert audio_frames-audio_ticks==base_audio
    due=int(counter!=previous);assert committed==due
    if due:
        diff=(counter-previous)&0xffffffff
        signed=diff if diff<0x80000000 else diff-0x100000000
        delta=max(1,min(60,signed));previous=counter;commits+=1
    else:skips+=1
    assert after==delta
    ready=due
log=(root/f'reports/{stem}.log').read_text()
assert 'PASS normal startup gameplay music:' in log
assert 'LOADING AUDIO: 52 source presentation frames serviced.' in log
assert '0 rejected, 0 unavailable output buffers' in log
for index in (51,52,53):assert f'AUDIO REQUEST: original index{index} count1.' in log
report={'status':'PASS_CONNECTED_TIMING_AUDIO_INTEGRATION','trace_rows':len(rows),'gameplay_updates':updates,'commits':commits,'render_only_events':skips,'elapsed_ticks':counter,'prior_menu_loading_audio_frames':base_audio,'scope':'Connected menu/loading/ordinary-gameplay transition; original countdown51/52/53 exactly once, nonzero music and full recovered sound controller. Host rational clock, commit clamp and audio tick accounting checked. Independent original continuous trajectory, complete boot RNG, fading timestep ownership, interpolation, focus/reentry and physical display/controller unverified. Output disabled; no listening claim.','binary_sha256':hashlib.sha256((root/'bin'/binary).read_bytes()).hexdigest()}
(root/f'reports/{stem}-verification.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
