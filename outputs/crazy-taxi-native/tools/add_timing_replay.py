import pathlib,json
root=pathlib.Path('outputs/crazy-taxi-native');ref=root/'reports/original-core-reference';rows=json.loads((ref/'boot3300trace.actor-trace.json').read_text())+json.loads((ref/'boot4200trace.actor-trace.json').read_text());out=[];last=None
for i,r in enumerate(rows):
 if i>=332 and r['previous']!=last:out.append(r['frame_delta'])
 last=r['previous']
assert len(out)>200 and out[0]==4 and all(1<=d<=60 for d in out)
(root/'reports/original-gameplay-step-replay.txt').write_text(''.join(str(d)+'\n' for d in out));print('Original replay steps',len(out),'total units',sum(out))
p=root/'src/scene_viewer.c';text=p.read_text();(root/'reports/scene-viewer-before-timing-replay.c').write_text(text)
text='#include "frame_time.h"\n'+text
anchor='static void drive_tick(TaxiXboxSample pad,int scripted) {'
new='''static FILE *gameplay_step_replay;
static TaxiFrameTime gameplay_frame_time;
static uint32_t gameplay_replay_counter;
static int32_t gameplay_global_delta=1;
static void drive_tick(TaxiXboxSample pad,int scripted) {
    int32_t frame_delta=1;
    if(gameplay_step_replay){
        unsigned elapsed;
        if(fscanf(gameplay_step_replay,"%u",&elapsed)!=1||elapsed<1||elapsed>60)fail("Original timing replay exhausted or invalid.");
        gameplay_replay_counter+=elapsed;
        if(taxi_frame_time_update(&gameplay_frame_time,gameplay_replay_counter,(uint8_t)native_pause.menu.frozen))fail("Original frame timing invalid.");
        frame_delta=gameplay_frame_time.delta_3b00;
    }
'''
assert anchor in text;text=text.replace(anchor,new)
a=text.index('static void drive_tick(');b=text.index('\nstatic ',a+25);part=text[a:b]
part=part.replace('int keyboard_enabled=!startup_game_audio_smoke&&!live_attract;','int keyboard_enabled=!gameplay_step_replay&&!startup_game_audio_smoke&&!live_attract;')
part=part.replace('.elapsed_ticks=1','.elapsed_ticks=(uint16_t)frame_delta').replace('.frame_delta=1','.frame_delta=frame_delta').replace('.global_delta=1','.global_delta=gameplay_global_delta').replace('.delta=1','.delta=frame_delta')
part=part.replace('TaxiGearInput input={.delta=frame_delta','TaxiGearInput input={.delta=gameplay_global_delta')
part=part.replace('taxi_game_start_step(&live_game_start,1,','taxi_game_start_step(&live_game_start,frame_delta,').replace('taxi_game_start_challenge_step(&live_game_start,1,','taxi_game_start_challenge_step(&live_game_start,frame_delta,')
part=part.replace('live_game_start_events=start_active?', '''/* Original BA44 copies frame delta only during phase-1 intro.
       The normal B00A caller copies it again before movement. */
    if(gameplay_step_replay&&start_active&&live_game_start.phase==-1)gameplay_global_delta=frame_delta;
    live_game_start_events=start_active?''')
part=part.replace('int movement_blocked=normal_reward_frame', '''if(gameplay_step_replay&&!start_active)gameplay_global_delta=frame_delta;
    input.delta=gameplay_global_delta;
    int movement_blocked=normal_reward_frame''')
part=part.replace('car.world.heading_20,1,(int16_t)','car.world.heading_20,frame_delta,(int16_t)').replace('(int16_t)car.world.angle_18,1,','(int16_t)car.world.angle_18,frame_delta,').replace('camera_yaw_offset,camera_yaw_target,1)','camera_yaw_offset,camera_yaw_target,frame_delta)')
part=part.replace('car.driver.power.gears.flags_4ac,1,paused,','car.driver.power.gears.flags_4ac,frame_delta,paused,').replace('.delta=frame_delta,.clock=60','.delta=gameplay_global_delta,.clock=60').replace('0,1,60)<0','0,gameplay_global_delta,60)<0')
part=part.replace('&cash_glyphs,1,(uint8_t)','&cash_glyphs,frame_delta,(uint8_t)').replace('taxi_destination_vertical_scale(remaining),1)','taxi_destination_vertical_scale(remaining),frame_delta)')
text=text[:a]+part+text[b:]
cli='else if (!strcmp(argv[i],"--startup-idle-check"))'
text=text.replace(cli,'''else if (!strcmp(argv[i],"--gameplay-step-replay")&&i+1<argc) {
            gameplay_step_replay=fopen(argv[++i],"r");if(!gameplay_step_replay)fail("Original timing replay file unavailable.");
        }
        '''+cli)
text=text.replace('if(start_capture_mask!=7)fail','if(!gameplay_step_replay&&start_capture_mask!=7)fail').replace('live_game_start.active||start_ticks<315||simulation_error','live_game_start.active||(!gameplay_step_replay&&start_ticks<315)||simulation_error')
text=text.replace('if(startup_gameplay_check) {','if(gameplay_step_replay&&(!startup_gameplay_check||!startup_idle_check||route_probe_active))fail("Timing replay requires ordinary startup idle diagnostic.");\n    if(startup_gameplay_check) {',1)
p.write_text(text)
build=(root/'tools/rebuild_playable_verified.ps1').read_text().replace("$viewerCompilerArgs += @('-UNDEBUG')","$viewerCompilerArgs += @('-UNDEBUG',(Join-Path $projectRoot 'src/frame_time.c'))")
(root/'tools/rebuild_timing_replay.ps1').write_text(build)
print('Added diagnostic timing replay; installed executable unchanged.')
