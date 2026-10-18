#include "game_audio_events.h"
#include <stdio.h>
int main(void){unsigned a,b,effects,action,intro,end,freeze;int timer,speed,delta;
 while(scanf("%u%u%d%d%d%u%u%u%u%u",&a,&b,&timer,&speed,&delta,&effects,&action,&intro,&end,&freeze)==10){
  TaxiGameAudioEvents s={(uint8_t)a,(uint8_t)b,timer};TaxiGameAudioEventsInput i={speed,delta,(uint8_t)effects,(uint8_t)action,(uint8_t)intro,(uint8_t)end,(uint8_t)freeze};TaxiGameAudioEventRequests q;
  taxi_game_audio_events_step(&s,&i,&q);
  printf("%u %u %d %u %u",s.flag0,s.flag1,s.hold_timer,q.query_action2,q.count);
  for(unsigned j=0;j<q.count;j++)printf(" %u",q.sounds[j]);puts("");
 }return 0;}
