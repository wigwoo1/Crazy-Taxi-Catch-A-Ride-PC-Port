#include "game_audio_vehicle_loop.h"
unsigned taxi_game_audio_vehicle_loop(uint32_t *handle,const TaxiGameAudioVehicleLoopInput *i,TaxiGameAudioEngineRequest q[3]){
 unsigned n=0;
 if(!i->vehicle564&&!(i->flags28&8)&&!i->frozen&&!i->ended&&!i->fade){
  if(!*handle){
   q[n++]=(TaxiGameAudioEngineRequest){1,58,257};*handle=i->start;
   q[n++]=(TaxiGameAudioEngineRequest){2,*handle,16};
  }
 }else if(*handle){
  q[n++]=(TaxiGameAudioEngineRequest){6,*handle,58};
  if(i->busy){q[n++]=(TaxiGameAudioEngineRequest){4,*handle,0};*handle=0;}
 }
 return n;
}

unsigned taxi_game_audio_vehicle_loop_dispatch(uint32_t *handle,const TaxiGameAudioVehicleLoopInput *i,TaxiGameAudioDispatch call,void *context){
 if(!call)return 0;
 uint32_t reply=0;
 unsigned n=0;
 if(!i->vehicle564&&!(i->flags28&8)&&!i->frozen&&!i->ended&&!i->fade){
  if(!*handle){
   reply=call(context,(TaxiGameAudioEngineRequest){1,58,257});n++;*handle=(uint32_t)reply;
   reply=call(context,(TaxiGameAudioEngineRequest){2,*handle,16});n++;
  }
 }else if(*handle){
  reply=call(context,(TaxiGameAudioEngineRequest){6,*handle,58});n++;
  if((uint8_t)reply){reply=call(context,(TaxiGameAudioEngineRequest){4,*handle,0});n++;*handle=0;}
 }
 return n;
}
