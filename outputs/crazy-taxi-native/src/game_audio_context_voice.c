#include "game_audio_context_voice.h"
#include "random.h"
unsigned taxi_game_audio_context_voice_step(TaxiGameAudioContextVoice *s,const TaxiGameAudioContextVoiceInput *i,TaxiGameAudioEngineRequest q[2]){
 if(i->intro||i->ended||i->frozen)return 0;
 if(s->selection!=-1&&!s->handle&&!s->driver_handle){
  s->last=(uint8_t)s->selection;
  s->classification=(uint8_t)(s->last-18)<=3?i->alternate_class:i->current_class;
  if(s->classification==-1)return 0;
  q[0]=(TaxiGameAudioEngineRequest){1,(uint32_t)(s->classification*24+s->selection+83),0};
  s->handle=i->start;return 1;
 }
 if(!s->handle)return 0;
 q[0]=(TaxiGameAudioEngineRequest){6,s->handle,s->classification*24+s->selection+83};
 if(i->busy)return 1;
 int divisor=0,offset=0;unsigned n=1;
 if(s->last<=1){divisor=2;offset=7;}
 else if((uint8_t)(s->last-2)<=1){divisor=3;}
 else if((uint8_t)(s->last-4)<=1){divisor=3;offset=4;}
 else if((uint8_t)(s->last-12)<=1){divisor=2;offset=11;}
 else if((uint8_t)(s->last-9)<=2){s->followup=17;}
 else if((uint8_t)(s->last-6)<=2){divisor=2;offset=13;}
 if(divisor){q[n++]=(TaxiGameAudioEngineRequest){8,0,65535};s->followup=(int8_t)(i->random%divisor+offset);}
 s->selection=-1;s->handle=0;return n;
}

unsigned taxi_game_audio_context_voice_dispatch(TaxiGameAudioContextVoice *s,const TaxiGameAudioContextVoiceInput *i,TaxiGameAudioDispatch call,void *context){
 if(!call)return 0;
 uint32_t reply=0;
 if(i->intro||i->ended||i->frozen)return 0;
 if(s->selection!=-1&&!s->handle&&!s->driver_handle){
  s->last=(uint8_t)s->selection;
  s->classification=(uint8_t)(s->last-18)<=3?i->alternate_class:i->current_class;
  if(s->classification==-1)return 0;
  reply=call(context,(TaxiGameAudioEngineRequest){1,(uint32_t)(s->classification*24+s->selection+83),0});
  s->handle=reply;return 1;
 }
 if(!s->handle)return 0;
 reply=call(context,(TaxiGameAudioEngineRequest){6,s->handle,s->classification*24+s->selection+83});
 if((uint8_t)reply)return 1;
 int divisor=0,offset=0;unsigned n=1;
 if(s->last<=1){divisor=2;offset=7;}
 else if((uint8_t)(s->last-2)<=1){divisor=3;}
 else if((uint8_t)(s->last-4)<=1){divisor=3;offset=4;}
 else if((uint8_t)(s->last-12)<=1){divisor=2;offset=11;}
 else if((uint8_t)(s->last-9)<=2){s->followup=17;}
 else if((uint8_t)(s->last-6)<=2){divisor=2;offset=13;}
 if(divisor){reply=call(context,(TaxiGameAudioEngineRequest){8,0,65535});n++;s->followup=(int8_t)(((int32_t)(reply<=2147483647u?reply:(int64_t)reply-4294967296LL))%divisor+offset);}
 s->selection=-1;s->handle=0;return n;
}
int8_t taxi_game_audio_context_voice_select(uint32_t *rng,int mode){
 if(!rng)return -1;
 int32_t draw=taxi_random_range(rng,0,65535);
 return (int8_t)(draw%2+(mode?14:16));
}
int taxi_game_audio_customer_class(const TaxiPassengerState *customer,int8_t *out){
 static const int8_t classes[8]={1,0,0,1,0,0,1,1};
 if(!out)return -1;
 if(!customer){*out=-1;return 0;}
 int type=customer->bytes[1];if(type>=128)type-=256;
 if(type<0||type>=8)return -1;
 *out=classes[type];return 0;
}

int taxi_game_audio_passenger_classes(const TaxiPassengerScene *scene,int8_t *current,int8_t *alternate){
 if(!scene||!current||!alternate||current==alternate||scene->count>128)return -1;
 int32_t a=scene->session.trip.active_customer,b=scene->pickup.dialogue_index;
 if(a < -1 || b < -1 || (a>=0&&(uint32_t)a>=scene->count) || (b>=0&&(uint32_t)b>=scene->count))return -1;
 int8_t ac,bc;
 if(taxi_game_audio_customer_class(a==-1?0:&scene->customers[a],&ac)||
    taxi_game_audio_customer_class(b==-1?0:&scene->customers[b],&bc))return -1;
 *current=ac;*alternate=bc;return 0;
}
