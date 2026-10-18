#include "attract_flow.h"
#include <string.h>
static int32_t add(int32_t a,int32_t b){uint32_t u=(uint32_t)a+(uint32_t)b;int32_t s;memcpy(&s,&u,4);return s;}
void taxi_attract_flow_begin(TaxiAttractFlow *s){if(s)*s=(TaxiAttractFlow){0};}
int taxi_attract_flow_step(TaxiAttractFlow *s,int32_t delta,
 uint8_t fade,uint8_t confirm,uint8_t start){
 if(!s)return -1;
 if(fade||s->exit_state)return 0;
 int begin_fade=0;
 s->elapsed=add(s->elapsed,delta);
 /* ROM084a59dc=60 is the CLOCK BASE, not a recording header. */
 if(s->elapsed>6300&&!s->exit_gate){
  if(!s->blink_enabled){s->blink_frame=0;s->blink_elapsed=0;}
  s->blink_enabled=1;s->blink_elapsed=add(s->blink_elapsed,delta);
  if(s->blink_elapsed>16){
   s->blink_elapsed=add(s->blink_elapsed,-17);
   s->blink_frame++;
   if(s->blink_frame==80){
    s->blink_frame=0;s->phase++;
    if(s->phase==2){s->phase=1;s->exit_state=1;s->exit_gate=2;begin_fade=1;}
   }
  }
 }
 if(confirm||start){s->exit_state=1;s->exit_gate=2;begin_fade=1;}
 return begin_fade;
}
