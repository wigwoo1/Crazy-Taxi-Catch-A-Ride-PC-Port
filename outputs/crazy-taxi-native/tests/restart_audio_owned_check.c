#define TAXI_NORMAL_RESTART_TEST_ENTRY taxi_saved_counter_fixture_main
#include "normal_restart_native_check.c"
#include "normal_retry_owned.h"
#include "frame_audio_owned.h"
#include "music_playback.h"
typedef struct {
 Full *full;TaxiFrameAudioOwners owners;TaxiAudioFrame audio;TaxiAudioAsset effects;TaxiAudioChannel channels[4];TaxiMusicContext context;
 uint8_t pitch[1536],output[2][352];int16_t mix[352];TaxiAudioOutputBuffer buffers[2];
 FILE *rng_log;unsigned city,run,events,dispatches,draws,reference_draws;uint32_t reference_rng;uint64_t pcm_hash,nonzero;
} Audio;
static void rng_row(Audio *a,unsigned kind,int low,int high,uint32_t before,uint32_t after,uint32_t result){
 assert(fprintf(a->rng_log,"%u %u %u %d %d %u %u %u %u\n",a->city,a->run,kind,low,high,before,after,result,a->events)>0);
}
static uint32_t real_random(void *context){
 Audio *a=context;uint32_t *seed=&a->full->core.n.scene.random_state,before=*seed;uint32_t value=(uint32_t)taxi_random_range(seed,0,65535);a->draws++;rng_row(a,2,0,65535,before,*seed,value);return value;
}
static uint32_t reference_random(void *context){Audio *a=context;a->reference_draws++;return (uint32_t)taxi_random_range(&a->reference_rng,0,65535);}
static void audio_tick(void *context,uint32_t counter){
 Audio *a=context;Native *n=&a->full->core.n;Fixture *f=&n->f;assert(counter==f->host.counter&&a->owners.event==&f->event);
 a->audio.ended=n->scene.session.ended_1697;a->audio.outer=n->scene.session.outer_state_1ea8;
 f->event.settings_1eb0=n->session.settings[0];
 TaxiAudioFrame expected=a->audio;TaxiMusicTracker tracker=a->full->tracker;TaxiAudioChannel channels[4];memcpy(channels,a->channels,sizeof channels);int16_t mix[352];memcpy(mix,a->mix,sizeof mix);uint8_t output[2][352];memcpy(output,a->output,sizeof output);TaxiAudioOutputBuffer buffers[2]={{output[0],352},{output[1],352}};
 a->reference_rng=n->scene.random_state;a->reference_draws=a->draws;TaxiMusicContext music=a->context;music.random=reference_random;int dispatched=0;
 assert(!taxi_audio_frame_step(&expected,&tracker,&a->full->module,a->pitch,&music,channels,&a->effects,mix,352,buffers,&dispatched));
 TaxiFrameEvent event=f->event;TaxiFrameWait wait=f->wait;TaxiMusicTracker before_tracker=a->full->tracker;TaxiAudioFrame before_audio=a->audio;uint32_t before_rng=n->scene.random_state;
 for(unsigned bad=0;bad<2;bad++){
  uint32_t wrong=event.counter_3520+(bad?2:0);assert(taxi_frame_audio_deliver_after_counter(&a->owners,&f->wait,wrong)==-1);
  assert(!memcmp(&event,&f->event,sizeof event)&&!memcmp(&wait,&f->wait,sizeof wait)&&!memcmp(&before_tracker,&a->full->tracker,sizeof before_tracker)&&!memcmp(&before_audio,&a->audio,sizeof before_audio)&&before_rng==n->scene.random_state);
 }
 assert(!taxi_frame_audio_deliver_after_counter(&a->owners,&f->wait,counter));
 assert(!memcmp(&expected,&a->audio,sizeof expected)&&!memcmp(&tracker,&a->full->tracker,sizeof tracker)&&!memcmp(channels,a->channels,sizeof channels)&&!memcmp(mix,a->mix,sizeof mix)&&!memcmp(output,a->output,sizeof output)&&dispatched==a->owners.dispatched&&a->reference_rng==n->scene.random_state&&a->reference_draws==a->draws);
 uint8_t settings=event.preserve_1ea7?event.settings_1eb0:(uint8_t)((event.settings_1eb0&0xe7)|8);assert(f->event.counter_3520==counter&&f->event.acknowledge==1&&f->wait.handshake==1&&f->event.settings_1eb0==settings&&f->event.scanline_4308==event.scanline_4308);
 n->session.settings[0]=n->scene.session.settings.bytes[0]=settings;a->events++;a->dispatches+=(unsigned)dispatched;f->events++;f->audio++;
 unsigned bank=a->audio.bank^1u;for(unsigned j=0;j<352;j++){a->pcm_hash^=a->output[bank][j];a->pcm_hash*=UINT64_C(1099511628211);a->nonzero+=a->output[bank][j]!=0;}
}
static void audio_release(Audio *a,uint64_t ns){Fixture *f=&a->full->core.n.f;uint32_t ticks;assert(!taxi_host_time_advance_serviced(&f->host,ns,audio_tick,a,&ticks));}
static int audio_call(void *context,TaxiNormalRestartFlow *s,uint32_t target,const uint32_t args[7],uint32_t *value){
 Audio *a=context;
 if(s->phase==TAXI_RESTART_SETUP&&target==0x080227ac&&a->full->core.n.setup.phase==TAXI_PASSENGER_SETUP_IDLE){
  char name[1024];assert(snprintf(name,sizeof name,"%s/settings-city%u-run%u.bin",a->full->folder,a->city+1,a->run)>0);FILE *file=fopen(name,"wb");assert(file&&fwrite(a->full->core.n.session.settings,1,512,file)==512&&!fclose(file));
 }
 uint32_t before=a->full->core.n.scene.random_state;int rc=full_call(a->full,s,target,args,value);uint32_t after=a->full->core.n.scene.random_state;
 if(target==0x0801e72c)rng_row(a,0,0,65535,before,after,*value);
 else if(s->phase==TAXI_RESTART_SETUP&&target==0x08014a58)rng_row(a,1,20,60,before,after,UINT32_MAX);
 else assert(before==after);
 return rc;
}
static void audio_cancel(void *context,uint32_t target){Audio *a=context;full_cancel(a->full,target);}
static void bind_audio(Audio *a){Fixture *f=&a->full->core.n.f;f->bridge.remainder=audio_call;f->bridge.context=a;f->bridge.cancel_delegated=audio_cancel;}
static void complete_audio_restart(Audio *a){
 Full *p=a->full;Fixture *f=&p->core.n.f;TaxiNormalRestartFlow s={.graphics={.retained_sl=1,.context_base=0x02002634,.city=(uint8_t)a->city}};int rc=taxi_normal_restart_flow_begin(&s,taxi_normal_restart_presentation_owned,&f->bridge);assert(!rc);unsigned polls=0;
 for(unsigned j=0;!rc&&j<4096;j++){
  rc=taxi_normal_restart_flow_step(&s,taxi_normal_restart_presentation_owned,&f->bridge);if(rc<0)fprintf(stderr,"audio join failure city%u run%u phase%u graph%u entry%u setup%u settings%u HUDsettings%u counter%u events%u rng%u\n",a->city,a->run,s.phase,p->visual.graphics_index,p->visual.entry_index,p->core.owner.index,p->core.n.session.settings[0],p->hud_input.settings,f->host.counter,a->events,p->core.n.scene.random_state);assert(rc>=0);
  if(f->bridge.active_target||f->bridge.delegated_target){
   TaxiMusicTracker tracker=p->tracker;TaxiAudioFrame audio=a->audio;uint32_t seed=p->core.n.scene.random_state;unsigned events=a->events;TaxiLoadingPublishedFrame pub=f->published;
   for(unsigned k=0;k<8;k++){assert(!taxi_normal_restart_flow_step(&s,taxi_normal_restart_presentation_owned,&f->bridge));polls++;}
   assert(!memcmp(&tracker,&p->tracker,sizeof tracker)&&!memcmp(&audio,&a->audio,sizeof audio)&&seed==p->core.n.scene.random_state&&events==a->events&&!memcmp(&pub,&f->published,sizeof pub));audio_release(a,20000000);
  }
 }
 assert(rc==1&&s.phase==TAXI_RESTART_READY&&p->loaded&&p->tracker.state.playing&&p->core.owner.phase==3&&p->visual.graphics_index==15&&p->visual.entry_index==8&&p->guidance.count==(a->city?0u:66u));
 expected(&p->core.n,p->folder,a->city);snapshot(p,0);assert(p->core.n.scene.random_state==a->reference_rng);
 printf("PASS real native audio restart city%u run%u: %uactualevents/%udispatches/%umusic RNG, %uempty polls, fullnative READY and original actor/pool bytes\n",a->city+1,a->run,a->events,a->dispatches,a->draws,polls);fflush(stdout);
}
static void finish_entry_fade(Audio *a){Fixture *f=&a->full->core.n.f;unsigned count=0;while(f->fade.fade.active_177b&&count++<128){assert(!taxi_ordinary_loading_pending_prepare_present(&f->owner)&&!taxi_ordinary_loading_pending_begin(&f->owner));audio_release(a,20000000);assert(taxi_ordinary_loading_pending_poll(&f->owner)==1);}assert(!f->fade.fade.active_177b);}
int main(int argc,char **argv){
 assert(argc==4||argc==5);uint8_t *rom=malloc(8388608);assert(rom);FILE *f=fopen(argv[1],"rb");assert(f&&fread(rom,1,8388608,f)==8388608&&fgetc(f)==EOF&&!fclose(f));Full *p=malloc(sizeof *p);Audio *a=calloc(1,sizeof *a);assert(p&&a);a->full=p;
 char name[1024];assert(snprintf(name,sizeof name,"%s/rng-events.txt",argv[3])>0);a->rng_log=fopen(name,"wb");assert(a->rng_log);assert(snprintf(name,sizeof name,"%s/assets/audio",argv[2])>0);assert(!taxi_audio_asset_load(&a->effects,name));raw(argv[2],"assets/music/pitch-table.bin",a->pitch,1536,1);TaxiGraphicsPackage package={0};const uint8_t *graphics=rom;if(argc==5){assert(!taxi_graphics_package_load(&package,argv[4]));graphics=package.bytes;}
 for(unsigned city=0;city<2;city++){
  memset(p,0,sizeof *p);core_initialize(&p->core,graphics,argv[2],city);assert(snprintf(name,sizeof name,"assets/scene-descriptors/city%u.bin",city+1)>0);raw(argv[2],name,p->descriptor,104,1);uint8_t trig[16388];raw(argv[2],"assets/driving.ctr",trig,sizeof trig,0);memcpy(p->sine,trig+4,8192);memcpy(p->cosine,trig+8196,8192);memset(&p->guidance,0xa5,sizeof p->guidance);
  a->city=city;a->events=a->draws=a->dispatches=0;a->nonzero=0;a->pcm_hash=UINT64_C(1469598103934665603);memset(a->channels,0,sizeof a->channels);memset(a->output,0,sizeof a->output);memset(a->mix,0,sizeof a->mix);
  assert(taxi_audio_channel_start(a->channels,a->effects.sounds+41,21024,0,0)==0&&taxi_audio_channel_start(a->channels,a->effects.sounds+9,21024,1,1)==1);
  a->audio=(TaxiAudioFrame){.rate=21024,.processing=1,.music_enabled=1,.effects_enabled=1,.effects_active=1,.music_volume=64,.effects_volume=64,.fade=256};a->context=(TaxiMusicContext){.main_mode=0,.random=real_random,.context=a};a->buffers[0]=(TaxiAudioOutputBuffer){a->output[0],352};a->buffers[1]=(TaxiAudioOutputBuffer){a->output[1],352};
  a->owners=(TaxiFrameAudioOwners){.event=&p->core.n.f.event,.audio=&a->audio,.tracker=&p->tracker,.music=&p->module,.pitch=a->pitch,.music_context=&a->context,.channels=a->channels,.effects=&a->effects,.mix=a->mix,.capacity=352,.buffers=a->buffers};
  for(unsigned run=0;run<2;run++){
   a->run=run;
   if(run){
    finish_entry_fade(a);
    /* Explicit diagnostic near-module-end start, actual original data/order.
       Exercise real music RNG on the SAME game seed before warm Retry. */
    assert(taxi_music_tracker_start(&p->tracker,&p->module,(uint8_t)(p->module.bytes[1]-1)));unsigned draws=a->draws,target=a->events+600;
    while(a->events<target)audio_release(a,16666667);
    assert(a->draws>draws);
    Fixture *v=&p->core.n.f;TaxiPauseInput inputs[5]={{.start=1},{.down=1},{.confirm=1},{.up=1},{.confirm=1}};TaxiPauseEvents event;TaxiAudioSettingsState settings={64,64,1,1,1};
    for(unsigned k=0;k<5;k++){assert(!taxi_pause_menu_step(&v->pause,&inputs[k],&event));if(event.pause_audio){assert(!taxi_music_playback(&p->tracker.state.playing,TAXI_MUSIC_PAUSE)&&!taxi_audio_settings_disable_effects(&settings));a->audio.effects_enabled=settings.effects_enabled;}}
    assert(event.fade_out&&v->pause.outer_state==1);uint16_t rate=21024;uint8_t exit=0;for(unsigned k=0;k<10;k++)p->core.audio.handles[k]=(k%2)+1;
    TaxiNormalRetryOwned retry={.pause=&v->pause,.loading=&v->owner,.session=&p->core.n.session,.scene=&p->core.n.scene,.sounds=&p->core.audio,.channels=a->channels,.audio=&settings,.tracker=&p->tracker,.output_rate=&rate,.override_1708=&v->state.gate.override_1708,.exit_5f1a=&exit};assert(!taxi_normal_retry_owned_fade(&retry,&event));finish_entry_fade(a);assert(taxi_normal_retry_owned_transfer(&retry)==2&&retry.stops==10&&!p->tracker.state.playing&&settings.effects_enabled==1);a->audio.effects_enabled=settings.effects_enabled;
   }
   bind_full(p,rom,city,run,argv[3]);bind_audio(a);complete_audio_restart(a);
  }
  assert(a->events>800&&a->dispatches>500&&a->draws&&a->nonzero);printf("PASS real audio city%u: %uevents, %umodule-end draws, %llu nonzero samples, PCMhash%016llx; counter wrap/settings/wait aliases and per-event tracker/channels/mix/output/RNG exact native composition\n",city+1,a->events,a->draws,(unsigned long long)a->nonzero,(unsigned long long)a->pcm_hash);fflush(stdout);cleanup(&p->core.n);
 }
 assert(!fclose(a->rng_log));taxi_audio_asset_free(&a->effects);taxi_graphics_package_release(&package);free(a);free(p);free(rom);puts("PASS complete restart and Retry with actual native audio/events; host device/fulloriginal IRQ timing not claimed");return 0;
}
