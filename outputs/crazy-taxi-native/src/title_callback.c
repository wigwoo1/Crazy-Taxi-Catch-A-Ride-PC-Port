#include "title_callback.h"
#include "random.h"
int taxi_title_callback(TaxiTitleCallback *s,uint32_t *random,int sound,
 int32_t delta,uint32_t clock,uint8_t fade,uint16_t width,uint16_t height,
 TaxiTitleCallbackOutput *out){
 if(!s||!random||!out||!width||width>512)return -1;
 *out=(TaxiTitleCallbackOutput){.sound_index=-1};
 if(!s->sound_done){
  if(sound){
   /* Source first low bit is always<=202, but still consumes the draw. */
   (void)taxi_random_range(random,0,65535);
   out->sound_index=taxi_random_range(random,0,65535)&1;
  }
  s->sound_done=1;
 }
 int moving=taxi_title_scroll_step(&s->scroll,delta,width);
 if(moving<0)return -1;
 if(moving)return 0;
 int count=taxi_title_prompt(s->scroll.timer,clock,fade,width,height,out->requests);
 if(count<0)return -1;
 out->request_count=(unsigned)count;return 0;
}
