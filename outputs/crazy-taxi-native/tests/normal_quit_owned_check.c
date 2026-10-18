#define TAXI_NORMAL_RETRY_TEST_ENTRY taxi_previous_retry_owned_check_main
#include "normal_retry_owned_check.c"
int main(int argc,char **argv){
 assert(!taxi_previous_retry_owned_check_main(argc,argv));
 assert(argc>=4);FILE *fp=fopen(argv[1],"rb");uint8_t *rom=malloc(8388608);assert(fp&&rom&&fread(rom,1,8388608,fp)==8388608&&!fclose(fp));
 Full *a=malloc(sizeof *a);assert(a);
 for(unsigned city=0;city<2;city++)for(unsigned qualified=0;qualified<2;qualified++){
  memset(a,0,sizeof *a);core_initialize(&a->core,rom,argv[2],city);char name[128];assert(snprintf(name,sizeof name,"assets/scene-descriptors/city%u.bin",city+1)>0);raw(argv[2],name,a->descriptor,104,1);uint8_t trig[16388];raw(argv[2],"assets/driving.ctr",trig,sizeof trig,0);memcpy(a->sine,trig+4,8192);memcpy(a->cosine,trig+8196,8192);
  bind_full(a,rom,city,0,argv[3]);Native *n=&a->core.n;Fixture *f=&n->f;TaxiNormalRestartFlow flow={.graphics={.retained_sl=1,.context_base=0x02002634,.city=(uint8_t)city}};finish_restart(a,&flow);
  unsigned fade_pubs=0;while(f->fade.fade.active_177b&&fade_pubs++<128){assert(!taxi_ordinary_loading_pending_prepare_present(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));release(f);assert(taxi_ordinary_loading_pending_poll(&f->owner)==1);}assert(!f->fade.fade.active_177b);
  n->scene.session.fare.qualified_1691=n->session.session_165e[51]=(uint8_t)qualified;
  TaxiPauseInput steps[6]={{.start=1},{.down=1},{.down=1},{.confirm=1},{.up=1},{.confirm=1}};TaxiPauseEvents event;TaxiAudioSettingsState audio={17,19,128,255,0};a->tracker.state.playing=99;
  for(unsigned i=0;i<6;i++){assert(!taxi_pause_menu_step(&f->pause,steps+i,&event));if(event.pause_audio){assert(!taxi_music_playback(&a->tracker.state.playing,TAXI_MUSIC_PAUSE));assert(!taxi_audio_settings_disable_effects(&audio));}}
  assert(event.fade_out&&f->pause.outer_state==2&&f->pause.exit_flag&&f->pause.frozen);
  TaxiSoundAsset sound={0};TaxiAudioChannel channels[4]={0};for(unsigned j=0;j<4;j++){channels[j].sound=&sound;channels[j].stamp=55+j;channels[j].volume=(uint8_t)(13+j);}
  uint32_t handles[10]={1,1,0,4,2,3,0,1,4,3};memcpy(a->core.audio.handles,handles,sizeof handles);TaxiGameAudioController retained_audio=a->core.audio;uint16_t rate=16000;uint8_t exit=165;
  TaxiNormalRetryOwned quit={.pause=&f->pause,.loading=&f->owner,.session=&n->session,.scene=&n->scene,.sounds=&a->core.audio,.channels=channels,.audio=&audio,.tracker=&a->tracker,.output_rate=&rate,.override_1708=&f->state.gate.override_1708,.exit_5f1a=&exit};
  TaxiDriveWorld car=a->core.car;TaxiPassengerState actors[128];memcpy(actors,n->scene.customers,sizeof actors);uint32_t rng=n->scene.random_state;TaxiPaletteFadeStart before_fade=f->fade;
  assert(taxi_normal_retry_owned_fade(&quit,&event)==-1&&!memcmp(&before_fade,&f->fade,sizeof before_fade));
  TaxiNormalRetryOwned invalid=quit;invalid.channels=NULL;assert(taxi_normal_quit_owned_fade(&invalid,&event)==-1&&!memcmp(&before_fade,&f->fade,sizeof before_fade));
  f->pause.exit_flag=0;assert(taxi_normal_quit_owned_fade(&quit,&event)==-1&&!memcmp(&before_fade,&f->fade,sizeof before_fade));f->pause.exit_flag=1;
  assert(!taxi_normal_quit_owned_fade(&quit,&event)&&quit.phase==1&&f->fade.masks.background_mask==0x201&&f->fade.masks.object_mask==0x201&&exit==1);
  fade_pubs=0;while(f->fade.fade.active_177b&&fade_pubs++<128){assert(!taxi_normal_quit_owned_transfer(&quit)&&!quit.stops);assert(!taxi_ordinary_loading_pending_prepare_present(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));for(unsigned j=0;j<8;j++)assert(!taxi_normal_quit_owned_transfer(&quit));release(f);assert(taxi_ordinary_loading_pending_poll(&f->owner)==1);}
  assert(!f->fade.fade.active_177b);f->owner.phase=TAXI_LOADING_WAITING;assert(!taxi_normal_quit_owned_transfer(&quit)&&!quit.stops);f->owner.phase=TAXI_LOADING_IDLE;
  assert(taxi_normal_quit_owned_transfer(&quit)==1&&quit.phase==2&&quit.stops==8&&quit.requests.count==4);
  const uint32_t expected[4]={0x08010aac,0x08027754,0x0802771c,0x08027730};assert(!memcmp(quit.requests.calls,expected,sizeof expected));
  TaxiNormalRetryOwned retained=quit;TaxiAudioChannel stopped[4];memcpy(stopped,channels,sizeof stopped);for(unsigned j=0;j<8;j++)assert(taxi_normal_quit_owned_transfer(&quit)==1&&!memcmp(&retained,&quit,sizeof quit)&&!memcmp(stopped,channels,sizeof stopped));
  assert(!memcmp(&retained_audio,&a->core.audio,sizeof retained_audio)&&audio.effects_enabled==255&&!a->tracker.state.playing&&rng==n->scene.random_state&&!memcmp(&car,&a->core.car,sizeof car)&&!memcmp(actors,n->scene.customers,sizeof actors));
  TaxiHostTime clock=f->host;TaxiFrameWait wait=f->wait;int rc=taxi_normal_quit_owned_prepare(&quit);assert(rc==(qualified?2:1));
  if(qualified){assert(quit.phase==3&&quit.parent.phase==3&&quit.requests.count==1&&quit.requests.calls[0]==0x080243a4&&f->pause.frozen&&!quit.parent.callback);retained=quit;for(unsigned j=0;j<16;j++){assert(!taxi_normal_quit_owned_prepare(&quit)&&!taxi_normal_quit_owned_resume(&quit,0)&&!memcmp(&retained,&quit,sizeof quit)&&!memcmp(&clock,&f->host,sizeof clock)&&!memcmp(&wait,&f->wait,sizeof wait));}assert(taxi_normal_quit_owned_resume(&quit,1)==1);}
  assert(quit.phase==4&&!quit.requests.count&&!quit.parent.phase&&quit.parent.callback==0x080174dd&&quit.parent.main_mode&&quit.parent.redraw&&!quit.parent.results&&!quit.parent.frozen&&!quit.parent.outer&&!quit.parent.ended);
  assert(!f->pause.frozen&&!f->pause.outer_state&&!n->session.outer_1ea8&&!n->scene.session.outer_state_1ea8&&!n->session.session_165e[57]&&!f->service.frozen_5f20&&exit==1&&f->pause.exit_flag==1);
  assert(!memcmp(&clock,&f->host,sizeof clock)&&!memcmp(&wait,&f->wait,sizeof wait)&&rng==n->scene.random_state&&!memcmp(&car,&a->core.car,sizeof car)&&!memcmp(actors,n->scene.customers,sizeof actors));
  retained=quit;for(unsigned j=0;j<8;j++)assert(taxi_normal_quit_owned_prepare(&quit)==1&&taxi_normal_quit_owned_resume(&quit,1)==1&&!memcmp(&retained,&quit,sizeof quit));
  printf("PASS native pause Quit city%u qualified%u: actual201 fade, ordered8 stops, retained handles/RNG/car/actors/clock/wait, real credits-return gate, once-only174DD menu callback\n",city+1,qualified);fflush(stdout);cleanup(&a->core.n);
 }
 free(a);free(rom);puts("PASS native Quit protocol plus prior complete nativeRetry checks; credits child return is an explicit unit fixture, not earned/live audio proof");return 0;
}