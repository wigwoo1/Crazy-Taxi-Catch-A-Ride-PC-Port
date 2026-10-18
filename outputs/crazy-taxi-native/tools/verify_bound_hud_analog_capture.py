from pathlib import Path
import os,subprocess,json,hashlib
r=Path('outputs/crazy-taxi-native');rep=r/'reports'
keys=[0]*450
for j in range(220,450):keys[j]=1|((16,32,0,48)[(j//9)%4])|(2 if j%23<5 else 0)|(256 if j%31<7 else 0)|(512 if j%41<6 else 0)
keyfile=rep/'bound-hud-analog-drive-keys.txt';keyfile.write_text('\n'.join(f'0 {(-32768,-16384,-7850,0,7850,16384,32767)[(j//17)%7] if j>=220 else 0} 0 {255 if j>=220 else 0} {255 if j>=220 and j%43<6 else 0}' for j in range(450))+'\n');step=rep/'bound-hud-analog-drive-ticks.txt';step.write_text('initial_delta 1\n'+'1\n'*450)
env=os.environ.copy();env['TAXI_DRIVER_TRACE']=str(rep/'bound-hud-analog-driver.txt');env['TAXI_WORLD_TRACE']=str(rep/'bound-hud-analog-world.txt');env['TAXI_BOUND_PAUSE_TRACE']='1';env['TAXI_BOUND_COMMAND_TRACE']=str(rep/'bound-hud-analog-command-actions.txt')
exe=r/'bin/scene-viewer-bound-command-capture.exe';p=subprocess.run([str(exe),'--challenge-bound-drive-preview','--ticks','450','--effect-state-trace',str(rep/'bound-hud-analog-effects.txt'),'--camera-owner-trace','--camera-state-trace',str(rep/'bound-hud-analog-camera.txt'),'--gameplay-xbox-replay',str(keyfile),'--gameplay-step-replay',str(step),'--snapshot',str(rep/'bound-hud-analog-command-actions.bmp')],env=env,capture_output=True,text=True,timeout=45,creationflags=subprocess.CREATE_NO_WINDOW)
(rep/'bound-hud-analog-command-actions.log').write_text(p.stdout);(rep/'bound-hud-analog-command-actions.err').write_text(p.stderr);assert p.returncode==0 and not p.stderr,p.stderr
ns={};exec((r/'tools/compare_player_command_rom.py').read_text().split('\ncases=')[0],ns);rows=[list(map(int,l.split())) for l in (rep/'bound-hud-analog-command-actions.txt').read_text().splitlines()];assert rows
for row in rows:
 tick,ended,profile,flags=row[:4];buttons=row[4:34];command=row[34:50];actions=row[50:];assert len(row)==54
 streams=[sum(buttons[j*3+k]<<j for j in range(10)) for k in range(3)]
 for action,expected in zip((2,3,4,5),actions):
  q=ns['ActionProbe']((0,0,0,0,0,1),0);q.initialize(tuple(streams)+(profile,flags,0,0,action));q.arm.mem[0x2001694]=ended<<24;q.run(entry=0x8015d98,stop=0x10020000);assert q.reg[0]==expected,(tick,action,q.reg[0],expected)
 q=ns['CommandProbe']((0,0,0,0,0,1),0);q.initialize(tuple(streams)+(profile,flags,0,0,0));q.arm.mem[0x2001694]=ended<<24
 for j in range(16):q.write(0x30053d0+j,0,1)
 q.reg[0]=0;q.run(entry=0x8011734,stop=0x10020000);got=[q.read(0x30053d0+j,1) for j in range(16)]
 assert all(got[j]==command[j] for j in range(16) if j not in (12,13)),(tick,got,command)
report=dict(status='PASS',driver_calls=len(rows),actions=len(rows)*4,distinct_held=len({tuple(row[4:34:3]) for row in rows}),distinct_pressed=len({tuple(row[5:34:3]) for row in rows}),executable_sha256=hashlib.sha256(exe.read_bytes()).hexdigest(),scope='Actual current450callback injectedXbox stick/trigger replay; hostmapping extension; original11734 compares14 commandbytes (analog12/13 excluded);15D98 allfouractions fromcapturedheld/pressed/repeat/profile/vehicleflags/endflag. Originalper-call fixtures, not sampler/physicalXbox/analogsource/continuouswholegame.')
(rep/'bound-hud-analog-command-actions-verification.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS',report)
