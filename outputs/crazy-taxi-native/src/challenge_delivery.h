#ifndef TAXI_CHALLENGE_DELIVERY_H
#define TAXI_CHALLENGE_DELIVERY_H
#include "passenger_delivery.h"
typedef struct { TaxiDeliveryInput delivery; uint8_t submode1537,ended1697,pause5f20; } TaxiChallengeDeliveryInput;
/* Original0790E..0797E acceptance. Caller performs E034 distance update,
   DF8C(),2939C(),CD38(1) in order after acceptance. Does not complete trip. */
int taxi_challenge_delivery_accept(const TaxiChallengeDeliveryInput *,TaxiDeliveryEffects *);
/* Call AFTER those handlers, using their retained16A8. Only4/5/7 request
   success/rank here. Caller then sets153C/1697 and calls9AB0(0). */
int taxi_challenge_delivery_rank_request(uint8_t submode,int32_t retained16a8);
#endif
