#include "../src/music_tracker.h"
#include "../src/random.h"
#include "../src/audio_frame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t rng;static unsigned draws;
static uint32_t random_value(void *p){(void)p;draws++;return (uint32_t)taxi_random_range(&rng,0,65535);}
#include "../src/audio_reset.h"
#include "../src/frame_event.h"
typedef struct {
    TaxiAudioFrame *audio;TaxiMusicTracker *tracker;TaxiMusicAsset *music;
    const uint8_t *pitch;TaxiMusicContext *music_context;TaxiAudioChannel *channels;
    TaxiAudioAsset *effects;int16_t *mix;TaxiAudioOutputBuffer *buffers;
    TaxiFrameWait wait;TaxiFrameEvent event;unsigned phases;int dispatch;
} Fixture;
static int event_call(void *context,TaxiFrameEvent *event,uint32_t target,uint32_t argument){
    (void)event;(void)argument;Fixture *f=context;
    if(target==0x08026888)return taxi_audio_frame_step(f->audio,f->tracker,f->music,f->pitch,f->music_context,f->channels,f->effects,f->mix,352,f->buffers,&f->dispatch);
    if(target==0x080161f4){f->wait.handshake=1;return 0;}
    return -1;
}
static int wait_service(void *context,TaxiFrameWait *wait){
    (void)wait;Fixture *f=context;
    return taxi_frame_event(&f->event,159,event_call,f);
}
static int platform(void *context,TaxiAudioResetPhase phase,uint32_t rate,uint32_t count){
    Fixture *f=context;if(rate!=21024||count!=352)return -1;
    if(phase==TAXI_AUDIO_RESET_PREPARE){
        if(f->phases)return -1;
        for(unsigned i=0;i<4;i++)if(f->channels[i].sound||f->channels[i].flags4||f->channels[i].flags5)return -1;
        f->phases=1;return 0;
    }
    if(phase!=TAXI_AUDIO_RESET_ENABLE||f->phases!=1)return -1;
    f->phases=12;return 0;
}
int main(void){uint8_t *b=malloc(181241),pitch[1536];FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);f=fopen("outputs/crazy-taxi-native/assets/music/pitch-table.bin","rb");if(!f||fread(pitch,1,1536,f)!=1536)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;
TaxiAudioAsset effects={0};if(taxi_audio_asset_load(&effects,"outputs/crazy-taxi-native/assets/audio"))return 1;unsigned order,seed,mode,enabled,handshake,counter,keep,setting,divider,bank,warm;
while(scanf("%u %u %u %u %u %u %u %u %u %u %u",&order,&seed,&mode,&enabled,&handshake,&counter,&keep,&setting,&divider,&bank,&warm)==11){TaxiMusicTracker t={0};memset(t.state.state,165,12);t.state.state[0]=1;for(unsigned k=0;k<16;k++)memset(t.voice[k].bytes,165,32);if(!taxi_music_tracker_start(&t,&a,(uint8_t)order))return 1;rng=seed;draws=0;TaxiMusicContext c={.main_mode=(uint8_t)mode,.random=random_value};
TaxiAudioChannel channels[4]={0};channels[0]=(TaxiAudioChannel){.stamp=123,.phase=0,.step=128,.rate=10512,.volume=64,.last=165,.sound=effects.sounds+41};channels[2]=(TaxiAudioChannel){.stamp=456,.flags4=1,.flags5=1,.phase=0,.step=128,.rate=10512,.volume=32,.last=165,.sound=effects.sounds+9};TaxiAudioFrame audio={.rate=21024,.processing=1,.music_enabled=1,.effects_enabled=1,.effects_active=1,.music_volume=64,.effects_volume=64,.fade=256};uint8_t output[2][352]={{0}};TaxiAudioOutputBuffer buffers[2]={{output[0],352},{output[1],352}};int16_t mixed[352]={0};
audio.divider=(uint8_t)divider;audio.bank=bank;
for(unsigned i=0;i<warm;i++){int dispatched;if(taxi_audio_frame_step(&audio,&t,&a,pitch,&c,channels,&effects,mixed,352,buffers,&dispatched))return 1;}
unsigned draws_before_reset=draws;
Fixture fx={.audio=&audio,.tracker=&t,.music=&a,.pitch=pitch,.music_context=&c,.channels=channels,.effects=&effects,.mix=mixed,.buffers=buffers,.wait={(uint16_t)enabled,(uint16_t)handshake},.event={.counter_3520=counter,.preserve_1ea7=(uint8_t)keep,.settings_1eb0=(uint8_t)setting}};
if(taxi_audio_reset(channels,21024,352,&fx.wait,wait_service,platform,&fx))return 1;
int dispatch=fx.dispatch;
printf("%u %u ",audio.divider,dispatch);for(unsigned j=0;j<12;j++)printf("%02x",t.state.state[j]);printf(" %llu",(unsigned long long)t.state.pattern_offset);for(unsigned k=0;k<16;k++){putchar(' ');for(unsigned j=0;j<28;j++)printf("%02x",t.voice[k].bytes[j]);printf(" %llu",(unsigned long long)(t.voice[k].sample?(size_t)(t.voice[k].sample-b):UINT32_MAX));}printf(" %u %u %u",rng,draws,t.state.playing);for(unsigned j=0;j<352;j++)printf(" %d",mixed[j]);printf(" %u %u",audio.bank,audio.request_756c);for(unsigned k=0;k<4;k++)printf(" %u %u %u %u %u %u %u %u %u",channels[k].stamp,channels[k].rate,channels[k].phase,channels[k].step,channels[k].flags4,channels[k].flags5,channels[k].volume,channels[k].last,channels[k].sound!=0);for(unsigned k=0;k<2;k++){putchar(' ');for(unsigned j=0;j<352;j++)printf("%02x",output[k][j]);}printf(" %u %u %u %u %u %u",fx.event.counter_3520,fx.event.scanline_4308,fx.event.settings_1eb0,fx.event.acknowledge,fx.wait.handshake,fx.phases);
printf(" %u\n",draws_before_reset);
}
taxi_audio_asset_free(&effects);free(b);return 0;}
