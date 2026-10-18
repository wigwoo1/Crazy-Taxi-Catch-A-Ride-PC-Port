#include "challenge_delivery.h"
int taxi_challenge_delivery_accept(const TaxiChallengeDeliveryInput *in,TaxiDeliveryEffects *out){
 if(!in||!out)return -1;
 TaxiDeliveryEffects e={.gate_1700=in->delivery.gate_1700};
 if(in->submode1537<=2||in->ended1697||in->pause5f20){*out=e;return 0;}
 TaxiDeliveryInput delivery=in->delivery;
 /* 0790E branch has no ordinary1708 gate. */
 delivery.gate_1708=0;
 int rc=taxi_passenger_delivery(&delivery,&e);if(rc)return rc;
 *out=e;return 0;
}
int taxi_challenge_delivery_rank_request(uint8_t mode,int32_t timer){
 return (mode==4||mode==5||mode==7)&&timer!=0;
}
