from pathlib import Path
r=Path('outputs/crazy-taxi-native')
for tool in ['verify_bound_pause_exits.py','verify_bound_pause_exit_audio.py']:
 s=(r/'tools'/tool).read_text().replace('scene-viewer-bound-pause-exit-probe-v5.exe','scene-viewer-bound-retained-input-probe.exe').replace('bound-pause-exit-','bound-retained-input-host-exit-');exec(compile(s,'retained-input-host-verification','exec'),{})
s=(r/'tools/verify_bound_session_order_main.py').read_text().replace('scene-viewer-bound-session-order.exe','scene-viewer-bound-retained-input-probe.exe').replace('bound-session-order-','bound-retained-input-regression-');exec(compile(s,'retained-input-regression','exec'),{})
