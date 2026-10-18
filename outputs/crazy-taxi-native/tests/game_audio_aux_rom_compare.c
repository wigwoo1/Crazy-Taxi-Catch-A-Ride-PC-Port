#include "game_audio_aux.h"
#include <stdio.h>
int main(void){unsigned loop,one,pending,action,over,intro,end,freeze,busy,ls,os;int speed;
 while(scanf("%u%u%u%d%u%u%u%u%u%u%u%u",&loop,&one,&pending,&speed,&action,&over,&intro,&end,&freeze,&busy,&ls,&os)==12){
 TaxiGameAudioAux s={loop,one,(uint8_t)pending};TaxiGameAudioAuxInput i={speed,(uint8_t)action,(uint8_t)over,(uint8_t)intro,(uint8_t)end,(uint8_t)freeze,(uint8_t)busy,ls,os};TaxiGameAudioEngineRequest q[6];
 unsigned n=taxi_game_audio_aux_step(&s,&i,q);printf("%u %u %u %u",s.loop,s.one_shot,s.pending,n);
 for(unsigned j=0;j<n;j++)printf(" %u %u %d",q[j].op,q[j].handle,q[j].value);puts("");
 }return 0;}
