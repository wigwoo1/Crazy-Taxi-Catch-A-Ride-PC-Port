#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/attract_flow.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 TaxiAttractFlow s;taxi_attract_flow_begin(&s);
 for(int n=0;n<100;n++)assert(!taxi_attract_flow_step(&s,1,1,1,1)&&!s.elapsed);
 for(int n=0;n<6300;n++)assert(!taxi_attract_flow_step(&s,1,0,0,0)&&!s.blink_enabled);
 for(int n=0;n<2720;n++){
  int result=taxi_attract_flow_step(&s,1,0,0,0);
  assert(result==(n==2719));
 }
 assert(s.elapsed==9020&&s.phase==1&&s.exit_state==1&&s.exit_gate==2);
 assert(!taxi_attract_flow_step(&s,1,0,1,1)&&s.elapsed==9020);
 for(int button=0;button<2;button++){
  taxi_attract_flow_begin(&s);
  assert(taxi_attract_flow_step(&s,1,0,button==0,button==1)==1&&s.exit_state==1&&s.exit_gate==2);
 }
 puts("PASS original attract controller:105-second threshold,160 blink advances/17ticks, exit fade request, physical A/Start cancellation and fade/terminal gates; playback not integrated.");
}
