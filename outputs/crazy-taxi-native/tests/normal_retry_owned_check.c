#ifndef TAXI_NORMAL_RETRY_TEST_ENTRY
#define TAXI_NORMAL_RETRY_TEST_ENTRY main
#endif
#define TAXI_NORMAL_RESTART_TEST_ENTRY taxi_full_restart_native_check_main
#include "normal_restart_native_check.c"
#include "normal_retry_owned.h"
#include "music_playback.h"
static void finish_restart(Full *a,TaxiNormalRestartFlow *s){
 Fixture *f=&a->core.n.f;int rc=taxi_normal_restart_flow_begin(s,taxi_normal_restart_presentation_owned,&f->bridge);assert(!rc);
 for(unsigned i=0;!rc&&i<4096;i++){
  rc=taxi_normal_restart_flow_step(s,taxi_normal_restart_presentation_owned,&f->bridge);assert(rc>=0);
  if(f->bridge.active_target||f->bridge.delegated_target)release(f);
 }
 assert(rc==1&&s->phase==TAXI_RESTART_READY&&!a->root.aborted);
}
int TAXI_NORMAL_RETRY_TEST_ENTRY(int argc,char **argv){
 assert(!taxi_full_restart_native_check_main(argc,argv));
 uint8_t *rom=malloc(8388608);assert(rom);FILE *file=fopen(argv[1],"rb");assert(file&&fread(rom,1,8388608,file)==8388608&&fgetc(file)==EOF&&!fclose(file));
 TaxiGraphicsPackage package={0};const uint8_t *graphics=rom;if(argc==5){assert(!taxi_graphics_package_load(&package,argv[4]));graphics=package.bytes;}
 Full *a=malloc(sizeof *a);assert(a);
 for(unsigned city=0;city<2;city++){
  memset(a,0,sizeof *a);core_initialize(&a->core,graphics,argv[2],city);char name[128];assert(snprintf(name,sizeof name,"assets/scene-descriptors/city%u.bin",city+1)>0);raw(argv[2],name,a->descriptor,104,1);uint8_t trig[16388];raw(argv[2],"assets/driving.ctr",trig,sizeof trig,0);memcpy(a->sine,trig+4,8192);memcpy(a->cosine,trig+8196,8192);memset(&a->guidance,0xa5,sizeof a->guidance);
  for(unsigned run=0;run<2;run++){
   bind_full(a,rom,city,run,argv[3]);Native *n=&a->core.n;Fixture *f=&n->f;
   /* AF80 starts an active fadeIN; let actual native26318 complete it
      before supplying post-intro driving gates for a subsequent Retry. */
   unsigned entry_publications=0;
   while(f->fade.fade.active_177b&&entry_publications<128){
    assert(!taxi_ordinary_loading_pending_prepare_present(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));release(f);assert(taxi_ordinary_loading_pending_poll(&f->owner)==1);entry_publications++;
   }
   assert(!f->fade.fade.active_177b);
   /* Already sampled post-intro driving gates; no intro elapsed-time claim. */
   TaxiPauseInput inputs[5]={{.start=1},{.down=1},{.confirm=1},{.up=1},{.confirm=1}};TaxiPauseEvents event;
   TaxiAudioSettingsState audio={17,19,128,255,0};a->tracker.state.playing=99;
   for(unsigned j=0;j<5;j++){
    assert(!taxi_pause_menu_step(&f->pause,&inputs[j],&event));
    if(event.pause_audio){assert(!taxi_music_playback(&a->tracker.state.playing,TAXI_MUSIC_PAUSE));assert(!taxi_audio_settings_disable_effects(&audio));}
   }
   assert(event.fade_out&&f->pause.outer_state==1&&f->pause.frozen&&!f->pause.exit_flag&&!a->tracker.state.playing&&!audio.effects_enabled&&audio.saved_effects_enabled==255);
   TaxiSoundAsset sound={0};TaxiAudioChannel channels[4];memset(channels,0xa5,sizeof channels);
   for(unsigned j=0;j<4;j++){channels[j].sound=&sound;channels[j].stamp=55+j;channels[j].volume=(uint8_t)(13+j);}
   uint32_t handles[10]={1,1,0,4,2,3,0,1,4,3};memcpy(a->core.audio.handles,handles,sizeof handles);TaxiGameAudioController retained_audio=a->core.audio;
   uint16_t rate=16000;uint8_t exit=165;
   TaxiNormalRetryOwned retry={.pause=&f->pause,.loading=&f->owner,.session=&n->session,.scene=&n->scene,.sounds=&a->core.audio,.channels=channels,.audio=&audio,.tracker=&a->tracker,.output_rate=&rate,.override_1708=&f->state.gate.override_1708,.exit_5f1a=&exit};
   TaxiDriveWorld car=a->core.car;TaxiPassengerState actors[128];memcpy(actors,n->scene.customers,sizeof actors);uint32_t rng=n->scene.random_state;
   TaxiNormalRetryOwned invalid=retry;invalid.channels=NULL;TaxiPaletteFadeStart old_fade=f->fade;assert(taxi_normal_retry_owned_fade(&invalid,&event)==-1&&!memcmp(&old_fade,&f->fade,sizeof old_fade));
   a->core.audio.handles[9]=5;invalid=retry;assert(taxi_normal_retry_owned_fade(&invalid,&event)==-1&&!memcmp(&old_fade,&f->fade,sizeof old_fade));memcpy(a->core.audio.handles,handles,sizeof handles);
   assert(!taxi_normal_retry_owned_fade(&retry,&event)&&retry.phase==1&&f->fade.masks.mode==0&&f->fade.masks.cap==32&&f->fade.masks.background_mask==0x201&&f->fade.masks.object_mask==0x201);
   assert(n->session.outer_1ea8==1&&n->scene.session.outer_state_1ea8==1&&n->session.effect_5f20[0]==1&&f->service.frozen_5f20==1&&!exit);
   TaxiNormalRetryOwned retained_retry=retry;assert(taxi_normal_retry_owned_fade(&retry,&event)==-1&&!memcmp(&retry,&retained_retry,sizeof retry));
   unsigned fade_pubs=0;
   while(f->fade.fade.active_177b&&fade_pubs<128){
    assert(!taxi_normal_retry_owned_transfer(&retry)&&!retry.stops);
    assert(!taxi_ordinary_loading_pending_prepare_present(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));
    TaxiLoadingPublishedFrame published=f->published;
    for(unsigned k=0;k<8;k++){assert(!taxi_normal_retry_owned_transfer(&retry)&&!taxi_ordinary_loading_pending_poll(&f->owner)&&!memcmp(&published,&f->published,sizeof published));}
    release(f);assert(taxi_ordinary_loading_pending_poll(&f->owner)==1);fade_pubs++;
    assert(!memcmp(&car,&a->core.car,sizeof car)&&!memcmp(actors,n->scene.customers,sizeof actors)&&rng==n->scene.random_state&&!memcmp(&retained_audio,&a->core.audio,sizeof retained_audio)&&!audio.effects_enabled&&!a->tracker.state.playing);
   }
   assert(fade_pubs&&fade_pubs<128&&!f->fade.fade.active_177b&&f->fade.fade.endpoint_count_1774==2);
   /* Even an inactive fade cannot bypass an unfinished presentation. */
   f->owner.phase=TAXI_LOADING_WAITING;assert(!taxi_normal_retry_owned_transfer(&retry)&&!retry.stops);f->owner.phase=TAXI_LOADING_IDLE;
   assert(taxi_normal_retry_owned_transfer(&retry)==2&&retry.phase==2&&retry.parent.phase==2&&retry.stops==8&&retry.requests.count==5);
   const uint32_t calls[5]={0x08010aac,0x08027754,0x0802771c,0x08027730,0x0800ac70};assert(!memcmp(retry.requests.calls,calls,sizeof calls));
   assert(audio.effects_enabled==255&&audio.saved_effects_enabled==255&&audio.music_volume==17&&audio.effects_volume==19&&audio.music_enabled==128&&!a->tracker.state.playing&&!memcmp(&retained_audio,&a->core.audio,sizeof retained_audio));
   for(unsigned j=0;j<4;j++)assert(!channels[j].sound&&!channels[j].phase&&!channels[j].step&&!channels[j].flags4&&!channels[j].flags5&&!channels[j].last&&channels[j].rate==16000&&channels[j].stamp==55+j&&channels[j].volume==13+j);
   assert(n->session.outer_1ea8==1&&n->scene.session.outer_state_1ea8==1&&f->pause.frozen==1&&n->session.effect_5f20[0]==1&&rng==n->scene.random_state&&!memcmp(&car,&a->core.car,sizeof car));
   retained_retry=retry;TaxiAudioChannel after[4];memcpy(after,channels,sizeof after);for(unsigned j=0;j<8;j++)assert(taxi_normal_retry_owned_transfer(&retry)==2&&!memcmp(&retry,&retained_retry,sizeof retry)&&!memcmp(after,channels,sizeof after));
   uint32_t expected_rng=rng;taxi_random_range(&expected_rng,0,65535);taxi_random_range(&expected_rng,20,60);
   a->begin_generation=f->published.generation;TaxiNormalRestartFlow s={.graphics={.retained_sl=1,.context_base=0x02002634,.city=(uint8_t)city}};finish_restart(a,&s);
   assert(n->scene.random_state==expected_rng&&!n->session.outer_1ea8&&!n->scene.session.outer_state_1ea8&&!f->pause.frozen&&!n->session.effect_5f20[0]&&a->tracker.state.playing);
   expected(n,argv[3],city);snapshot(a,0);assert(a->core.audio.handles[5]==3);for(unsigned j=0;j<10;j++)if(j!=5)assert(!a->core.audio.handles[j]);
   assert(taxi_normal_retry_owned_transfer(&retry)==2&&!memcmp(&retry,&retained_retry,sizeof retry));
   printf("PASS native Retry Yes city%u run%u: exact201fade/%upublications, ordered8sound stops, frozen/outer retained to terminalAC70 then full native restart READY, original actor/pool/contact/cache bytes, sharedRNG and slot5 retained\n",city+1,run,fade_pubs);fflush(stdout);
  }
  cleanup(&a->core.n);
 }
 taxi_graphics_package_release(&package);free(a);free(rom);puts("PASS native Retry handoff and complete restart integration; no live menu/GPU/audio-event equivalence claim");return 0;
}
