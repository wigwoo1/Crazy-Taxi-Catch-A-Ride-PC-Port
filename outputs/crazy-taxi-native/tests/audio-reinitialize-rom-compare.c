#include "../src/music_tracker.h"
#include "../src/random.h"
#include "../src/audio_frame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static uint32_t rng;static unsigned draws;
static uint32_t random_value(void *p){(void)p;draws++;return (uint32_t)taxi_random_range(&rng,0,65535);}
#include "../src/audio_bootstrap.h"
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
static int boot_platform(void *context,TaxiAudioBootPhase phase,uint32_t rate,uint32_t count){
 (void)rate;(void)count;Fixture *f=context;f->phases=f->phases*10+(unsigned)phase;return 0;
}
int main(void){uint8_t *b=malloc(181241),pitch[1536];FILE *f=fopen("outputs/crazy-taxi-native/assets/music/original-module.bin","rb");if(!b||!f||fread(b,1,181241,f)!=181241)return 1;fclose(f);f=fopen("outputs/crazy-taxi-native/assets/music/pitch-table.bin","rb");if(!f||fread(pitch,1,1536,f)!=1536)return 1;fclose(f);TaxiMusicAsset a;if(taxi_music_asset_index(&a,b,181241))return 1;
TaxiAudioAsset effects={0};if(taxi_audio_asset_load(&effects,"outputs/crazy-taxi-native/assets/audio"))return 1;unsigned selector,enabled,initialized,old_count;
while(scanf("%u %u %u %u",&selector,&enabled,&initialized,&old_count)==4){
 TaxiMusicTracker t={0};rng=7;draws=0;TaxiMusicContext c={.main_mode=1,.random=random_value};
 TaxiAudioChannel channels[4]={0};channels[0]=(TaxiAudioChannel){.stamp=123,.phase=9876,.step=128,.rate=10512,.volume=64,.last=165,.flags4=1,.sound=effects.sounds+41};
 TaxiAudioFrame audio={.rate=21024,.processing=1,.music_volume=64,.effects_volume=64,.bank=1,.divider=6,.fade=256};
 uint8_t output[704];memset(output,165,sizeof output);int16_t mixed[352];memset(mixed,165,sizeof mixed);
 TaxiAudioOutputBuffer buffers[2]={{output,old_count},{output+352,old_count}};
 Fixture fx={.audio=&audio,.tracker=&t,.music=&a,.pitch=pitch,.music_context=&c,.channels=channels,.effects=&effects,.mix=mixed,.buffers=buffers,.wait={(uint16_t)enabled,0xa5a5},.event={.counter_3520=0xffffffff,.settings_1eb0=165}};
 TaxiAudioBootstrap boot={.audio=&audio,.tracker=&t,.channels=channels,.configuration={21024,352,1},.buffers=buffers,.output_storage=output,.output_capacity=704,.mix=mixed,.mix_capacity=352,.initialized_7561=(uint8_t)initialized};
 boot.configuration.count_7568=old_count;
 if(taxi_audio_reinitialize(&boot,37,53,&fx.wait,wait_service,boot_platform,&fx))return 1;
 (void)selector;
 unsigned bank_after_boot=audio.bank,divider_after_boot=audio.divider;

 printf("%u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u ",audio.rate,boot.configuration.count_7568,audio.processing,boot.initialized_7561,audio.bank,audio.divider,audio.music_volume,audio.effects_volume,t.state.playing,audio.effects_active,t.state.state[0],t.state.state[2],bank_after_boot,divider_after_boot,fx.event.counter_3520,fx.event.settings_1eb0,fx.wait.handshake,fx.phases,rng,draws);
 for(unsigned k=0;k<4;k++)printf("%u %u %u %u %u %u %u %u %u ",channels[k].stamp,channels[k].rate,channels[k].phase,channels[k].step,channels[k].flags4,channels[k].flags5,channels[k].volume,channels[k].last,channels[k].sound!=0);
 for(unsigned j=0;j<704;j++)printf("%02x",output[j]);putchar(' ');
 for(unsigned j=0;j<352;j++)printf("%04x",(uint16_t)mixed[j]);
 printf(" %llu %llu %llu %llu\n",(unsigned long long)(buffers[0].samples-output),(unsigned long long)buffers[0].count,(unsigned long long)(buffers[1].samples-output),(unsigned long long)buffers[1].count);
}
taxi_audio_asset_free(&effects);free(b);return 0;}
