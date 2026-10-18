#include "game_audio_engine.h"
#include <limits.h>
static int32_t signed32(uint32_t v){return v<=INT32_MAX?(int32_t)v:(int32_t)((int64_t)v-4294967296LL);}
static int32_t volume(int32_t level,int32_t amplitude){
 int32_t v=signed32((uint32_t)level*(uint32_t)amplitude)/48;
 return v>=0?v/256:(int32_t)(-1-((-(int64_t)v-1)/256));
}
unsigned taxi_game_audio_engine_step(TaxiGameAudioEngine *s,int32_t low,int32_t high,
 int32_t amplitude,int32_t pitch,const uint32_t starts[2],TaxiGameAudioEngineRequest out[6]){
 int32_t levels[2]={low,high};unsigned n=0;
 for(unsigned i=0;i<2;i++){
  uint32_t handle=s->handle[i];int32_t level=levels[i];
  if(!handle){
   if(!level)continue;
   out[n++]=(TaxiGameAudioEngineRequest){1,9+i,257};
   handle=starts[i];s->handle[i]=handle;
  }else if(!level){
   out[n++]=(TaxiGameAudioEngineRequest){4,handle,0};s->handle[i]=0;continue;
  }
  out[n++]=(TaxiGameAudioEngineRequest){2,handle,volume(level,amplitude)};
  out[n++]=(TaxiGameAudioEngineRequest){3,handle,pitch};
 }
 return n;
}

unsigned taxi_game_audio_engine_dispatch(TaxiGameAudioEngine *s,int32_t low,int32_t high,
 int32_t amplitude,int32_t pitch,TaxiGameAudioDispatch call,void *context){
 if(!s||!call)return 0;
 int32_t levels[2]={low,high};unsigned n=0;
 for(unsigned i=0;i<2;i++){
  uint32_t h=s->handle[i];int32_t level=levels[i];
  if(!h){
   if(!level)continue;
   h=call(context,(TaxiGameAudioEngineRequest){1,9+i,257});n++;s->handle[i]=h;
  }else if(!level){
   call(context,(TaxiGameAudioEngineRequest){4,h,0});n++;s->handle[i]=0;continue;
  }
  call(context,(TaxiGameAudioEngineRequest){2,h,volume(level,amplitude)});n++;
  call(context,(TaxiGameAudioEngineRequest){3,h,pitch});n++;
 }
 return n;
}
