#include "screen_flow.h"
#include <string.h>
static int32_t wrapped(uint32_t value){int32_t result;memcpy(&result,&value,4);return result;}
int taxi_screen_flow_step(TaxiScreenFlow *s,int32_t delta,uint8_t flags,uint32_t callback,
 uint32_t height,uint8_t fade,uint8_t confirm,uint8_t start,uint8_t sound,
 uint16_t bg,uint16_t obj,TaxiScreenFlowCall call,void *context){
 if(!s||!call)return -1;
 if(s->remaining_359c<=0||fade||height!=160)return 0;
 if(flags&1)s->remaining_359c=wrapped((uint32_t)s->remaining_359c-(uint32_t)delta);
 s->elapsed=wrapped((uint32_t)s->elapsed+(uint32_t)delta);
 if((flags&2)&&(confirm||start)){
  if(sound&&call(context,s,0x080267c8,5,0,0,0))return -1;
  s->fade_1771=s->fade_1772=0;
  if(call(context,s,0x0800e5a8,0,32,bg,obj))return -1;
  s->remaining_359c=0;s->child_active_3898=0;
  return call(context,s,0x0802776c,0,0,0,0)?-1:0;
 }
 int exit=(flags&1)&&s->remaining_359c<=0;
 if(!exit&&!(flags&4)&&s->elapsed>59&&(confirm||start))exit=1;
 if(exit&&callback!=0x080167d1){
  s->fade_1771=s->fade_1772=0;
  if(call(context,s,0x0800e5a8,0,32,bg,obj))return -1;
  s->remaining_359c=0;
 }
 return 0;
}
