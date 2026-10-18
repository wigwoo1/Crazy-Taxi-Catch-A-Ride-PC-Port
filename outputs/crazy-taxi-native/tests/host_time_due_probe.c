#include "../src/host_time.h"
#include <stdio.h>
#include <inttypes.h>
int main(void){TaxiHostTime s={0};uint64_t ns;unsigned frozen;
 while(scanf("%" SCNu64 " %u",&ns,&frozen)==2){uint32_t ticks;
  if(taxi_host_time_advance(&s,ns,&ticks))return 1;int rc=taxi_host_time_present_due(&s,(uint8_t)frozen);if(rc<0)return 1;
  printf("%u %u %d %d %u %u %u\n",ticks,s.counter,rc,s.frame.delta_3b00,s.frame.elapsed_755c,s.frame.animation_1e10,s.frame.page_7540);
 }return 0;}
