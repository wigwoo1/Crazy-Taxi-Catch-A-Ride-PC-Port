#include "save_load_owned.h"
#include "music_playback.h"
static void publish(TaxiSaveLoadOwners *o,TaxiSaveLoad *s){
 TaxiAudioFrame *a=o->bootstrap->audio;
 a->music_enabled=s->music_enabled;a->effects_enabled=s->effects_enabled;
 a->music_volume=s->music_volume;a->effects_volume=s->effects_volume;
 o->event->preserve_1ea7=s->preserve_settings;o->event->settings_1eb0=s->settings[0];
 o->event->counter_3520=s->current_clock;
 /* Save timer callback3548 is independent of frame callback352C. */
}
static void collect(TaxiSaveLoadOwners *o,TaxiSaveLoad *s){
 s->current_clock=o->event->counter_3520;s->settings[0]=o->event->settings_1eb0;
}
typedef struct {TaxiSaveLoadOwners *owners;TaxiSaveLoad *save;} Bridge;
static int deliver(void *context,TaxiFrameWait *wait){
 Bridge *b=context;int rc=b->owners->deliver(b->owners->context,wait);
 collect(b->owners,b->save);return rc;
}
static int platform(void *context,TaxiAudioBootPhase phase,uint32_t rate,uint32_t count){
 Bridge *b=context;return b->owners->platform(b->owners->context,phase,rate,count);
}
static int service(void *context,TaxiSaveLoad *s,uint32_t target,uint32_t a,uint32_t arg,uint32_t *result){
 Bridge *b=context;TaxiSaveLoadOwners *o=b->owners;publish(o,s);
 int handled=taxi_save_file_device_call(o->device,s,target,a,arg,result);
 if(handled)return handled==1?0:-1;
 switch(target){
 case 0x0802763c:return taxi_audio_channels_stop_rate(o->bootstrap->channels,(uint16_t)o->bootstrap->configuration.rate_7564)<0?-1:0;
 case 0x08027730:return taxi_music_playback(&o->bootstrap->tracker->state.playing,TAXI_MUSIC_STOP);
 case 0x08027558:return platform(b,TAXI_AUDIO_BOOT_STOP,o->bootstrap->configuration.rate_7564,o->bootstrap->configuration.count_7568);
 case 0x08016208:return taxi_frame_wait(o->wait,a,deliver,b);
 case 0x08026630:return taxi_audio_reinitialize(o->bootstrap,s->settings[1],s->settings[2],o->wait,deliver,platform,b);
 default:return -1;
 }
}
int taxi_save_load_owned(TaxiSaveLoad *s,uint32_t slot,TaxiSaveLoadOwners *o){
 if(!s||!s->settings||!s->runtime_rows||!s->first_ten||!o||!o->device||!o->bootstrap||!o->bootstrap->audio||!o->bootstrap->tracker||!o->bootstrap->channels||!o->wait||!o->event||!o->deliver||!o->platform)return -1;
 Bridge b={o,s};int rc=taxi_save_load(s,slot,1,service,&b);
 /* Publish final preserve/domain/volume/clock state even on a service error. */
 publish(o,s);return rc;
}
