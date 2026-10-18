#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_context_voice.h"
#include <assert.h>
#include <stdio.h>
int main(void){const int8_t expected[]={1,0,0,1,0,0,1,1};TaxiPassengerState p={{0}};int8_t out=37;assert(!taxi_game_audio_customer_class(0,&out)&&out==-1);
 for(unsigned i=0;i<256;i++){p.bytes[1]=(uint8_t)i;out=37;int rc=taxi_game_audio_customer_class(&p,&out);if(i<8)assert(!rc&&out==expected[i]);else assert(rc==-1&&out==37);}
 assert(taxi_game_audio_customer_class(&p,0)==-1);
 TaxiPassengerScene scene={0};scene.count=3;scene.customers[0].bytes[1]=0;scene.customers[1].bytes[1]=1;scene.customers[2].bytes[1]=3;
 scene.session.trip.active_customer=1;scene.pickup.dialogue_index=0;scene.pickup.selected_index=2;
 int8_t cur=37,alt=38;assert(!taxi_game_audio_passenger_classes(&scene,&cur,&alt)&&cur==0&&alt==1);
 scene.session.trip.active_customer=-1;assert(!taxi_game_audio_passenger_classes(&scene,&cur,&alt)&&cur==-1&&alt==1);
 scene.pickup.dialogue_index=-1;assert(!taxi_game_audio_passenger_classes(&scene,&cur,&alt)&&cur==-1&&alt==-1);
 for(int j=-2;j<=128;j++){if(j>=-1&&j<3)continue;scene.pickup.dialogue_index=j;cur=37;alt=38;assert(taxi_game_audio_passenger_classes(&scene,&cur,&alt)==-1&&cur==37&&alt==38);}
 scene.pickup.dialogue_index=0;scene.customers[0].bytes[1]=255;cur=37;alt=38;assert(taxi_game_audio_passenger_classes(&scene,&cur,&alt)==-1&&cur==37&&alt==38);
 assert(taxi_game_audio_passenger_classes(&scene,&cur,&cur)==-1);puts("PASS original8 customer classes/absent sentinel and invalid signedclass transactional rejection");return 0;}
