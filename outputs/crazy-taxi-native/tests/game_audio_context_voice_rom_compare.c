#include "game_audio_context_voice.h"
#include <stdio.h>
int main(void){unsigned h,dh,last,intro,end,freeze,busy,start;int sel,cls,follow,cur,alt,random;
 while(scanf("%u%u%d%d%d%u%u%u%u%u%d%d%u%d",&h,&dh,&sel,&cls,&follow,&last,&intro,&end,&freeze,&busy,&cur,&alt,&start,&random)==14){
 TaxiGameAudioContextVoice s={h,dh,(int8_t)sel,(int8_t)cls,(int8_t)follow,(uint8_t)last};
 TaxiGameAudioContextVoiceInput i={(uint8_t)intro,(uint8_t)end,(uint8_t)freeze,(uint8_t)busy,(int8_t)cur,(int8_t)alt,start,random};TaxiGameAudioEngineRequest q[2];unsigned n=taxi_game_audio_context_voice_step(&s,&i,q);
 printf("%u %d %d %d %u %u",s.handle,s.selection,s.classification,s.followup,s.last,n);
 for(unsigned j=0;j<n;j++)printf(" %u %u %d",q[j].op,q[j].handle,q[j].value);puts("");
 }return 0;}
