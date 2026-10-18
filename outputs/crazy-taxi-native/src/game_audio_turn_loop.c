#include "game_audio_turn_loop.h"
unsigned taxi_game_audio_turn_loop(TaxiGameAudioTurnLoop *s,const TaxiGameAudioTurnLoopInput *i,TaxiGameAudioEngineRequest q[4]){
 unsigned n=0;
 if(s->event_handle){q[n++]=(TaxiGameAudioEngineRequest){6,s->event_handle,44};if(!i->event_busy)s->event_handle=0;}
 int angle=i->angle578;if(angle<0)angle=-angle;
 if(i->enabled25d4&&!(i->flags28&8)&&angle>768&&!i->frozen&&!i->ended&&!i->fade){
  if(!s->loop_handle){q[n++]=(TaxiGameAudioEngineRequest){1,58,257};s->loop_handle=i->start;q[n++]=(TaxiGameAudioEngineRequest){2,s->loop_handle,64};}
 }else if(s->loop_handle){
  q[n++]=(TaxiGameAudioEngineRequest){6,s->loop_handle,58};
  if(i->loop_busy)q[n++]=(TaxiGameAudioEngineRequest){4,s->loop_handle,0};
  s->loop_handle=0;
 }
 return n;
}

unsigned taxi_game_audio_turn_loop_dispatch(TaxiGameAudioTurnLoop *s,const TaxiGameAudioTurnLoopInput *i,TaxiGameAudioDispatch call,void *context){
 if(!call)return 0;
 uint32_t reply=0;
 unsigned n=0;
 if(s->event_handle){reply=call(context,(TaxiGameAudioEngineRequest){6,s->event_handle,44});n++;if(!(uint8_t)reply)s->event_handle=0;}
 int angle=i->angle578;if(angle<0)angle=-angle;
 if(i->enabled25d4&&!(i->flags28&8)&&angle>768&&!i->frozen&&!i->ended&&!i->fade){
  if(!s->loop_handle){reply=call(context,(TaxiGameAudioEngineRequest){1,58,257});n++;s->loop_handle=(uint32_t)reply;reply=call(context,(TaxiGameAudioEngineRequest){2,s->loop_handle,64});n++;}
 }else if(s->loop_handle){
  reply=call(context,(TaxiGameAudioEngineRequest){6,s->loop_handle,58});n++;
  if((uint8_t)reply){call(context,(TaxiGameAudioEngineRequest){4,s->loop_handle,0});n++;}
  s->loop_handle=0;
 }
 return n;
}
