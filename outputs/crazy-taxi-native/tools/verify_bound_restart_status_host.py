from pathlib import Path
r=Path('outputs/crazy-taxi-native')
for tool in ['verify_bound_pause_exits.py','verify_bound_pause_exit_audio.py']:
 s=(r/'tools'/tool).read_text().replace('scene-viewer-bound-pause-exit-probe-v5.exe','scene-viewer-bound-restart-status-probe.exe').replace('bound-pause-exit-','bound-restart-status-exit-')
 exec(compile(s,'restart-status-host-verification','exec'),{})
