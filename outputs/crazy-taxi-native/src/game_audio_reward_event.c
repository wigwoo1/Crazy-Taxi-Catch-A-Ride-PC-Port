#include "game_audio_reward_event.h"
unsigned taxi_game_audio_reward_event(TaxiGameAudioRewardEvent *s,const TaxiGameAudioRewardEventInput *i,TaxiGameAudioEngineRequest q[2]){
 unsigned n=0;if(!s->pending)return 0;
 if(!s->handle){q[n++]=(TaxiGameAudioEngineRequest){1,44,0};s->handle=i->start;}
 s->pending=0;
 if(!i->gate00e4&&!s->cooldown){
  unsigned whole=i->speed>0x21570;int fraction=whole?0:i->speed>0x19014?75:i->speed>0x10ab8?50:25;
  q[n++]=(TaxiGameAudioEngineRequest){7,whole,fraction};s->cooldown=20;
 }
 return n;
}

unsigned taxi_game_audio_reward_event_dispatch(TaxiGameAudioRewardEvent *s,const TaxiGameAudioRewardEventInput *i,TaxiGameAudioDispatch call,void *context){
 if(!call)return 0;
 uint32_t reply=0;
 unsigned n=0;if(!s->pending)return 0;
 if(!s->handle){reply=call(context,(TaxiGameAudioEngineRequest){1,44,0});n++;s->handle=reply;}
 s->pending=0;
 if(!i->gate00e4&&!s->cooldown){
  unsigned whole=i->speed>0x21570;int fraction=whole?0:i->speed>0x19014?75:i->speed>0x10ab8?50:25;
  reply=call(context,(TaxiGameAudioEngineRequest){7,whole,fraction});n++;s->cooldown=20;
 }
 return n;
}
