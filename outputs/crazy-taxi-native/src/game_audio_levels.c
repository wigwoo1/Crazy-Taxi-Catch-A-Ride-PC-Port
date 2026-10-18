#include "game_audio_levels.h"
#include <limits.h>
static int32_t signed32(uint32_t v){return v<=INT32_MAX?(int32_t)v:(int32_t)((int64_t)v-4294967296LL);}
static int32_t mul(int32_t a,int32_t b){return signed32((uint32_t)a*(uint32_t)b);}
static int32_t asr(int32_t v,unsigned n){return v>=0?v/(int32_t)(1u<<n):(int32_t)(-1-((-(int64_t)v-1)/(1u<<n)));}
int taxi_game_audio_levels(const TaxiGameAudioLevelsInput *i,TaxiGameAudioLevels *o){
 if(!i||!o||!i->max_speed)return -1;
 TaxiGameAudioLevels s={0};
 s.cooldown=signed32((uint32_t)i->cooldown-(uint32_t)i->delta);if(s.cooldown<=0)s.cooldown=0;
 s.speed_class=i->speed/1138;
 int32_t magnitude=i->speed<0?signed32(0u-(uint32_t)i->speed):i->speed;
 int32_t numerator=mul(magnitude,100);
 if(numerator==INT32_MIN&&i->max_speed==-1)return -1;
 s.ratio=numerator/i->max_speed;
 s.low=s.ratio<=24?48:mul(signed32(100u-(uint32_t)s.ratio),48)/75;
 s.high=s.ratio<=9?0:s.ratio>50?48:mul(signed32((uint32_t)s.ratio-10u),48)/40;
 if(i->muted){s.low=0;s.high=0;}
 s.factor=i->factor;
 if(i->ended||i->outer==1||i->outer==2){
  s.low=asr(mul(s.low,i->factor),8);s.high=asr(mul(s.high,i->factor),8);
  uint16_t raw=(uint16_t)((uint32_t)(uint16_t)i->factor-((uint32_t)i->delta<<2));
  s.factor=(int16_t)(raw<32768?raw:(int32_t)raw-65536);
  if(s.factor<=63)s.factor=64;
 }
 s.pitch=signed32(0x2910u+(uint32_t)asr(mul(i->rpm,i->scale),12));
 *o=s;return 0;
}
