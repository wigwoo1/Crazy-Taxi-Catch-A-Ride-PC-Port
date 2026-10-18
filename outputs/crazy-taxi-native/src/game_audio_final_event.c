#include "game_audio_final_event.h"
int taxi_game_audio_final_event_step(TaxiGameAudioFinalEvent *s,int32_t delta,uint32_t start,uint8_t busy,TaxiGameAudioEngineRequest q[2],unsigned *count){
 unsigned n=0;
 if(s->timer){
  uint16_t raw=(uint16_t)((uint32_t)(uint16_t)s->timer-(uint32_t)delta);
  s->timer=(int16_t)(raw<32768?raw:(int32_t)raw-65536);
  if(s->timer>0){*count=0;return 0;}
  s->timer=0;q[n++]=(TaxiGameAudioEngineRequest){1,70,0};s->handle=start;
 }
 q[n++]=(TaxiGameAudioEngineRequest){6,s->handle,70};*count=n;return !busy;
}

int taxi_game_audio_final_event_dispatch(TaxiGameAudioFinalEvent *s,int32_t delta,TaxiGameAudioDispatch call,void *context,unsigned *count){
 if(!call){*count=0;return 0;}
 uint32_t reply=0;
 unsigned n=0;
 if(s->timer){
  uint16_t raw=(uint16_t)((uint32_t)(uint16_t)s->timer-(uint32_t)delta);
  s->timer=(int16_t)(raw<32768?raw:(int32_t)raw-65536);
  if(s->timer>0){*count=0;return 0;}
  s->timer=0;reply=call(context,(TaxiGameAudioEngineRequest){1,70,0});n++;s->handle=reply;
 }
 reply=call(context,(TaxiGameAudioEngineRequest){6,s->handle,70});n++;*count=n;return !(uint8_t)reply;
}
