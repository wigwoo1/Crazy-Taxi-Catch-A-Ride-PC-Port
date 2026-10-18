#include "game_audio_turn_loop.h"
#include <stdio.h>
int main(void){unsigned a,b,enabled,flags,freeze,end,fade,eb,lb,start;int angle;
 while(scanf("%u%u%d%u%u%u%u%u%u%u%u",&a,&b,&angle,&enabled,&flags,&freeze,&end,&fade,&eb,&lb,&start)==11){
 TaxiGameAudioTurnLoop s={a,b};TaxiGameAudioTurnLoopInput i={(int16_t)angle,(uint8_t)enabled,(uint8_t)flags,(uint8_t)freeze,(uint8_t)end,(uint8_t)fade,(uint8_t)eb,(uint8_t)lb,start};TaxiGameAudioEngineRequest q[4];unsigned n=taxi_game_audio_turn_loop(&s,&i,q);
 printf("%u %u %u",s.event_handle,s.loop_handle,n);for(unsigned j=0;j<n;j++)printf(" %u %u %d",q[j].op,q[j].handle,q[j].value);puts("");
 }return 0;}
