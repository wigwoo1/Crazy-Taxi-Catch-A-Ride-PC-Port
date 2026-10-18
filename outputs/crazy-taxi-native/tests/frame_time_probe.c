#include "../src/frame_time.h"
#include <stdio.h>
#include <assert.h>
int main(void) {
 unsigned previous,counter,elapsed,animation,page,freeze;
 while(scanf("%u %u %u %u %u %u",&previous,&counter,&elapsed,&animation,&page,&freeze)==6) {
  TaxiFrameTime s={previous,elapsed,animation,0,(uint8_t)page};
  assert(taxi_frame_time_update(&s,counter,(uint8_t)freeze)==0);
  printf("%u %d %u %u %u\n",s.previous_3524,s.delta_3b00,s.elapsed_755c,s.animation_1e10,s.page_7540);
 }
 return 0;
}
