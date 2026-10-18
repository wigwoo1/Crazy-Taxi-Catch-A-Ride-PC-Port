#include "../src/title_callback.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned done,sound,clock,fade,w,h,seed;int timer,x,delta;
 while(scanf("%u %u %d %d %d %u %u %u %u %u",&done,&sound,&timer,&x,&delta,&clock,&fade,&w,&h,&seed)==10){
  TaxiTitleCallback s={(uint8_t)done,{timer,x}};TaxiTitleCallbackOutput o;uint32_t rng=seed;
  if(taxi_title_callback(&s,&rng,(int)sound,delta,clock,(uint8_t)fade,(uint16_t)w,(uint16_t)h,&o))return 2;
  printf("%u %d %d %u %d %u",s.sound_done,s.scroll.timer,s.scroll.x_q8,rng,o.sound_index,o.request_count);
  for(unsigned i=0;i<o.request_count;i++){TaxiTitlePrompt *q=o.requests+i;printf(" %d %d %d %u %u %u %u %u %u",q->x,q->y,q->z,q->frame,q->group,q->priority,q->bucket,q->flags,q->palette);}
  puts("");
 }return 0;}
