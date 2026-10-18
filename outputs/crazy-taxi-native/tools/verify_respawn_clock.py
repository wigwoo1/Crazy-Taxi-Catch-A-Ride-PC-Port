import pathlib,json,hashlib,sys
root=pathlib.Path('outputs/crazy-taxi-native')
stem=sys.argv[1] if len(sys.argv)>1 else 'respawn-clock'
binary=sys.argv[2] if len(sys.argv)>2 else 'scene-viewer-respawn-clock.exe'
rows=[list(map(int,l.split())) for l in (root/f'reports/{stem}.txt').read_text().splitlines()]
assert len(rows)==360 and all(len(r)==10 for r in rows)
phase=counter=previous=0;delta=1;ready=1;updates=0
for index,(ns,ticks,prior,now,after,freeze,commit,count,audio_ticks,audio_frames) in enumerate(rows):
 if index==120:delta=1;ready=1;assert ns<120000000
 updates+=ready;assert count==updates and prior==delta
 expected,phase=divmod(phase+ns*262144,4389000000000);assert ticks==expected
 counter=(counter+ticks)&0xffffffff;assert now==counter and audio_ticks==counter and audio_frames==counter
 due=int(counter!=previous);assert commit==due
 if due:
  diff=(counter-previous)&0xffffffff;signed=diff if diff<0x80000000 else diff-0x100000000
  delta=max(1,min(60,signed));previous=counter
 assert after==delta;ready=due
log=(root/f'reports/{stem}.log').read_text();assert 'PASS RESPAWN CLOCK:' in log
report={'status':'PASS_CAPTURED_RESPAWN_CLOCK','frames':len(rows),'gameplay_updates':updates,'elapsed_audio_ticks':counter,'reset_presentation':121,'first_post_reset_ns':rows[120][0],'first_post_reset_input_delta':rows[120][2],'binary_sha256':hashlib.sha256((root/'bin'/binary).read_bytes()).hexdigest(),'scope':'Actual captured trace validates retained rational clock phase/counter, one reset-ready update and first input delta1, reset interval excludes injected120ms work, every elapsed tick mixed once. Controlled native reset, not keyboard/OS/device/source original full reset/cadence equivalence.'}
(root/f'reports/{stem}-verification.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
