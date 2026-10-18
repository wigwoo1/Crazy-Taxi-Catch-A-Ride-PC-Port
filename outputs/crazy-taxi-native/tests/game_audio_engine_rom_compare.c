#include "game_audio_engine.h"
#include <stdio.h>
int main(void){unsigned a,b,sa,sb;int low,high,amplitude,pitch;
 while(scanf("%u%u%d%d%d%d%u%u",&a,&b,&low,&high,&amplitude,&pitch,&sa,&sb)==8){
  TaxiGameAudioEngine s={{a,b}};uint32_t starts[2]={sa,sb};TaxiGameAudioEngineRequest q[6];
  unsigned n=taxi_game_audio_engine_step(&s,low,high,amplitude,pitch,starts,q);
  printf("%u %u %u",s.handle[0],s.handle[1],n);
  for(unsigned i=0;i<n;i++)printf(" %u %u %d",q[i].op,q[i].handle,q[i].value);puts("");
 }return 0;}
