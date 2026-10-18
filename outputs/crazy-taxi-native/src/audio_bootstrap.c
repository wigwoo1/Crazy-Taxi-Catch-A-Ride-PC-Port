#include "audio_bootstrap.h"
#include <string.h>
typedef struct {TaxiAudioBootstrap *s;TaxiFrameWait *wait;TaxiFrameWaitService service;TaxiAudioBootPlatform platform;void *context;} BootstrapContext;
static int reset_platform(void *pointer,TaxiAudioResetPhase phase,uint32_t rate,uint32_t count){
 BootstrapContext *c=pointer;
 return c->platform(c->context,phase==TAXI_AUDIO_RESET_PREPARE?TAXI_AUDIO_BOOT_RESET_PREPARE:TAXI_AUDIO_BOOT_RESET_ENABLE,rate,count);
}
static int bootstrap_wait_service(void *pointer,TaxiFrameWait *wait){
 BootstrapContext *c=pointer;return c->service?c->service(c->context,wait):-1;
}
static int configuration_call(void *pointer,TaxiAudioConfiguration *configuration,uint32_t target){
 BootstrapContext *c=pointer;if(target!=0x08026704)return -1;
 c->s->audio->rate=configuration->rate_7564;
 return taxi_audio_reset(c->s->channels,configuration->rate_7564,configuration->count_7568,c->wait,bootstrap_wait_service,reset_platform,c);
}
int taxi_audio_bootstrap(TaxiAudioBootstrap *s,uint32_t selector,uint8_t music_volume,uint8_t effects_volume,
 TaxiFrameWait *wait,TaxiFrameWaitService service,TaxiAudioBootPlatform platform,void *context){
 if(!s||!s->audio||!s->tracker||!s->channels||!s->buffers||!s->output_storage||s->output_capacity<704||!s->mix||s->mix_capacity<352||!wait||!platform)return -1;
 s->audio->processing=0;s->configuration.processing_7560=0;
 if(platform(context,TAXI_AUDIO_BOOT_STOP,0,0))return -1;
 s->audio->effects_volume=s->audio->music_volume=0;
 memset(s->output_storage,0,704);memset(s->mix,0,704);s->audio->bank=0;
 BootstrapContext c={s,wait,service,platform,context};
 if(taxi_audio_configuration_select(&s->configuration,selector,configuration_call,&c))return -1;
 if(!s->initialized_7561&&platform(context,TAXI_AUDIO_BOOT_BIND,0,0))return -1;
 s->audio->music_volume=music_volume;s->audio->effects_volume=effects_volume;
 s->tracker->state.playing=0;s->audio->effects_active=1;
 s->tracker->state.state[0]=0;s->audio->divider=0;s->tracker->state.state[2]=0;
 size_t count=s->configuration.count_7568;
 s->buffers[0]=(TaxiAudioOutputBuffer){s->output_storage,count};
 s->buffers[1]=(TaxiAudioOutputBuffer){s->output_storage+count,count};
 s->audio->processing=1;s->initialized_7561=1;return 0;
}

int taxi_audio_reinitialize(TaxiAudioBootstrap *s,uint8_t music_volume,uint8_t effects_volume,
 TaxiFrameWait *wait,TaxiFrameWaitService service,TaxiAudioBootPlatform platform,void *context){
 if(!s||!s->audio||!s->tracker||!s->channels||!s->buffers||!s->output_storage||s->output_capacity<704||!s->mix||s->mix_capacity<352||!wait||!platform)return -1;
 uint32_t rate=s->configuration.rate_7564,count=s->configuration.count_7568;
 if(!rate||!count||count>352||count%4)return -1;
 s->audio->processing=0;s->configuration.processing_7560=0;s->audio->music_volume=s->audio->effects_volume=0;
 memset(s->output_storage,0,704);memset(s->mix,0,704);s->audio->bank=0;
 BootstrapContext c={s,wait,service,platform,context};
 if(taxi_audio_reset(s->channels,rate,count,wait,bootstrap_wait_service,reset_platform,&c))return -1;
 s->audio->rate=rate;s->audio->music_volume=music_volume;s->audio->effects_volume=effects_volume;
 s->tracker->state.playing=0;s->audio->effects_active=1;s->tracker->state.state[0]=0;s->audio->divider=0;s->tracker->state.state[2]=0;
 s->buffers[0]=(TaxiAudioOutputBuffer){s->output_storage,count};s->buffers[1]=(TaxiAudioOutputBuffer){s->output_storage+count,count};
 s->audio->processing=1;s->configuration.processing_7560=1;return 0;
}
