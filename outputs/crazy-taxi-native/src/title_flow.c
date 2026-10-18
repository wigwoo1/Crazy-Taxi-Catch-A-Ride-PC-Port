#include "title_flow.h"
#include <string.h>
static int32_t wrap(uint32_t v){int32_t r;memcpy(&r,&v,4);return r;}
void taxi_title_flow_init(TaxiTitleFlow *s){if(s)*s=(TaxiTitleFlow){900,0,0};}
int taxi_title_flow_step(TaxiTitleFlow *s,int32_t delta,uint8_t fade,
 int complete,uint8_t confirm,uint8_t start,int sound,TaxiTitleActions *a){
 if(!s||!a)return -1;
 *a=(TaxiTitleActions){0,-1};
 if(s->remaining<=0||fade||!complete)return 0;
 s->remaining=wrap((uint32_t)s->remaining-(uint32_t)delta);
 s->elapsed=wrap((uint32_t)s->elapsed+(uint32_t)delta);
 if(confirm||start){s->remaining=0;s->menu_requested=1;a->begin_exit_fade=1;if(sound)a->sound_index=5;}
 else if(s->remaining<=0){s->remaining=0;a->begin_exit_fade=1;}
 return 0;
}
