import pathlib
root=pathlib.Path('outputs/crazy-taxi-native');p=root/'src/scene_viewer.c';s=p.read_text();s=s.replace('static FILE *gameplay_key_replay;','static FILE *gameplay_key_replay;\nstatic FILE *gameplay_xbox_replay;')
a='    uint8_t paused=(uint8_t)(native_pause.menu.frozen&&!live_attract&&!challenge_jump_drive_preview);';s=s.replace(a,'''    if(gameplay_xbox_replay){
        unsigned buttons,rt,lt;int lx,ly;
        if(fscanf(gameplay_xbox_replay,"%u %d %d %u %u",&buttons,&lx,&ly,&rt,&lt)!=5||buttons>65535||lx<-32768||lx>32767||ly<-32768||ly>32767||rt>255||lt>255)fail("Native Xbox replay exhausted or invalid.");
        pad=taxi_xbox_decode((uint16_t)buttons,(int16_t)lx,(int16_t)ly,(uint8_t)rt,(uint8_t)lt);scripted=0;
    }
'''+a,1)
a='else if (!strcmp(argv[i],"--gameplay-key-replay")&&i+1<argc) {';s=s.replace(a,'''else if (!strcmp(argv[i],"--gameplay-xbox-replay")&&i+1<argc) {
            gameplay_xbox_replay=fopen(argv[++i],"r");if(!gameplay_xbox_replay)fail("Native Xbox replay file unavailable.");
        }
        '''+a)
s=s.replace('if(gameplay_key_replay&&!gameplay_step_replay)', 'if(gameplay_key_replay&&gameplay_xbox_replay)fail("Input replay sources must be exclusive.");\n    if((gameplay_key_replay||gameplay_xbox_replay)&&!gameplay_step_replay)')
s=s.replace('puts(startup_idle_check?"STARTUP IDLE:', 'puts(gameplay_key_replay||gameplay_xbox_replay?"STARTUP INPUT REPLAY: selected city/driver->60 controlled native input ticks passed; full fidelity incomplete.":startup_idle_check?"STARTUP IDLE:')
p.write_text(s)
for label,stick in [('left',-32768),('right',32767),('center',0),('deadzone',7849)]:
 (root/f'reports/native-timing-xbox-{label}.txt').write_text('0 0 0 0 0\n'*114+'0 0 0 255 0\n'*30+(f'0 {stick} 0 255 0\n')*30)
print('Added native decoded-XInput replay for trigger/stick direction tests.')
