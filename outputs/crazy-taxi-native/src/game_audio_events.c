#include "game_audio_events.h"
#include <limits.h>
void taxi_game_audio_events_step(TaxiGameAudioEvents *s,const TaxiGameAudioEventsInput *i,TaxiGameAudioEventRequests *q){
 *q=(TaxiGameAudioEventRequests){0};
 if(s->flag0){s->flag0=0;if(i->effects)q->sounds[q->count++]=25;}
 if(s->flag1){s->flag1=0;if(i->effects)q->sounds[q->count++]=24;}
 q->query_action2=(uint32_t)i->speed_class<=64;
 if(q->query_action2&&i->action2&&!i->intro&&!i->ended&&!i->frozen){
  if(!s->hold_timer)s->hold_timer=60;
 }else if(s->hold_timer){
  uint32_t value=(uint32_t)s->hold_timer-(uint32_t)i->delta;
  s->hold_timer=value<=INT32_MAX?(int32_t)value:(int32_t)((int64_t)value-4294967296LL);
  if(s->hold_timer<=0)s->hold_timer=0;
 }
}

unsigned taxi_game_audio_events_dispatch(TaxiGameAudioEvents *s,const TaxiGameAudioEventsInput *i,TaxiGameAudioDispatch call,void *context){
 if(!call)return 0;
 unsigned n=0;
 if(s->flag0){s->flag0=0;if(i->effects){call(context,(TaxiGameAudioEngineRequest){9,25,0});n++;}}
 if(s->flag1){s->flag1=0;if(i->effects){call(context,(TaxiGameAudioEngineRequest){9,24,0});n++;}}
 int query=(uint32_t)i->speed_class<=64;uint8_t action=0;
 if(query){action=(uint8_t)call(context,(TaxiGameAudioEngineRequest){5,0,2});n++;}
 if(query&&action&&!i->intro&&!i->ended&&!i->frozen){if(!s->hold_timer)s->hold_timer=60;}
 else if(s->hold_timer){uint32_t value=(uint32_t)s->hold_timer-(uint32_t)i->delta;
  s->hold_timer=value<=INT32_MAX?(int32_t)value:(int32_t)((int64_t)value-4294967296LL);if(s->hold_timer<=0)s->hold_timer=0;}
 return n;
}
