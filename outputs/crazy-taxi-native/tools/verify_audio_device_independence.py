"""Compare bounded native startup observations at the same logical frame."""
from pathlib import Path
import hashlib,json,re
root=Path('outputs/crazy-taxi-native')
logs=['reports/audio-logical-checkpoint-disabled.log','reports/audio-logical-checkpoint-enabled.log']
checkpoints=[]
for name in logs:
    text=(root/name).read_text()
    match=re.search(r'^AUDIO CHECKPOINT: tick900 frames(\d+) PCMhash([0-9a-f]+) RNG(\d+) mixed(\d+) dispatches(\d+) preFX(\d+)\.$',text,re.M)
    assert match,(name,'checkpoint missing')
    assert 'PASS normal startup gameplay music:' in text
    assert '0 rejected, 0 unavailable output buffers' in text
    checkpoints.append(match.groups())
assert checkpoints[0]==checkpoints[1],checkpoints
files=['src/scene_viewer.c','src/audio_frame.c','src/audio_frame.h','bin/scene-viewer.exe','tools/verify_audio_device_independence.py','reports/audio-device-independent-build.log','reports/audio-frame-refresh-build.log','reports/audio-frame-refresh-comparison.log','reports/audio-frame-rom-comparison.json']+logs
report={'status':'PASS','scope':'Native startup at exact logical tick900, PC output disabled without audio-preview vs PC output enabled. Logical source audio assets/frame processing independent of Windows output device.', 'checkpoint':dict(zip(['frames','PCM_FNV1a64','shared_RNG','music_frames','dispatches','preFX_nonzero'],checkpoints[0])), 'limits':['Matching generated PCM fingerprint is not captured speaker-output proof.','Terminal totals952/953 differ from presentation catch-up; comparison uses exact logical tick900 without altering cadence.','Original initial seed, reset26704 five-frame wait, full loading scheduling and sound ownership remain incomplete.','This verifies startup only, not every mode/lifecycle or original GBA trajectory.'], 'overall_completion_estimate_percent':30,'estimate_confidence':'low','sha256':{f:hashlib.sha256((root/f).read_bytes()).hexdigest() for f in files}}
(root/'reports/audio-device-independence-verification.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS device independence at tick900:',report['checkpoint'])
