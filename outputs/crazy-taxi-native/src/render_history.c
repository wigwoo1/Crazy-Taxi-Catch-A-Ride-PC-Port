#include "render_history.h"
#include <string.h>
void taxi_render_history_clear(TaxiRenderHistory *h){if(h)memset(h,0,sizeof(*h));}
int taxi_render_history_push(TaxiRenderHistory *h,uint64_t time,uint64_t tag){
 if(!h||h->count>TAXI_RENDER_HISTORY_CAPACITY||h->begin>=TAXI_RENDER_HISTORY_CAPACITY)return -1;
 if(h->count){unsigned last=(h->begin+h->count-1)%TAXI_RENDER_HISTORY_CAPACITY;if(time<=h->stamps[last].time_ns)return -1;}
 if(h->count==TAXI_RENDER_HISTORY_CAPACITY){h->begin=(h->begin+1)%TAXI_RENDER_HISTORY_CAPACITY;h->count--;}
 h->stamps[(h->begin+h->count)%TAXI_RENDER_HISTORY_CAPACITY]=(TaxiRenderStamp){time,tag};h->count++;return 0;
}
int taxi_render_history_bracket(const TaxiRenderHistory *h,uint64_t target,TaxiRenderBracket *out){
 if(!h||!out||!h->count||h->count>TAXI_RENDER_HISTORY_CAPACITY||h->begin>=TAXI_RENDER_HISTORY_CAPACITY)return -1;
 TaxiRenderStamp before=h->stamps[h->begin];TaxiRenderBracket result={before.tag,before.tag,0,1};
 if(target<=before.time_ns){*out=result;return 0;}
 for(unsigned n=1;n<h->count;n++){TaxiRenderStamp after=h->stamps[(h->begin+n)%TAXI_RENDER_HISTORY_CAPACITY];if(after.time_ns<=before.time_ns)return -1;
  if(target<=after.time_ns){result=(TaxiRenderBracket){before.tag,after.tag,(double)(target-before.time_ns)/(double)(after.time_ns-before.time_ns),0};*out=result;return 0;}before=after;}
 result=(TaxiRenderBracket){before.tag,before.tag,0,1};*out=result;return 0;
}
