#include "../src/host_time.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){unsigned previous,counter,minimum,freeze;
 while(scanf("%u %u %u %u",&previous,&counter,&minimum,&freeze)==4){
  TaxiHostTime state={.counter=counter,.frame={.previous_3524=previous,.elapsed_755c=123,.animation_1e10=44,.delta_3b00=7,.page_7540=1}},before=state;
  int result=taxi_host_time_present_after_ticks(&state,(unsigned char)freeze,minimum);
  if(result<=0)assert(!memcmp(&state,&before,sizeof state));
  assert(!memcmp(&state.clock,&before.clock,sizeof state.clock)&&state.counter==counter);
  printf("%d %u %d %u %u %u\n",result,state.frame.previous_3524,state.frame.delta_3b00,state.frame.elapsed_755c,state.frame.animation_1e10,state.frame.page_7540);
 }
 assert(taxi_host_time_present_after_ticks(NULL,0,1)==-1);return 0;
}
