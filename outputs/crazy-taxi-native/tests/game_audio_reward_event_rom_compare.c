#include "game_audio_reward_event.h"
#include <stdio.h>
int main(void){unsigned h,pending,cooldown,start;int speed,gate;
 while(scanf("%u%u%u%d%d%u",&h,&pending,&cooldown,&speed,&gate,&start)==6){
  TaxiGameAudioRewardEvent s={h,(uint8_t)pending,(uint8_t)cooldown};TaxiGameAudioRewardEventInput i={speed,(int16_t)gate,start};TaxiGameAudioEngineRequest q[2];unsigned n=taxi_game_audio_reward_event(&s,&i,q);
  printf("%u %u %u %u",s.handle,s.pending,s.cooldown,n);for(unsigned j=0;j<n;j++)printf(" %u %u %d",q[j].op,q[j].handle,q[j].value);puts("");
 }return 0;}
