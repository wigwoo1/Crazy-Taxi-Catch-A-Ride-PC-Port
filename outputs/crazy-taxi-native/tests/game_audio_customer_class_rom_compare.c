#include "game_audio_context_voice.h"
#include <stdio.h>
int main(void){int cur,alt,sel;
 while(scanf("%d%d%d",&cur,&alt,&sel)==3){
 TaxiPassengerScene scene={0};scene.count=2;
 scene.customers[0].bytes[1]=(uint8_t)cur;scene.customers[1].bytes[1]=(uint8_t)alt;
 scene.session.trip.active_customer=cur==-1?-1:0;scene.pickup.dialogue_index=alt==-1?-1:1;
 scene.pickup.selected_index=0; /* Deliberately differs from alternate. */
 int8_t ac=37,bc=37;
 if(taxi_game_audio_passenger_classes(&scene,&ac,&bc))return 2;
 TaxiGameAudioContextVoice s={0,0,(int8_t)sel,37,37,255};
 TaxiGameAudioContextVoiceInput i={0,0,0,0,ac,bc,0x2009040,0};
 TaxiGameAudioEngineRequest q[2];unsigned n=taxi_game_audio_context_voice_step(&s,&i,q);
 printf("%u %d %d %d %u %u",s.handle,s.selection,s.classification,s.followup,s.last,n);
 for(unsigned j=0;j<n;j++)printf(" %u %u %d",q[j].op,q[j].handle,q[j].value);puts("");
 }return 0;}
