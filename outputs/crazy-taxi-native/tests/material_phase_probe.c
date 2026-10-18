#include "../src/material_phase.h"
#include <stdio.h>
#include <inttypes.h>
int main(void){uint32_t phase,freeze;int32_t delta;
 while(scanf("%" SCNu32 " %" SCNd32 " %" SCNu32,&phase,&delta,&freeze)==3){
  if(freeze>255)return 2;
  printf("%" PRIu32 "\n",taxi_material_phase_advance(phase,delta,(uint8_t)freeze));
 }return 0;
}
