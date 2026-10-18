#include "timing_tape.h"
static int same(const TaxiHostTime *a,const TaxiHostTime *b){
 return a->clock.phase==b->clock.phase&&a->counter==b->counter&&
 a->frame.previous_3524==b->frame.previous_3524&&
 a->frame.elapsed_755c==b->frame.elapsed_755c&&
 a->frame.animation_1e10==b->frame.animation_1e10&&
 a->frame.delta_3b00==b->frame.delta_3b00&&a->frame.page_7540==b->frame.page_7540;
}
int taxi_timing_tape_init(TaxiTimingTape *t,TaxiTimingRecord *r,size_t capacity,size_t count,int replay){
 if(!t||!r||!capacity||count>capacity||(replay!=0&&replay!=1)||(!replay&&count))return -1;
 *t=(TaxiTimingTape){.records=r,.count=count,.capacity=capacity,.replay=replay};return 0;
}
static int bad(TaxiTimingTape *t){if(t)t->failed=1;return -1;}
int taxi_timing_tape_advance(TaxiTimingTape *t,uint32_t site,TaxiHostTime *s,
 uint64_t ns,TaxiHostTickService service,void *context,uint32_t *ticks){
 if(!t||t->failed||!s||!service||!ticks||!site)return bad(t);
 if(!t->records||t->count>t->capacity||t->cursor>t->count)return bad(t);
 TaxiHostTime before=*s,predicted=*s;uint32_t expected;
 TaxiTimingRecord *r;
 if(t->replay){
  if(t->cursor==t->count)return bad(t);
  r=&t->records[t->cursor];
  if(r->site!=site||!same(s,&r->before))return bad(t);
  ns=r->ns;
 }else{
  if(t->count==t->capacity)return bad(t);
  r=&t->records[t->count];
 }
 /* Validate arithmetic and recorded post-state before dispatching services. */
 if(taxi_host_time_advance(&predicted,ns,&expected))return bad(t);
 if(t->replay&&(expected!=r->ticks||!same(&predicted,&r->after)))return bad(t);
 uint32_t actual;
 if(taxi_host_time_advance_serviced(s,ns,service,context,&actual))return bad(t);
 if(actual!=expected||!same(s,&predicted))return bad(t);
 if(!t->replay){
  *r=(TaxiTimingRecord){.site=site,.ticks=actual,.ns=ns,.before=before,.after=*s};
  t->count++;
 }
 t->cursor++;*ticks=actual;return 0;
}
int taxi_timing_tape_finish(const TaxiTimingTape *t){
 return t&&!t->failed&&t->cursor==t->count?0:-1;
}