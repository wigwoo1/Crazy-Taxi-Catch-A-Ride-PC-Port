#include "frame_time.h"
#include <string.h>
int taxi_frame_time_update(TaxiFrameTime *s,uint32_t counter,uint8_t freeze) {
 if(!s)return -1;
 uint32_t difference=counter-s->previous_3524;int32_t delta;
 memcpy(&delta,&difference,sizeof delta);
 if(delta>60)delta=60;
 if(delta<=0)delta=1;
 s->previous_3524=counter;s->delta_3b00=delta;
 if(!freeze)s->elapsed_755c+=(uint32_t)delta;
 s->animation_1e10+=4;s->page_7540=(uint8_t)((s->page_7540+1u)&1u);
 return 0;
}

int32_t taxi_frame_time_intro_delta(int32_t prior,int32_t frame,int16_t phase,uint8_t active,uint8_t fade,uint8_t exiting) {
 return active&&!fade&&!exiting&&phase==-1?frame:prior;
}
int32_t taxi_frame_time_normal_delta(int32_t prior,int32_t frame,uint8_t active,uint8_t freeze,uint8_t ended,uint8_t demo) {
 return !active&&!freeze&&!ended&&!demo?frame:prior;
}
