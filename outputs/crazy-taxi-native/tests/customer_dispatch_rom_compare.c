#include "passenger_scene.h"
#include <stdio.h>
int main(int argc,char **argv){
 (void)argv;unsigned seed,selected,pending,phasec,phasef,phased;int x,z;
 while(scanf("%u%u%u%u%u%u%d%d",&seed,&selected,&pending,&phasec,&phasef,&phased,&x,&z)==8){
  TaxiPassengerScene s={.count=1,.use_native_random=1,.random_state=seed};
  s.marker_phase=(TaxiPassengerMarkerPhase){(uint8_t)phasec,(uint8_t)phasef,(uint8_t)phased};
  s.pickup.selected_index=selected?0:-1;s.pickup.pending_1704=(int32_t)pending;s.pickup.approach_x_7534=256;
  for(unsigned j=0;j<44;j++){unsigned b;if(scanf("%u",&b)!=1)return 2;s.customers[0].bytes[j]=(uint8_t)b;}
  TaxiPassengerFrameInput in={.delta=1,.freeze=(uint8_t)(argc>1),.taxi={x,0,z}};
  if(taxi_passenger_scene_customer_service(&s,&in))return 3;
  printf("%u %u %u %u",s.random_state,s.marker_phase.clock_5f28,s.marker_phase.frame_5f29,s.marker_phase.direction_5f2a);
  for(unsigned j=0;j<44;j++)printf(" %u",s.customers[0].bytes[j]);puts("");
 }
 return 0;
}
