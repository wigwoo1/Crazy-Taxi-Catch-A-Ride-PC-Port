#include "frame_audio_owned.h"
typedef struct {TaxiFrameAudioOwners *owner;TaxiFrameWait *wait;} Delivery;
static int event_call(void *context,TaxiFrameEvent *event,uint32_t target,uint32_t argument){
 Delivery *d=context;TaxiFrameAudioOwners *o=d->owner;(void)event;
 switch(target){
 case 0x08026888:return taxi_audio_frame_step(o->audio,o->tracker,o->music,o->pitch,o->music_context,o->channels,o->effects,o->mix,o->capacity,o->buffers,&o->dispatched);
 case 0x0803244c:
  return o->registered?o->registered(o->context,argument):taxi_frame_event_callback_is_noop(argument)?0:-1;
 case 0x080161f4:return taxi_frame_wait_signal(d->wait);
 default:return -1;
 }
}
int taxi_frame_audio_deliver(void *context,TaxiFrameWait *wait){
 TaxiFrameAudioOwners *o=context;if(!o||!o->event||!wait)return -1;
 Delivery delivery={o,wait};
 return taxi_frame_event(o->event,o->scanline,event_call,&delivery);
}

int taxi_frame_audio_deliver_after_counter(TaxiFrameAudioOwners *o,TaxiFrameWait *wait,uint32_t counter){
 if(!o||!o->event||!wait)return -1;
 Delivery delivery={o,wait};
 return taxi_frame_event_after_counter(o->event,counter,event_call,&delivery);
}
