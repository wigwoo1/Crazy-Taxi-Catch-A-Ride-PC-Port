#include "game_audio_aux.h"
#include <limits.h>
static int32_t signed32(uint32_t v){return v<=INT32_MAX?(int32_t)v:(int32_t)((int64_t)v-4294967296LL);}
unsigned taxi_game_audio_aux_step(TaxiGameAudioAux *s,const TaxiGameAudioAuxInput *i,TaxiGameAudioEngineRequest q[6]){
 unsigned n=0;int32_t magnitude=i->speed_class<0?signed32(0u-(uint32_t)i->speed_class):i->speed_class;
 int active=0;
 if(magnitude<=64&&i->speed_class){
  q[n++]=(TaxiGameAudioEngineRequest){5,0,3};
  active=(i->action3||i->override2620)&&!i->intro&&!i->ended&&!i->frozen;
 }
 if(active){
  if(!s->loop){q[n++]=(TaxiGameAudioEngineRequest){1,58,257};s->loop=i->loop_start;}
  int32_t value=signed32(64u-(uint32_t)magnitude);
  value=value>=0?value/2:(int32_t)(-1-((-(int64_t)value-1)/2));
  q[n++]=(TaxiGameAudioEngineRequest){2,s->loop,value};
 }else if(s->loop){q[n++]=(TaxiGameAudioEngineRequest){4,s->loop,0};s->loop=0;}
 if(s->pending&&!s->one_shot){
  q[n++]=(TaxiGameAudioEngineRequest){1,56,0};s->one_shot=i->one_shot_start;s->pending=0;
 }
 if(s->one_shot){
  q[n++]=(TaxiGameAudioEngineRequest){6,s->one_shot,56};
  if(!i->busy)s->one_shot=0;
 }
 return n;
}

unsigned taxi_game_audio_aux_dispatch(TaxiGameAudioAux *s,const TaxiGameAudioAuxInput *i,TaxiGameAudioDispatch call,void *context){
 if(!call)return 0;
 uint32_t reply=0;
 unsigned n=0;int32_t magnitude=i->speed_class<0?signed32(0u-(uint32_t)i->speed_class):i->speed_class;
 int active=0;
 if(magnitude<=64&&i->speed_class){
  reply=call(context,(TaxiGameAudioEngineRequest){5,0,3});n++;
  active=((uint8_t)reply||i->override2620)&&!i->intro&&!i->ended&&!i->frozen;
 }
 if(active){
  if(!s->loop){reply=call(context,(TaxiGameAudioEngineRequest){1,58,257});n++;s->loop=(uint32_t)reply;}
  int32_t value=signed32(64u-(uint32_t)magnitude);
  value=value>=0?value/2:(int32_t)(-1-((-(int64_t)value-1)/2));
  reply=call(context,(TaxiGameAudioEngineRequest){2,s->loop,value});n++;
 }else if(s->loop){reply=call(context,(TaxiGameAudioEngineRequest){4,s->loop,0});n++;s->loop=0;}
 if(s->pending&&!s->one_shot){
  reply=call(context,(TaxiGameAudioEngineRequest){1,56,0});n++;s->one_shot=(uint32_t)reply;s->pending=0;
 }
 if(s->one_shot){
  reply=call(context,(TaxiGameAudioEngineRequest){6,s->one_shot,56});n++;
  if(!(uint8_t)reply)s->one_shot=0;
 }
 return n;
}
