#include "game_audio_final_event.h"
#include <stdio.h>
int main(void){unsigned h,start,busy;int timer,delta;while(scanf("%u%d%d%u%u",&h,&timer,&delta,&start,&busy)==5){TaxiGameAudioFinalEvent s={h,(int16_t)timer};TaxiGameAudioEngineRequest q[2];unsigned n;int ready=taxi_game_audio_final_event_step(&s,delta,start,(uint8_t)busy,q,&n);printf("%u %d %d %u",s.handle,s.timer,ready,n);for(unsigned j=0;j<n;j++)printf(" %u %u %d",q[j].op,q[j].handle,q[j].value);puts("");}return 0;}
