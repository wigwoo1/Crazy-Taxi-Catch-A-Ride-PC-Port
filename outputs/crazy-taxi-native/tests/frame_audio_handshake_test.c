#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/frame_audio_owned.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
typedef struct {
 TaxiFrameWait wait;TaxiFrameEvent event;TaxiFrameAudioOwners owners;
 unsigned audio,registered;int audio_error,registered_error,require_clear;
} Fixture;
static Fixture *active;
int taxi_audio_frame_step(TaxiAudioFrame *s,TaxiMusicTracker *t,const TaxiMusicAsset *m,
 const uint8_t *pitch,const TaxiMusicContext *music,TaxiAudioChannel channels[4],
 const TaxiAudioAsset *effects,int16_t *mix,size_t capacity,TaxiAudioOutputBuffer buffers[2],int *dispatched){
 (void)s;(void)t;(void)m;(void)pitch;(void)music;(void)channels;(void)effects;(void)mix;(void)capacity;(void)buffers;
 assert(active);active->audio++;assert(active->event.counter_3520==(uint32_t)(UINT32_MAX+active->audio));
 if(active->require_clear)assert(!(active->wait.handshake&1));
 *dispatched=1;return active->audio_error;
}
static int registered(void *opaque,uint32_t identity){
 Fixture *f=opaque;assert(f==active&&identity==f->event.callback_352c);
 f->registered++;assert(f->audio==f->registered);
 if(f->registered_error)return f->registered_error;
 f->wait.handshake=UINT16_MAX;return 0;
}
static void init(Fixture *f){
 memset(f,0,sizeof *f);f->wait=(TaxiFrameWait){1,UINT16_MAX};
 f->event=(TaxiFrameEvent){.counter_3520=UINT32_MAX,.callback_352c=0x080161dd,.acknowledge=UINT16_MAX};
 f->owners=(TaxiFrameAudioOwners){.event=&f->event,.scanline=160,.registered=registered,.context=f};
 active=f;
}
int main(void){
 TaxiFrameWait flag={0};
 for(unsigned value=0;value<65536;value++){flag.handshake=(uint16_t)value;assert(!taxi_frame_wait_signal(&flag)&&flag.handshake==1);}
 assert(taxi_frame_wait_signal(NULL)==-1);
 const uint32_t counts[]={0,1,7,255,256,257,UINT32_MAX};
 unsigned deliveries=0;
 for(unsigned enabled=0;enabled<3;enabled++)for(unsigned j=0;j<sizeof counts/sizeof *counts;j++){
  Fixture f;init(&f);f.wait.event_delivery_enabled=(uint16_t)enabled;f.require_clear=1;
  assert(!taxi_frame_wait(&f.wait,counts[j],taxi_frame_audio_deliver,&f.owners));
  unsigned wanted=enabled==1?(uint8_t)counts[j]:0;
  assert(f.audio==wanted&&f.registered==wanted);
  if(wanted)assert(f.wait.handshake==1&&f.event.acknowledge==1&&f.event.settings_1eb0==8&&f.event.scanline_4308==160);
  else assert(f.wait.handshake==UINT16_MAX&&f.event.acknowledge==UINT16_MAX&&f.event.counter_3520==UINT32_MAX);
  deliveries+=wanted;
 }
 Fixture f;init(&f);f.wait.handshake=0xfefe;f.owners.registered=NULL;
 assert(!taxi_frame_audio_deliver(&f.owners,&f.wait)&&f.wait.handshake==1&&f.event.acknowledge==1&&f.audio==1&&!f.registered);
 init(&f);f.wait.handshake=0xfefe;f.event.callback_352c=0x08017001;f.owners.registered=NULL;
 assert(taxi_frame_audio_deliver(&f.owners,&f.wait)==-1&&f.wait.handshake==0xfefe&&f.event.acknowledge==UINT16_MAX&&f.audio==1&&!f.registered);
 init(&f);f.event.callback_352c=0x08017001;
 assert(!taxi_frame_audio_deliver(&f.owners,&f.wait)&&f.wait.handshake==1&&f.registered==1);
 init(&f);f.wait.handshake=0xfefe;f.audio_error=-1;
 assert(taxi_frame_audio_deliver(&f.owners,&f.wait)==-1&&f.wait.handshake==0xfefe&&f.event.acknowledge==UINT16_MAX&&!f.registered);
 init(&f);f.wait.handshake=0xfefe;f.registered_error=-1;
 assert(taxi_frame_audio_deliver(&f.owners,&f.wait)==-1&&f.wait.handshake==0xfefe&&f.event.acknowledge==UINT16_MAX&&f.registered==1);
 assert(taxi_frame_audio_deliver(NULL,&f.wait)==-1&&taxi_frame_audio_deliver(&f.owners,NULL)==-1);
 f.owners.event=NULL;assert(taxi_frame_audio_deliver(&f.owners,&f.wait)==-1);
 printf("PASS all65536 handshake prestates, %u composed native events, low-byte wait counts/enabled gate, default callback, fresh flags and failure-before-ack boundaries; mixer stubbed for protocol checks\n",deliveries);
 return 0;
}
