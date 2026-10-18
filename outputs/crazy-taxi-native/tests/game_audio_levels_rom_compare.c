#include "game_audio_levels.h"
#include <stdio.h>
int main(void){int speed,max,scale,cooldown,delta,rpm,factor;unsigned mute,end,outer;
 while(scanf("%d%d%d%d%d%d%d%u%u%u",&speed,&max,&scale,&cooldown,&delta,&rpm,&factor,&mute,&end,&outer)==10){
  TaxiGameAudioLevelsInput i={speed,max,scale,cooldown,delta,(int16_t)rpm,(int16_t)factor,(uint8_t)mute,(uint8_t)end,(uint8_t)outer};TaxiGameAudioLevels o;
  if(taxi_game_audio_levels(&i,&o))return 2;
  printf("%d %d %d %d %d %d %d\n",o.cooldown,o.speed_class,o.ratio,o.low,o.high,o.pitch,o.factor);
 }return 0;}
