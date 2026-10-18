#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/title_callback.h"
#include "../src/random.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 for(unsigned sound=0;sound<2;sound++)for(unsigned seed=1;seed<4096;seed++){
  uint32_t rng=seed,expected=seed;TaxiTitleCallback s={0};TaxiTitleCallbackOutput out;
  int index=-1;if(sound){taxi_random_range(&expected,0,65535);index=taxi_random_range(&expected,0,65535)&1;}
  assert(!taxi_title_callback(&s,&rng,sound,1,0,1,240,160,&out));
  assert(rng==expected&&s.sound_done&&out.sound_index==index&&!out.request_count);
  for(int i=1;i<34;i++){
   assert(!taxi_title_callback(&s,&rng,sound,1,0,0,240,160,&out));
   assert(out.sound_index==-1&&!out.request_count&&rng==expected);
  }
  assert(s.scroll.timer==34&&s.scroll.x_q8==272*256);
  assert(!taxi_title_callback(&s,&rng,sound,1,0,0,240,160,&out)&&out.request_count==4);
  assert(!taxi_title_callback(&s,&rng,sound,1,112,0,240,160,&out)&&!out.request_count);
  assert(!taxi_title_callback(&s,&rng,sound,1,0,1,240,160,&out)&&!out.request_count);
 }
 puts("PASS original title callback: one-shot sound/shared RNG order,34 motion calls before prompt, clock/fade gates; sound playback not verified.");
}
