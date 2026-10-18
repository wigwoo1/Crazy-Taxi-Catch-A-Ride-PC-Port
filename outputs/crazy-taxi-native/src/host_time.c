#include "host_time.h"
int taxi_host_time_advance(TaxiHostTime *s,uint64_t ns,uint32_t *ticks){
 if(!s||!ticks)return -1;
 TaxiSimulationClock next=s->clock;uint32_t count;
 if(taxi_clock_advance(&next,ns,&count))return -1;
 s->clock=next;s->counter+=count;*ticks=count;return 0;
}
int taxi_host_time_advance_serviced(TaxiHostTime *s,uint64_t ns,
 TaxiHostTickService service,void *context,uint32_t *ticks){
 if(!s||!service||!ticks)return -1;
 TaxiSimulationClock next=s->clock;uint32_t count;
 if(taxi_clock_advance(&next,ns,&count))return -1;
 s->clock=next;
 for(uint32_t i=0;i<count;i++){
  s->counter+=1;service(context,s->counter);
 }
 *ticks=count;return 0;
}
int taxi_host_time_present(TaxiHostTime *s,uint8_t frozen){
 return s?taxi_frame_time_update(&s->frame,s->counter,frozen):-1;
}
int taxi_host_time_present_after_ticks(TaxiHostTime *s,uint8_t frozen,uint32_t minimum_ticks){
 if(!s||!minimum_ticks||minimum_ticks>60)return -1;
 uint32_t elapsed=s->counter-s->frame.previous_3524;
 if(elapsed<minimum_ticks)return 0;
 return taxi_host_time_present(s,frozen)?-1:1;
}
int taxi_host_time_present_due(TaxiHostTime *s,uint8_t frozen){
 return taxi_host_time_present_after_ticks(s,frozen,1);
}
