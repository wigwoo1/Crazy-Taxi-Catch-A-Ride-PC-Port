#include "game_audio_vehicle_loop.h"
#include <stdio.h>
int main(void){unsigned h,flags,freeze,end,fade,busy,start;int state;
 while(scanf("%u%d%u%u%u%u%u%u",&h,&state,&flags,&freeze,&end,&fade,&busy,&start)==8){
  uint32_t handle=h;TaxiGameAudioVehicleLoopInput i={state,(uint8_t)flags,(uint8_t)freeze,(uint8_t)end,(uint8_t)fade,(uint8_t)busy,start};TaxiGameAudioEngineRequest q[3];unsigned n=taxi_game_audio_vehicle_loop(&handle,&i,q);
  printf("%u %u",handle,n);for(unsigned j=0;j<n;j++)printf(" %u %u %d",q[j].op,q[j].handle,q[j].value);puts("");
 }return 0;}
