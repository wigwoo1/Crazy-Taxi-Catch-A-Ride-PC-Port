#include "../src/challenge_delivery.h"
#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){
 uint8_t record[48]={0};memset(record+40,255,8);record[40]=123;record[41]=0;
 TaxiChallengeDeliveryInput in={.delivery={.destination_index=0,.material_e60=123,.active_1694=1,.destination_record=record},.submode1537=4};
 TaxiDeliveryEffects e;
 for(unsigned mode=0;mode<256;mode++){
  in.submode1537=(uint8_t)mode;
  assert(!taxi_challenge_delivery_accept(&in,&e));assert(e.accepted==(mode>2));assert(e.gate_1700==(mode>2?120:0));
  for(int timer=-1;timer<=1;timer++)assert(taxi_challenge_delivery_rank_request((uint8_t)mode,timer)==((mode==4||mode==5||mode==7)&&timer!=0));
 }
 in.submode1537=4;in.delivery.gate_1708=1;
 assert(!taxi_challenge_delivery_accept(&in,&e)&&e.accepted);
 for(unsigned gate=0;gate<8;gate++){
  TaxiChallengeDeliveryInput bad=in;
  if(gate==0)bad.ended1697=255;if(gate==1)bad.pause5f20=128;
  if(gate==2)bad.delivery.destination_index=-1;if(gate==3)bad.delivery.active_1694=0;
  if(gate==4)bad.delivery.gate_1700=-1;if(gate==5)bad.delivery.pending_1704=-1;
  if(gate==6)bad.delivery.speed_2b78=1;if(gate==7)bad.delivery.speed_2b78=-1;
  assert(!taxi_challenge_delivery_accept(&bad,&e)&&!e.accepted);
 }
 for(unsigned material=0;material<65536;material++){
  in.delivery.material_e60=material;assert(!taxi_challenge_delivery_accept(&in,&e));assert(e.accepted==(material==123));
 }
 in.delivery.destination_record=0;e.accepted=77;e.gate_1700=88;
 assert(taxi_challenge_delivery_accept(&in,&e)==-1&&e.accepted==77&&e.gate_1700==88);
 assert(taxi_challenge_delivery_accept(0,&e)==-1);
 puts("PASS challenge delivery all256submodes/65536signedmaterials/gates/post-handler timer rank; handlers remain caller-owned, not playable Turn.");
}
