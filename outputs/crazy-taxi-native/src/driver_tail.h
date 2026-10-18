#ifndef TAXI_DRIVER_TAIL_H
#define TAXI_DRIVER_TAIL_H
#include "acceleration.h"
#include <string.h>
/* Original 08011ea6..08011f30: after vehicle update, before world update. */
static inline int32_t taxi_tail_sub(int32_t a,uint32_t b){uint32_t u=(uint32_t)a-b;int32_t s;memcpy(&s,&u,4);return s;}
static inline int32_t taxi_driver_countdown(int32_t state,int32_t frame_delta){int32_t n=taxi_tail_sub(state,(uint32_t)frame_delta);return n>=0?n:0;}
/* Original 08011e38..08011ea4: byte effect counters use simulation delta. */
static inline void taxi_driver_effect_decay(TaxiAcceleration *v,int32_t delta,uint8_t freeze){
 if(freeze)return;
 uint8_t *counters[2]={&v->state_6a9,&v->state_6aa};
 for(unsigned i=0;i<2;i++)if(*counters[i]){
  int32_t n=taxi_tail_sub(*counters[i],(uint32_t)delta);
  *counters[i]=(uint8_t)(n>=0?n:0);
 }
}
static inline void taxi_driver_tail(TaxiAcceleration *v,int32_t frame_delta,int action2,int action3){
 int32_t *timers[3]={&v->gears.timer_e78,&v->gears.timer_e7c,&v->gears.timer_e80};
 for(unsigned i=0;i<3;i++){int32_t n=taxi_tail_sub(*timers[i],(uint32_t)frame_delta);*timers[i]=n>0?n:0;}
 uint32_t rate=action2&&!action3?3u:8u;v->scale_e84=taxi_tail_sub(v->scale_e84,(uint32_t)frame_delta*rate);if(v->scale_e84<=4095)v->scale_e84=4096;
}
#endif
