#include "game_audio_voice.h"
#include <limits.h>
unsigned taxi_game_audio_voice_step(TaxiGameAudioVoice *s,const TaxiGameAudioVoiceInput *i,TaxiGameAudioEngineRequest q[1]){
 if(i->intro||i->ended||i->frozen)return 0;
 int driver=(i->settings&224)==128?i->challenge_driver:i->driver;
 int index=driver*18+s->selection+131;
 if(s->selection!=-1&&!s->other_handle&&!s->handle){
  q[0]=(TaxiGameAudioEngineRequest){1,(uint32_t)index,0};s->handle=i->start;return 1;
 }
 if(!s->handle)return 0;
 q[0]=(TaxiGameAudioEngineRequest){6,s->handle,index};
 if(!i->busy){s->selection=-1;s->handle=0;}
 return 1;
}
int32_t taxi_game_audio_amplitude_step(int32_t amplitude,int32_t delta){
 if(amplitude==16384)return amplitude;
 uint32_t value=(uint32_t)amplitude+((uint32_t)delta<<7);
 int32_t result=value<=INT32_MAX?(int32_t)value:(int32_t)((int64_t)value-4294967296LL);
 return result>16384?16384:result;
}

unsigned taxi_game_audio_voice_dispatch(TaxiGameAudioVoice *s,const TaxiGameAudioVoiceInput *i,TaxiGameAudioDispatch call,void *context){
 if(!call)return 0;
 uint32_t reply=0;
 if(i->intro||i->ended||i->frozen)return 0;
 int driver=(i->settings&224)==128?i->challenge_driver:i->driver;
 int index=driver*18+s->selection+131;
 if(s->selection!=-1&&!s->other_handle&&!s->handle){
  reply=call(context,(TaxiGameAudioEngineRequest){1,(uint32_t)index,0});s->handle=reply;return 1;
 }
 if(!s->handle)return 0;
 reply=call(context,(TaxiGameAudioEngineRequest){6,s->handle,index});
 if(!(uint8_t)reply){s->selection=-1;s->handle=0;}
 return 1;
}