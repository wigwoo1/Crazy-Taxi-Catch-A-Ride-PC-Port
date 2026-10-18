#ifdef NDEBUG
#undef NDEBUG
#endif
#include "save_load_owned.h"
#include "save_block.h"
#include "frame_audio_owned.h"
#include "startup_save_owned.h"
#include "save_audio_host.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {TaxiFrameEvent *event;TaxiAudioFrame *audio;TaxiSaveLoad *save;uint32_t expected_frame_callback;unsigned events,phases,timer_active_events,frame_callback_calls;int resetting;unsigned fail_phase;TaxiFrameAudioOwners frame;TaxiSaveAudioHost native_host;} Host;
static int event_call(void *ctx,TaxiFrameEvent *e,uint32_t target,uint32_t argument){
 Host *h=ctx;(void)e;(void)argument;
 if(target==0x08026888){assert(!h->audio->music_enabled&&!h->audio->effects_enabled);if(h->resetting)assert(!h->audio->processing&&!h->audio->music_volume&&!h->audio->effects_volume);return 0;}
 return target==0x0803244c||target==0x080161f4?0:-1;
}
static int registered(void *context,uint32_t identity){Host *h=context;assert(h&&identity==h->expected_frame_callback&&h->event->callback_352c==h->expected_frame_callback);assert(h->save->interrupt_callback!=identity);h->frame_callback_calls++;return 0;}
static int delivery(void *ctx,TaxiFrameWait *w){
 Host *h=ctx;assert(h->event->preserve_1ea7==1&&h->event->callback_352c==h->expected_frame_callback);
 assert(h->save->interrupt_callback==0x080161d9||h->save->interrupt_callback==0x08032081);
 int timer_active=h->save->interrupt_callback==0x08032081;
 if(event_call(h,h->event,0x08026888,0)||taxi_save_audio_host_deliver(&h->native_host,w))return -1;
 assert(h->event->callback_352c==h->expected_frame_callback);h->events++;h->timer_active_events+=(unsigned)timer_active;return 0;
}
static int platform(void *ctx,TaxiAudioBootPhase p,uint32_t rate,uint32_t count){
 Host *h=ctx;assert(h->event->callback_352c==h->expected_frame_callback);assert(rate==21024&&count==352);assert(p==TAXI_AUDIO_BOOT_STOP+h->phases*2-(h->phases==2));h->phases++;if((unsigned)p==h->fail_phase)return -1;h->resetting=p==TAXI_AUDIO_BOOT_RESET_PREPARE;return taxi_save_audio_host_platform(&h->native_host,p,rate,count);
}
static void check(unsigned enabled,unsigned corrupt,unsigned fail_phase,uint32_t frame_callback){
 uint8_t settings[512],rows[800],table[80]={0},storage[704];int16_t mix[352];memset(storage,99,sizeof storage);memset(mix,99,sizeof mix);taxi_save_block_defaults(settings,rows,table);
 TaxiSaveFileDevice device={0};memcpy(device.bytes,settings,512);if(corrupt)memset(device.bytes,255,512);
 TaxiAudioFrame audio={0};audio.processing=1;TaxiMusicTracker tracker={0};tracker.state.playing=1;TaxiAudioChannel channels[4]={0};TaxiSoundAsset sound={0};channels[0].sound=&sound;channels[0].flags4=1;
 TaxiAudioOutputBuffer buffers[2]={0};TaxiAudioBootstrap boot={0};boot.audio=&audio;boot.tracker=&tracker;boot.channels=channels;boot.configuration=(TaxiAudioConfiguration){21024,352,1};boot.buffers=buffers;boot.output_storage=storage;boot.output_capacity=704;boot.mix=mix;boot.mix_capacity=352;
 TaxiFrameEvent event={0};assert(!taxi_frame_event_register(&event,frame_callback));TaxiFrameWait wait={(uint16_t)enabled,0};Host host={.event=&event,.audio=&audio,.fail_phase=fail_phase,.expected_frame_callback=event.callback_352c};TaxiMusicAsset music={0};TaxiAudioAsset effects={0};uint8_t pitch[1536]={0};TaxiMusicContext music_context={0};audio.rate=21024;buffers[0]=(TaxiAudioOutputBuffer){storage,352};buffers[1]=(TaxiAudioOutputBuffer){storage+352,352};host.frame=(TaxiFrameAudioOwners){.event=&event,.audio=&audio,.tracker=&tracker,.music=&music,.pitch=pitch,.music_context=&music_context,.channels=channels,.effects=&effects,.mix=mix,.capacity=352,.buffers=buffers,.scanline=160,.registered=registered,.context=&host};TaxiAudioHost *output=NULL;assert(!taxi_audio_host_open(&output,21024));host.native_host=(TaxiSaveAudioHost){output,&host.frame,21024};TaxiSaveLoadOwners owners={&device,&boot,&wait,&event,delivery,platform,&host};TaxiSaveLoad save={0};save.settings=settings;save.runtime_rows=rows;save.first_ten=table;save.current_clock=15;save.music_enabled=save.effects_enabled=1;save.interrupt_callback=0x080161d9;host.save=&save;
  TaxiStartupInit init={0};TaxiStartupInitContinuation continuation={0};TaxiInput input={0};uint8_t music_volume=0,effects_volume=0,language=0;
 TaxiStartupInitOwners startup_owners={settings,rows,table,&input,&music_volume,&effects_volume,&language};
 assert(taxi_startup_init_owned_begin(&continuation,&init,&startup_owners)==TAXI_STARTUP_INIT_SCREEN);
 assert(taxi_startup_init_owned_resume(&continuation)==TAXI_STARTUP_INIT_SAVE);
 input.pressed_mask=0x155;input.raw_keys=0x2aa;input.pressed[0]=1;
 int rc=taxi_startup_save_owned(&continuation,&save,&owners);assert(event.callback_352c==host.expected_frame_callback&&host.frame_callback_calls==host.events);if(fail_phase){assert(save.interrupt_callback==(fail_phase==TAXI_AUDIO_BOOT_STOP?0x080161d9:0x08032081));assert(host.timer_active_events==(fail_phase==TAXI_AUDIO_BOOT_STOP?0u:fail_phase==TAXI_AUDIO_BOOT_RESET_PREPARE?5u:10u));assert(rc==-1);assert(continuation.pending==TAXI_STARTUP_INIT_SAVE&&!init.initialized_1ea4&&!init.dispatcher_3878);assert(save.preserve_settings&&event.preserve_1ea7);assert(!taxi_audio_host_close(&output));if(fail_phase!=TAXI_AUDIO_BOOT_STOP){assert(!audio.processing&&!audio.music_volume&&!audio.effects_volume);assert(!audio.music_enabled&&!audio.effects_enabled);}return;}assert(!rc);assert(continuation.pending==TAXI_STARTUP_INIT_DONE&&init.initialized_1ea4==1&&init.dispatcher_3878==0x0801e275);assert(music_volume==settings[1]&&effects_volume==settings[2]&&language==save.language);assert(input.pressed_mask==0x155&&input.raw_keys==0x2aa&&input.pressed[0]==1);assert(taxi_startup_save_owned(&continuation,&save,&owners)==-1);assert(host.events==(enabled==1?15u:0u)&&host.phases==3);assert(save.current_clock==(enabled==1?30u:15u)&&save.previous_clock==save.current_clock&&event.counter_3520==save.current_clock);assert(!save.preserve_settings&&!event.preserve_1ea7);assert(settings[0]==event.settings_1eb0);assert(save.interrupt_callback==0x080161d9);assert(host.timer_active_events==(enabled==1?(corrupt?5u:10u):0u));assert(audio.music_enabled&&audio.effects_enabled&&audio.processing);assert(audio.music_volume==settings[1]&&audio.effects_volume==settings[2]);assert(!tracker.state.playing&&!channels[0].sound);assert(device.reads==64&&device.verifications==64);assert(buffers[1].samples==storage+352);for(unsigned i=0;i<704;i++)assert(!storage[i]);for(unsigned i=0;i<352;i++)assert(!mix[i]);uint8_t silent[352]={0};assert(!taxi_audio_host_write(output,silent,sizeof silent));assert(!taxi_audio_host_close(&output));
}
int main(void){
 const uint32_t callbacks[]={0,0x08017001};
 for(unsigned c=0;c<2;c++){
  for(unsigned gate=0;gate<3;gate++)for(unsigned corrupt=0;corrupt<2;corrupt++)check(gate,corrupt,0,callbacks[c]);
  check(1,0,TAXI_AUDIO_BOOT_STOP,callbacks[c]);check(1,0,TAXI_AUDIO_BOOT_RESET_PREPARE,callbacks[c]);check(1,0,TAXI_AUDIO_BOOT_RESET_ENABLE,callbacks[c]);
 }
 puts("PASS native startup/save/audio callback isolation: twelve successful and six suspended paths; frame352C retained independently of active/restored timer3548");
 return 0;
}