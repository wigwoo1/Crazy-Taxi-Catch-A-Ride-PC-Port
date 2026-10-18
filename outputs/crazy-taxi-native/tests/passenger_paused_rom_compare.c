#include "passenger_idle.h"
#include <stdio.h>
int main(void){unsigned flags,timer,kind;
 while(scanf("%u%u%u",&flags,&timer,&kind)==3){
  TaxiPassengerState p={0};TaxiPickupState pick={.selected_index=-1};uint32_t rng=123;
  p.bytes[0]=(uint8_t)flags;p.bytes[1]=(uint8_t)kind;p.bytes[11]=(uint8_t)timer;
  if(taxi_passenger_idle_rng(&p,0,&pick,(TaxiRoutePoint){0,0},1,1,&rng)||taxi_passenger_frame_select(&p))return 2;
  for(unsigned i=0;i<44;i++)printf("%u ",p.bytes[i]);printf("%u\n",rng);
 }return 0;
}
