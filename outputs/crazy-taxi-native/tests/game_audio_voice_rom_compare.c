#include "game_audio_voice.h"
#include <stdio.h>
int main(void){unsigned h,other,settings,cd,intro,end,freeze,busy,start;int sel,driver;
 while(scanf("%u%u%d%u%d%u%u%u%u%u%u",&h,&other,&sel,&settings,&driver,&cd,&intro,&end,&freeze,&busy,&start)==11){
  TaxiGameAudioVoice s={h,other,(int8_t)sel};TaxiGameAudioVoiceInput i={(uint8_t)settings,(uint8_t)cd,(uint8_t)intro,(uint8_t)end,(uint8_t)freeze,(uint8_t)busy,(int8_t)driver,start};TaxiGameAudioEngineRequest q[1];unsigned n=taxi_game_audio_voice_step(&s,&i,q);
  printf("%u %d %u",s.handle,s.selection,n);if(n)printf(" %u %u %d",q[0].op,q[0].handle,q[0].value);puts("");
 }return 0;}
