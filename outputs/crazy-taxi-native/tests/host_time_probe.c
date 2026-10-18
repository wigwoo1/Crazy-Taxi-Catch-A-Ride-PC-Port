#include "../src/host_time.h"
#include <stdio.h>
#include <inttypes.h>
int main(void){unsigned previous,elapsed,animation,page,freeze;uint64_t ns;
 while(scanf("%u %u %u %u %u %" SCNu64,&previous,&elapsed,&animation,&page,&freeze,&ns)==6){
  TaxiHostTime s={.counter=previous,.frame={previous,elapsed,animation,0,(uint8_t)page}};uint32_t ticks;
  if(taxi_host_time_advance(&s,ns/2,&ticks)||taxi_host_time_advance(&s,ns-ns/2,&ticks)||taxi_host_time_present(&s,(uint8_t)freeze))return 1;
  printf("%u %d %u %u %u\n",s.frame.previous_3524,s.frame.delta_3b00,s.frame.elapsed_755c,s.frame.animation_1e10,s.frame.page_7540);
 }return 0;}
